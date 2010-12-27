// Timer.cpp: implementation of the Timer class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>

#include "Timer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timer timer;
// TODO slow mo timer etc
Timer::Timer()
{
	starttime = GetTickCount();
}

Timer::~Timer()
{

}

void Timer::Update()
{
	static unsigned long prev = starttime;
	unsigned long time = GetTickCount();
	framelength = time - prev;
	prev = time;
}


// returns time since start in seconds
float Timer::GetTime()
{
	return (float)(GetTickCount()-starttime)/1000.0f;
}

float Timer::Delta()
{ 
	return (float)framelength/1000.0f; 
}