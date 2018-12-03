#pragma once

// cstdlib
#include <iostream>
#include <cstdio>
#include <ctime>

// external libs

// program
#include "Logger.h"

// TODO: Doxygen

class Timer
{
public:
	Timer() { m_start = std::clock(); }
	float getDuration() { return (std::clock() - m_start) / (float)CLOCKS_PER_SEC; }

protected:
	std::clock_t m_start;
};
