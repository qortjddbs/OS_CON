#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

volatile int sum;
volatile bool flag[2] = { false, false };
volatile int victim;
void p_lock(const int th_id)
{
	const int other = 1 - th_id;
	flag[th_id] = true;
	victim = th_id;
	// _asm mfence;
	std::atomic_thread_fence(std::memory_order_seq_cst);
	while (true == flag[other] && victim == th_id);
}

void p_unlock(const int th_id)
{
	flag[th_id] = false;
}

void worker(const int th_id) 
{
	for (int i = 0; i < 2500'0000; ++i) {
		p_lock(th_id);
		sum += 2;
		p_unlock(th_id);
	}
}

int main() 
{
	std::thread t1{ worker, 0 };
	std::thread t2{ worker, 1 };
	t1.join();
	t2.join();
	std::cout << sum << std::endl;
}