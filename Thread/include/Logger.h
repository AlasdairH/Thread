#pragma once

#include <iostream>

#define DEBUG

#ifdef DEBUG
#define LOG_MESSAGE(_text) std::cout << "MESSAGE: " << _text << "\n"
#define LOG_WARNING(_text) std::cout << "WARNING: " << _text << "\n"
#define LOG_ERROR(_text) std::cout << "ERROR: " << _text << "\n"
#endif