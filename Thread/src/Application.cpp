#include <iostream>
#include <string>

#include "ThreadPool.h"
#include "Timer.h"


#undef main

int random(const int _min, const int _max)
{
	return _min + (rand() % static_cast<int>(_max - _min + 1));
}

float threadedFunc(int _secondsToWait)
{
	Timer time;
	std::this_thread::sleep_for(std::chrono::seconds(_secondsToWait));
	return time.getDuration();
}

int main()
{
	srand(time(NULL));

	Threads::ThreadPool threadPool;

	std::vector<std::future<float>> threadFutures;
	std::vector<float> resultFloats;

	// add 10 jobs to the thread queue
	for (int i = 1; i < 10; ++i)
	{
		int randWait = random(1, 5);

		threadFutures.push_back(
		threadPool.enqueue([=]
		{
			return threadedFunc(randWait);
		})
		);
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