#include <iostream>
#include <thread>

volatile bool g_ready = false;
volatile int g_data = 0;

void receiver()
{
	while (false == g_ready);
	std::cout << "Received data: " << g_data << std::endl;
}

void sender()
{
	g_data = 42;
	g_ready = true;
}

int main()
{
	std::thread t1{ receiver };
	std::thread t2{ sender };
	t1.join();
	t2.join();
}