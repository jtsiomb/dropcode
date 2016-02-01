#include <algorithm>
#include <chrono>
#include "threadpool.h"

using namespace std::chrono;

ThreadPool::ThreadPool(int num_threads)
{
	quit = false;
	qsize = 0;
	nactive = 0;

	if(num_threads == -1) {
		num_threads = std::thread::hardware_concurrency();
	}

	printf("creating thread pool with %d threads\n", num_threads);

	thread = new std::thread[num_threads];
	for(int i=0; i<num_threads; i++) {
		thread[i] = std::thread(&ThreadPool::thread_func, this);

#ifdef _MSC_VER
		/* detach the thread to avoid having to join them in the destructor, which
		 * causes a deadlock in msvc implementation when called after main returns
		 */
		thread[i].detach();
#endif
	}
	this->num_threads = num_threads;
}

ThreadPool::~ThreadPool()
{
#ifdef _MSC_VER
	clear_work();
#endif

	quit = true;
	workq_condvar.notify_all();

	printf("ThreadPool: waiting for %d worker threads to stop ", num_threads);
	fflush(stdout);
#ifndef _MSC_VER
	for(int i=0; i<num_threads; i++) {
		thread[i].join();
		putchar('.');
		fflush(stdout);
	}
#else
	// spin until all threads are done...
	std::unique_lock<std::mutex> lock(workq_mutex);
	while(nactive > 0) {
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(128));
		putchar('.');
		fflush(stdout);
		lock.lock();
	}
#endif	// _MSC_VER

	putchar('\n');
	delete [] thread;
}

void ThreadPool::add_work(std::function<void ()> func)
{
	add_work(func, std::function<void ()>{});
}

void ThreadPool::add_work(std::function<void ()> work_func, std::function<void ()> done_func)
{
	std::unique_lock<std::mutex> lock(workq_mutex);
	workq.push_back(WorkItem{work_func, done_func});
	++qsize;
	workq_condvar.notify_all();
}

void ThreadPool::clear_work()
{
	std::unique_lock<std::mutex> lock(workq_mutex);
	workq.clear();
	qsize = 0;
}

int ThreadPool::queued() const
{
	std::unique_lock<std::mutex> lock(workq_mutex);
	return qsize;
}

int ThreadPool::active() const
{
	std::unique_lock<std::mutex> lock(workq_mutex);
	return nactive;
}

int ThreadPool::pending() const
{
	std::unique_lock<std::mutex> lock(workq_mutex);
	return nactive + qsize;
}

long ThreadPool::wait()
{
	auto start_time = steady_clock::now();

	std::unique_lock<std::mutex> lock(workq_mutex);
	done_condvar.wait(lock, [this](){ return nactive == 0 && workq.empty(); });

	auto dur = steady_clock::now() - start_time;
	return duration_cast<milliseconds>(dur).count();
}

long ThreadPool::wait(long timeout)
{
	auto start_time = steady_clock::now();
	duration<long, std::milli> dur, timeout_dur(std::max(timeout, 5L));

	std::unique_lock<std::mutex> lock(workq_mutex);
	while(timeout_dur.count() > 0 && (nactive > 0 || !workq.empty())) {
		if(done_condvar.wait_for(lock, timeout_dur) == std::cv_status::timeout) {
			break;
		}
		dur = duration_cast<milliseconds>(steady_clock::now() - start_time);
		timeout_dur = milliseconds(std::max(timeout, 5L)) - dur;
	}

	/*printf("waited for: %ld ms (%ld req) (na %d,qs %d,em %s)\n", dur.count(), timeout,
			nactive, qsize, workq.empty() ? "true" : "false");*/
	return dur.count();
}

void ThreadPool::thread_func()
{
	std::unique_lock<std::mutex> lock(workq_mutex);
	while(!quit) {
		workq_condvar.wait(lock);

		while(!quit && !workq.empty()) {
			WorkItem witem = workq.front();
			workq.pop_front();
			++nactive;
			--qsize;
			lock.unlock();

			witem.work();
			if(witem.done) {
				witem.done();
			}

			lock.lock();
			--nactive;
			done_condvar.notify_all();
		}
	}
}

