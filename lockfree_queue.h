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
			node<T>* dummy = new node<T>(T{});
			head_ = dummy;
			tail_ = dummy;
		}
		size_t size() const {
			return size_.load();
		}
		void enqueue(const T &value) {
			auto new_node = new node<T>(value);
			while (true) {
				node<T>* old_tail = tail_.load(std::memory_order_acquire);
				// 获取当前尾节点的下一个节点（可能为nullptr，也可能其他线程已插入但tail_未更新）
				node<T>* old_next = tail_.load(std::memory_order_acquire)->next;

				// 检查tail_是否在我们读取后被其他线程修改
				if (old_tail == tail_)
				{
					// 暂未修改tail
					if (old_next == nullptr)// （1）检查是否有其他线程添加了元素即修改了tail->next但是还未修改tail
					{
						// 未添加新元素，old_next仍有效
						// 通过CAS尝试将新节点设置为tail_->next(即完成插入)
						if (old_tail->next.compare_exchange_weak(old_next, new_node)) // 原子性再次检查old_next是否有效，有效则把new_node添加到queue
						{
							// 插入成功，下一步尝试推进tail_到新节点
							size_.fetch_add(1);
							// 提升性能：虽然tail_的更新不是必须，但可以减少下一线程retry次数
							tail_.compare_exchange_weak(old_tail, new_node);
								return;
						}

					}
					// 插入失败，其他线程抢先成功插入了，old_next已变化，进入下一轮循环重试
					else { 
						// 说明其他线程已经插入新节点，(old_next!=nullptr)，但还没有更新tail_
						// 原子判断tail_(1)如果还是nullptr，就赋值为old_next（此时old_next已赋值）
						// (2)如果其他线程更新了tail_，重新循环
						// 这里的old_next是别人的new_node，而不是本线程的new_node
						tail_.compare_exchange_weak(old_tail, old_next);// 协助推进tail_指针到新节点，优化整体队列推进速度
						// 无论成功与否，重新循环检查 tail_
					}
				}
			}
		}
	private:
		std::atomic<node<T>*> head_;
		std::atomic<node<T>*> tail_;
		std::atomic<size_t> size_;
	};
}
