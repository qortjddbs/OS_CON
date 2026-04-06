#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

int sum;
std::mutex sum_m;

void worker() {
	for (int i = 0; i < 2500'0000; ++i) {
		sum_m.lock();
		sum += 2;
		sum_m.unlock();
	}
}

int main() {
	std::thread t1{ worker };
	std::thread t2{ worker };
	t1.join();
	t2.join();
	std::cout << sum << std::endl;
}