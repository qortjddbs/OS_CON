#include <iostream>
#include <thread>
#include <atomic>

std::atomic <int> sum;

void worker() {
	for (int i = 0; i < 2500'0000; ++i) {
		sum += 2;
	}
}

int main() {
	std::thread t1{ worker };
	std::thread t2{ worker };
	t1.join();
	t2.join();
	std::cout << sum << std::endl;
}