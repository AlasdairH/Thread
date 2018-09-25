#include <iostream>
#include <string>

#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"

#undef main

#define LOG_MESSAGE(text) std::cout << "MESSAGE: " << text << std::endl
#define LOG_WARNING(text) std::cout << "WARNING: " << text << std::endl
#define LOG_ERROR(text) std::cout << "ERROR: " << text << std::endl

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG_ERROR("SDL init failed");
		return -1;
	}
	else
	{
		LOG_MESSAGE("SDL init complete");
	}

	// wait
	int x; std::cin >> x;
	// end
	return 0;

}