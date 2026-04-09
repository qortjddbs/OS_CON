#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

constexpr int MAX_THREADS = 6;

volatile int sum;
std::mutex sum_m;
std::atomic<int> sum_a;
volatile int array_sum[MAX_THREADS];

struct NUM {
	alignas(64) volatile int value;
};
NUM array_sum2[MAX_THREADS];

void worker_array2(const int th_id, const int loop)
{
	for (int i = 0; i < loop; ++i) {
		array_sum2[th_id].value = array_sum2[th_id].value + 2;
	}
}

void worker_array(const int th_id, const int loop)
{
	for (int i = 0; i < loop; ++i) {
		array_sum[th_id] = array_sum[th_id] + 2;
	}
}

//void worker(const int loop)
//{
//	for (int i = 0; i < loop; ++i) {
//		sum_m.lock();
//		sum += 2;
//		sum_m.unlock();
//	}
//}

void worker_nolock(const int loop)
{
	for (int i = 0; i < loop; ++i) {
		sum += 2;
	}
}

void worker_optimal(const int loop)
{
	volatile int local_sum = 0;
	for (int i = 0; i < loop; ++i) {
		local_sum = local_sum + 2;
	}
	sum_m.lock();
	sum = sum + local_sum;
	sum_m.unlock();
}

int main()
{
	std::cout << "Single thread execution:" << std::endl;
	auto start_t = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 5'0000'0000; ++i) {
		sum += 2;
	}
	auto end_t = std::chrono::high_resolution_clock::now();
	auto exec_t = end_t - start_t;
	auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();
	std::cout << "Execution time: " << exec_ms << " ms, ";
	std::cout << "Single thread, Sum: " << sum << std::endl;

	std::cout << "\nWithout Lock:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_nolock, 5'0000'0000 / num_threads);
		}
		for (auto& t : threads) {
			t.join();
		}
		auto end_t = std::chrono::high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();

		std::cout << "Execution time: " << exec_ms << " ms, ";
		std::cout << "Threads: " << num_threads << ", Sum: " << sum << std::endl;
	}

	std::cout << "\nArray:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		for (auto& v : array_sum) v = 0;
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_array, i, 5'0000'0000 / num_threads);
		}
		for (auto& t : threads) {
			t.join();
		}
		for (int i = 0; i < num_threads; ++i) {
			sum = sum + array_sum[i];
		}
		auto end_t = std::chrono::high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();

		std::cout << "Execution time: " << exec_ms << " ms, ";
		std::cout << "Threads: " << num_threads << ", Sum: " << sum << std::endl;
	}

	std::cout << "\nArray-2:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		for (auto& v : array_sum2) v.value = 0;
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_array2, i, 5'0000'0000 / num_threads);
		}
		for (auto& t : threads) {
			t.join();
		}
		for (int i = 0; i < num_threads; ++i) {
			sum = sum + array_sum2[i].value;
		}
		auto end_t = std::chrono::high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();

		std::cout << "Execution time: " << exec_ms << " ms, ";
		std::cout << "Threads: " << num_threads << ", Sum: " << sum << std::endl;
	}

	std::cout << "\nRewrite:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_optimal, 5'0000'0000 / num_threads);
		}
		for (auto& t : threads) {
			t.join();
		}
		auto end_t = std::chrono::high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();

		std::cout << "Execution time: " << exec_ms << " ms, ";
		std::cout << "Threads: " << num_threads << ", Sum: " << sum << std::endl;
	}
}