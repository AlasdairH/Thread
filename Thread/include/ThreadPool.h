#pragma once

// cstdlib
#include <functional>
#include <vector>
#include <thread>
#include <future>
#include <condition_variable>
#include <queue>

// external libs

// program
#include "Logger.h"
#include "ThreadData.h"

// TODO: Doxygen


namespace Threads
{
	class ThreadPool
	{
	public:
		using Task = std::function<void()>;

		ThreadPool(const int _numThreads);
		~ThreadPool();

		template<class T>
		auto enqueue(T _task)->std::shared_future<decltype(_task())>
		{
			// contain a shared pointer to an abstract function
			auto wrapper = std::make_shared<std::packaged_task<decltype(_task()) ()>>(std::move(_task));
			// scope for mutex lock
			{
				std::unique_lock<std::mutex> lock(m_eventMutex);

				// emplace back a new task to the queue
				m_tasks.emplace([=]
				{
					(*wrapper)();
				});
			}
			// add the future of this task to the vector of futures
			m_futures.push_back(wrapper->get_future());
			// notify all 
			m_poolConditionalEvent.notify_all();
			return m_futures.back();
		}

		// check if a future is ready
		template<typename T>
		bool isReady(std::shared_future<T> const &_future)
		{
			if (_future.valid())
			{
				auto status = _future.wait_for(std::chrono::milliseconds(1));
				if (status == std::future_status::ready)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				LOG_ERROR("Unable to check if future is ready as it is invalid");
				return false;
			}
		}

		std::vector<ThreadData> pullCompletedThreads();

	protected:
		void start(const int _numThreads);
		void stop();

		std::queue<Task> m_tasks;
		std::vector<std::shared_future<ThreadData>> m_futures;

		std::vector<std::thread> m_threads;

		std::condition_variable m_poolConditionalEvent;
		std::mutex m_eventMutex;
		bool m_stopping = false;					// under the protection of m_eventMutex

	};
}
