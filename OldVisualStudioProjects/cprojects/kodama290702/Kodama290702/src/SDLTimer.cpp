#include "SDL.h"

#include "SDLTimer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timer timer;

Timer::Timer()
{
	starttime = SDL_GetTicks();
	current_time = SDL_GetTicks();	
	frame_scale = 1.0;
}

Timer::~Timer()
{

}

void Timer::Update()
{
	static unsigned long prev = SDL_GetTicks();
	unsigned long time = SDL_GetTicks();
	framelength = time - prev;
	framelength = (int)(framelength * frame_scale);
	current_time += framelength;

	prev = time;
}


// returns time since start in seconds
float Timer::GetTime()
{
	return (float)(current_time-starttime)/1000.0f;
}

float Timer::Delta()
{ 
	return (float)framelength/1000.0f; 
}