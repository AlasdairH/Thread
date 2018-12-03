#include <iostream>
#include <string>

#include "ThreadPool.h"
#include "Timer.h"


#undef main

Threads::ThreadData threadedFunc()
{
	Timer time;
	Threads::ThreadData data;

	std::this_thread::sleep_for(std::chrono::seconds(1));
	data.resultFloat = 1.1f;
	
	data.duration = time.getDuration();
	return data;
}

int main()
{
	Threads::ThreadPool threadPool;
	// add 10 jobs to the thread queue
	for (int i = 0; i < 10; ++i)
	{
		threadPool.enqueue([=]
		{
			return threadedFunc();
		});
	}

	std::vector<float> floats;

	bool looping = true;
	while (looping)
	{
		std::vector<Threads::ThreadData> data;
		data = threadPool.pullCompletedThreads();
		if (data.size() > 0)
		{
			LOG_MESSAGE("Thread Data Found");

			for (unsigned int i = 0; i < data.size(); ++i)
			{
				floats.push_back(data[i].duration);
			}
		}

		if (floats.size() >= 10)
		{
			looping = false;
		}
	}

	LOG_MESSAGE("Finished");
	return 0;
}