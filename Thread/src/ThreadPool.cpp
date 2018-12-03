#include "ThreadPool.h"

namespace Threads
{

	ThreadPool::ThreadPool(const int _numThreads)
	{
		start(_numThreads);
	}

	ThreadPool::~ThreadPool()
	{
		stop();
	}

	std::vector<ThreadData> ThreadPool::pullCompletedThreads()
	{
		std::vector<ThreadData> completedThreads;
		for (auto &future : m_futures)
		{
			if (isReady(future))
			{
				completedThreads.push_back(future.get());
			}
		}
		return completedThreads;
	}

	void ThreadPool::start(const int _numThreads)
	{
		for (int i = 0; i < _numThreads; ++i)
		{
			// create a new thread with a lambda
			m_threads.emplace_back([=]
			{
				// This code is run per thread
				while (true)
				{
					Task task;

					// scope for mutex. We do not want the mutex locked while the task is running. It could be a while.
					{
						// the lock is created around the event mutex
						std::unique_lock<std::mutex> lock(m_eventMutex);

						// wait unlocks the mutex and waits for the conditional event to be notified AND for the predicate to be true
						// waits for the thread pool to stop OR for a new task to be added to the pool
						m_poolConditionalEvent.wait(lock, [=] { return m_stopping || !m_tasks.empty(); });

						// if the thread pool is stopping and there is no tasks then break out of the while loop
						if (m_stopping && m_tasks.empty())
							break;

						// otherwise take the first task and assign it to this thread
						task = std::move(m_tasks.front());
						m_tasks.pop();
					}

					// if we've got to here then this thread has been assigned and we need to run it
					task();

				}
			});
		}

		LOG_MESSAGE("Created thread pool with " << _numThreads << " threads");
	}

	void ThreadPool::stop()
	{
		LOG_MESSAGE("Stopping Thread Pool...");

		// scope for mutex lock
		{
			std::unique_lock<std::mutex> lock(m_eventMutex);
			m_stopping = true;
		}

		// notify all threads
		m_poolConditionalEvent.notify_all();
		// loop through all the treads and wait for them to stop execution
		for (auto &thread : m_threads)
			thread.join();

		LOG_MESSAGE("Stopped Thead Pool");
	}

}
