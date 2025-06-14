// lockfree_queue.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <atomic>
namespace lockfree {
	template<typename T>
	struct node {
		T data;
		std::atomic<node*>next;
		node(const T & value) :data(value),next(nullptr) {}
	};


	template<typename T>
	class lockfree_queue {
	public:
		lockfree_queue() {

		}
	private:
		std::atomic<node<T>*> head_;
		std::atomic<node<T>*> tail_;
	};
}
