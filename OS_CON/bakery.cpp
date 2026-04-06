#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

std::atomic<int> sum;
std::mutex sum_m;

std::atomic<bool> flags[6] = { false, false, false, false, false, false };
std::atomic<int> label[6] = { 0, 0, 0, 0, 0, 0 };
std::atomic<int> num_threads;

void b_lock(const int th_id)
{
	flags[th_id] = true;
	int max_label = label[0];
	for (int i = 1; i < num_threads; ++i) if (label[i] > max_label) max_label = label[i];
	label[th_id] = max_label + 1;
	for (int i = 0; i < num_threads; ++i) {
		if (i == th_id) continue;
		while ((true == flags[i] && label[i] < label[th_id])
			|| label[i] == label[th_id] && i < th_id);
	}
}

void b_unlock(const int th_id)
{
	flags[th_id] = false;
}

void worker(const int loop)
{
	for (int i = 0; i < loop; ++i) {
		sum_m.lock();
		sum += 2;
		sum_m.unlock();
	}
}

void worker_bakery(const int th_id, const int loop)
{
	for (int i = 0; i < loop; ++i) {
		b_lock(th_id);
		sum += 2;
		b_unlock(th_id);
	}
}

std::atomic<bool> in_use;

void cae_lock()
{
	bool expected = false;
	while (false == std::atomic_compare_exchange_strong(&in_use, &expected, true))
		expected = false;
	in_use = true;
}

void cae_unlock()
{
	in_use = false;
}

void worker_CAE(const int th_id, const int loop)
{
	for (int i = 0; i < loop; ++i) {
		cae_lock();
		sum = sum + 2;
		cae_unlock();
	}
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

	std::cout << "\nWith Lock:\n";
	for (int num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker, 5000'0000 / num_threads);
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


	std::cout << "\nCAE:\n";
	for (num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_CAE, i, 5000'0000 / num_threads);
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

	std::cout << "\nBakery:\n";
	for (num_threads = 1; num_threads <= 6; num_threads++) {
		sum = 0;
		std::vector<std::thread> threads;
		auto start_t = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back(worker_bakery, i, 5000'0000 / num_threads);
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