#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>

#include <xutility>


Timer::Timer()
{
	usingQPF         = false;
	timerStopped     = true;
	QPFTicksPerSec  = 0;

	stopTime        = 0;
	lastElapsedTime = 0;
	baseTime        = 0;

	// Use QueryPerformanceFrequency() to get frequency of timer.  
	LARGE_INTEGER qwTicksPerSec;
	usingQPF = (bool) (QueryPerformanceFrequency( &qwTicksPerSec ) != 0);
	QPFTicksPerSec = qwTicksPerSec.QuadPart;
}


Timer::~Timer()
{
}
void Timer::reset()
{
	if( !usingQPF )
		return;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( stopTime != 0 )
		qwTime.QuadPart = stopTime;
	else
		QueryPerformanceCounter( &qwTime );

	baseTime        = qwTime.QuadPart;
	lastElapsedTime = qwTime.QuadPart;
	stopTime        = 0;
	timerStopped     = false;
}


//--------------------------------------------------------------------------------------
void Timer::start()
{
	if( !usingQPF )
		return;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( stopTime != 0 )
		qwTime.QuadPart = stopTime;
	else
		QueryPerformanceCounter( &qwTime );

	if( timerStopped )
		baseTime += qwTime.QuadPart - stopTime;
	stopTime = 0;
	lastElapsedTime = qwTime.QuadPart;
	timerStopped = false;
}


//--------------------------------------------------------------------------------------
void Timer::stop()
{
	if( !usingQPF )
		return;

	if( !timerStopped )
	{
		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( stopTime != 0 )
			qwTime.QuadPart = stopTime;
		else
			QueryPerformanceCounter( &qwTime );

		stopTime = qwTime.QuadPart;
		lastElapsedTime = qwTime.QuadPart;
		timerStopped = true;
	}
}


//--------------------------------------------------------------------------------------
void Timer::advance()
{
	if( !usingQPF )
		return;

	stopTime += QPFTicksPerSec/10;
}

double Timer::absoluteTime() const
{
	if( !usingQPF )
		return -1.0;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( stopTime != 0 )
		qwTime.QuadPart = stopTime;
	else
		QueryPerformanceCounter( &qwTime );

	double fTime = qwTime.QuadPart / (double) QPFTicksPerSec;

	return fTime;
}


//--------------------------------------------------------------------------------------
double Timer::currentTime() const
{
	if( !usingQPF )
		return -1.0;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( stopTime != 0 )
		qwTime.QuadPart = stopTime;
	else
		QueryPerformanceCounter( &qwTime );

	double fAppTime = (double) ( qwTime.QuadPart - baseTime ) / (double) QPFTicksPerSec;

	return fAppTime;
}


double Timer::elapsedTime()
{
	if( !usingQPF )
		return -1.0;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( stopTime != 0 )
		qwTime.QuadPart = stopTime;
	else
		QueryPerformanceCounter( &qwTime );

	double fElapsedTime = (double) ( qwTime.QuadPart - lastElapsedTime ) / (double) QPFTicksPerSec;
	lastElapsedTime = qwTime.QuadPart;

	return fElapsedTime;
}
