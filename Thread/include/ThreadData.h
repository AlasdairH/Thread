#pragma once

// cstdlib

// external libs

// program

namespace Threads
{
	enum DataType { DATA_INT, DATA_FLOAT };

	struct ThreadData
	{
		float startTime;
		float endTime;
		
		float resultFloat;
	};
}