#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <chrono>

template <class T>
class Safe_queue {
private:
	std::queue<T> queue;
	std::mutex queue_mutex;
	std::condition_variable condvar;
public:
	void push(const T& func) {
		std::unique_lock<std::mutex> lk(queue_mutex);
		queue.push(func);
		condvar.notify_one();
	}

	void pop() {
		std::unique_lock<std::mutex> lk(queue_mutex);
		condvar.wait(lk,[this]{ return !queue.empty(); });
		auto func = queue.front();
		queue.pop();
		func();
	}

	bool is_empty() {
		return queue.empty();
	}

};

class Thread_pool {
private:
	std::vector<std::thread> threads;
	Safe_queue<std::function<void()>> sQueue;
	std::mutex mtx;
public:
	Thread_pool() {
		size_t threadcount = std::thread::hardware_concurrency();
		for (size_t i = 0; i < threadcount; ++i) {
			threads.push_back(std::thread(&Thread_pool::work, this));
		}
	}

	~Thread_pool() {
		for (auto& tr : threads) {
			tr.join();
		}
	}

	void work() {
		while (1) {
			if (!sQueue.is_empty()) {
				std::lock_guard<std::mutex> lk(mtx);
				std::cout << "Thread id: " << std::this_thread::get_id() << " working to: ";
				sQueue.pop();
			}
			else {
				std::this_thread::yield();
			}
		}
	}

	void submit(const std::function<void()>& func) {
		sQueue.push(func);
	}
};

void func1() {
	std::cout << "Function 1" << std::endl;
}

void func2() {
	std::cout << "Function 2" << std::endl;
}
int main()
{
	Thread_pool pool;
	for (int i = 0; i < 10; ++i) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		pool.submit(func1);
		pool.submit(func2);
	}
	return 0;
}
