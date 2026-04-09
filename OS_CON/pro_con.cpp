#include <iostream>
#include <thread>
#include <queue>
#include <mutex>

constexpr long long LOOP = 100'0000;
constexpr long long SUM = LOOP * (LOOP - 1) / 2;

std::queue<int> buffer;
std::atomic<long long> sum;

void producer()
{
	for (int i = 0;i < LOOP; ++i) buffer.push(i);
}

void consumer()
{
	for (int i = 0;i < LOOP; ++i) {
		while (true == buffer.empty());
		sum = sum + buffer.front();
		buffer.pop();
	}
}

class ATOMIC_QUEUE {
private:
	std::queue<int> q;
	std::mutex ql;
public:
	void push(int val) {
		ql.lock();
		q.push(val);
		ql.unlock();
	}
	bool try_pop(int* value) {
		ql.lock();
		if (true == q.empty()) {
			ql.unlock();
			return false;
		}
		*value = q.front();
		q.pop();
		ql.unlock();
		return true;
	}
};

ATOMIC_QUEUE a_buffer;

void a_producer()
{
	for (int i = 0;i < LOOP; ++i) a_buffer.push(i);
}

void a_consumer()
{
	for (int i = 0;i < LOOP; ++i) {
		int val;
		while (false == a_buffer.try_pop(&val));
		sum += val;
	}
}

int main()
{
	{
		std::cout << "Producer - Consumer without synchronization\n";
		sum = 0;
		// Producer #1
		for (int i = 0;i < LOOP; ++i) buffer.push(i);
		// Producer #2
		for (int i = 0;i < LOOP; ++i) buffer.push(i);
		// Consumer #1
		for (int i = 0;i < LOOP; ++i) {
			sum = sum + buffer.front();
			buffer.pop();
		}
		// Consumer #2
		for (int i = 0;i < LOOP; ++i) {
			sum = sum + buffer.front();
			buffer.pop();
		}

		if (sum == SUM * 2)
			std::cout << "Correct : " << sum << std::endl;
		else
			std::cout << "InCorrect : " << sum << std::endl;
	}

	{
		std::cout << "\nMulti Thread Producer - Consumer\n";
		sum = 0;
		std::thread p1{ a_producer };
		std::thread p2{ a_producer };
		std::thread c1{ a_consumer };
		std::thread c2{ a_consumer };
		p1.join();
		p2.join();
		c1.join();
		c2.join();

		if (sum == SUM * 2)
			std::cout << "Correct : " << sum << std::endl;
		else
			std::cout << "InCorrect : " << sum << std::endl;
	}
}