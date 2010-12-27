// Timer.cpp: implementation of the Timer class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>

#include "input.h"
#include "Timer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timer timer;

Timer::Timer()
{
	starttime = GetTickCount();
	current_time = GetTickCount();	
	frame_scale = 1.0;
}

Timer::~Timer()
{

}

void Timer::Update()
{
	static unsigned long prev = GetTickCount();
	unsigned long time = GetTickCount();
	framelength = time - prev;
	framelength = (int)(framelength * frame_scale);
	current_time += framelength;

	prev = time;


	// TODO fix
	if (input.State('='))
		frame_scale += 0.1f;
	if (input.State('-'))
		frame_scale -= 0.1f;


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