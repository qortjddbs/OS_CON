#include <iostream>

int sum;

int main() {
	for (int i = 0; i < 5000'0000; ++i) {
		sum = sum + 2;
	}
	std::cout << sum << std::endl;
}