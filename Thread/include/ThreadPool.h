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


namespace Threads
{
	/*! @class ThreadPool
	*	@brief A Thread Pool that can be assisnged jobs through lambas or function pointers
	*
	*	Creates a threadpool with a specified number of threads. Jobs can be enqueued to the pool and will be assigned a worker thread
	*	as soon as one is available. 
	*/
	class ThreadPool
	{
	public:
		using Task = std::function<void()>;

		/** @brief ThreadPool Constructor
		*
		*	Creates a threadpool with a thread for every CPU core on the system
		*/
		ThreadPool();
		/** @brief ThreadPool Constructor
		*	@param _numThreads The number of worker threads to create
		*
		*	Creates a thread pool with the specified number of threads
		*/
		ThreadPool(const int _numThreads);

		/** @brief ThreadPool Deconstructor
		*
		*	Waits for all work to be done on each core and then stops each thread
		*/
		~ThreadPool();

		/** @brief Enqueues a task to the thread pool 
		*	@param _task The task to be queued to the pool
		*
		*	Adds a task to the pool to be executed when there is a free worker thread
		*/
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

		/** @brief Takes a future and tests to see if the task it is associated with has finished
		*	@param _future The future to test
		*
		*	Takes a future and tesks to see if the future has completed execution and the result can be extracted.
		*/
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

		/** @brief Checks the pool's futures for any completed tasks and returns a vector of the completed tasks
		*
		*	Looks through all the futures of the thread pool to check if any have been completed and then returns the data from those
		*	completed tasks
		*/
		std::vector<ThreadData> pullCompletedThreads();

	protected:
		/** @brief Starts the thread pool. Called by constructor.
		*	@param _numThreads The number of threads to start the pool with
		*
		*	Starts the thread pool with the specified number of threads.
		*	This method is called on construction of the ThreadPool.
		*/
		void start(const int _numThreads);

		/** @brief Stops the thread pool
		*
		*	Waits for execution of tasks to complete and then shuts down all threads.
		*/
		void stop();

		std::queue<Task>								m_tasks;					/**< The queue to tasks to be executed */
		std::vector<std::shared_future<ThreadData>>		m_futures;					/**< The vector of futures both complete and incomplete */

		std::vector<std::thread>						m_threads;					/**< Vector of all worker threads */

		std::condition_variable							m_poolConditionalEvent;		/**< The thread pool conditional event that holds threads before they get a job */
		std::mutex										m_eventMutex;				/**< The mutex which controlls access to the task queue */
		bool											m_stopping = false;			/**< A flag for the state of the thread pool, if set to true the pool will stop */

	};
}
