#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <list>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
private:
	int num_threads;
	std::thread *thread;	// array of threads

	struct WorkItem {
		std::function<void ()> work;
		std::function<void ()> done;
	};

	int qsize;
	std::list<WorkItem> workq;
	mutable std::mutex workq_mutex;
	std::condition_variable workq_condvar;

	int nactive;	// number of active workers (not sleeping)

	// condvar used by wait
	std::condition_variable done_condvar;

	bool quit;

	void thread_func();

public:
	// passing num_threads == -1 auto-detects based on number of processors
	explicit ThreadPool(int num_threads = -1);
	~ThreadPool();

	void add_work(std::function<void ()> func);
	void add_work(std::function<void ()> work_func, std::function<void ()> done_func);
	void clear_work();

	// returns the number of queued work items
	int queued() const;
	// returns the number of active threads
	int active() const;
	// returns number of pending work items (both in the queue and active)
	int pending() const;

	// waits for all work to be completed
	long wait();
	long wait(long timeout);
};

#endif	// THREAD_POOL_H_
