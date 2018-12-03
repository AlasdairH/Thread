#include <iostream>
#include <string>

#include "ThreadPool.h"
#include "Timer.h"


#undef main

float threadedFunc()
{
	Timer time;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	return time.getDuration();
}

int main()
{
	Threads::ThreadPool threadPool;
	std::vector<std::future<float>> threadFutures;
	std::vector<float> resultFloats;
	// add 10 jobs to the thread queue
	for (int i = 0; i < 10; ++i)
	{
		threadPool.enqueue([=]
		{
			return threadedFunc();
		});
	}

	

	bool looping = true;
	while (looping)
	{
		for (unsigned int i = 0; i < threadFutures.size(); ++i)
		{
			if (threadFutures[i].valid())
			{
				if (threadPool.isReady(threadFutures[i]))
				{
					resultFloats.push_back(threadFutures[i].get());
					LOG_MESSAGE("Thread is complete, result: " << resultFloats.back());
				}
			}

		}
	}

	LOG_MESSAGE("Finished");
	return 0;
}