#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

volatile int sum;
std::mutex sum_m;
std::atomic<int> sum_a;

void worker(const int loop) 
{
	for (int i = 0; i < loop; ++i) {
		sum_m.lock();
		sum += 2;
		sum_m.unlock();
	}
}

void worker_nolock(const int loop) 
{
	for (int i = 0; i < loop; ++i) {
		sum += 2;
	}
}

void worker_atomic(const int loop) 
{
	for (int i = 0; i < loop; ++i) {
		sum_a += 2;
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
	for (int i = 0; i < 5000'00000; ++i) {
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
			threads.emplace_back(worker_nolock, 5000'00000 / num_threads);
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

	std::cout << "\nWith Lock:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker, 5000'00000 / num_threads);
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

	std::cout << "\nAtomic:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		sum_a = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_atomic, 5000'00000 / num_threads);
		}
		for (auto& t : threads) {
			t.join();
		}
		auto end_t = std::chrono::high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();

		std::cout << "Execution time: " << exec_ms << " ms, ";
		std::cout << "Threads: " << num_threads << ", Sum: " << sum_a << std::endl;
	}

	std::cout << "\nRewrite:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_optimal, 5000'00000 / num_threads);
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