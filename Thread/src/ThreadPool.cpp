#include "ThreadPool.h"

namespace Threads
{

	ThreadPool::ThreadPool()
	{
		start(std::thread::hardware_concurrency());
	}

	ThreadPool::ThreadPool(const int _numThreads)
	{
		// start the thread pool with the specified number of threads
		start(_numThreads);
	}

	ThreadPool::~ThreadPool()
	{
		// stop the thread pool
		stop();
	}

	std::vector<ThreadData> ThreadPool::pullCompletedThreads()
	{
		std::vector<ThreadData> completedThreads;
		std::vector<unsigned int> indicesToRemove;

		// loop through all the current futures and find any that are ready
		for (unsigned int i = 0; i < m_futures.size(); ++i)
		{
			if (isReady(m_futures[i]))
			{
				// if the future is complete then add it to the data to send back
				completedThreads.push_back(m_futures[i].get());
				indicesToRemove.push_back(i);
			}
		}

		// reverse the vector of indices to remove as not to disrupt the order of the vector when removing
		std::reverse(indicesToRemove.begin(), indicesToRemove.end());
		for (unsigned int i = 0; i < indicesToRemove.size(); ++i)
		{
			m_futures.erase(m_futures.begin() + indicesToRemove[i]);
		}

		// return the completed thread data
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
					// a blank task
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
