// lockfree_queue.cpp: 定义应用程序的入口点。
//

#include "lockfree_queue.h"
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>
#include <cassert>
using namespace std;
template< size_t thread_num,size_t times, typename Queue>
void benchmark(Queue & queue){
	std::vector<std::thread> threads;
	auto task = [&queue]() {
		for (int i = 0; i < times; ++i)
		{
			queue.enqueue(i);
		}
	};
	auto begin = std::chrono::steady_clock::now();
	for (int i = 0; i < thread_num; ++i) {
		threads.emplace_back(task);
	}
	for (auto& i : threads) {
		i.join();
	}
	assert(queue.size() == (times * thread_num), "无锁队列插入出错");
	auto end = std::chrono::steady_clock::now();
	std::cout << "Cost:" << std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count() << "ms\n";
}
int main()
{
	lockfree::lockfree_queue<int> queue;
	benchmark<10,10000>(queue);
	return 0;
}
