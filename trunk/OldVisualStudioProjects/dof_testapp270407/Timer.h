
#pragma once


#include <SDL.h>

class Timer
{
public:
	virtual ~Timer() {}

	unsigned getTime() const { return SDL_GetTicks(); }
};