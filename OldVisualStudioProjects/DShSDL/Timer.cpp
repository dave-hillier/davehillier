#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <assert.h>

#include <xutility>

namespace Base {

	 Timer &Timer::GlobalTimer() {
		static Timer *timer = NULL; 
		if (!timer)
			timer = new Timer();
		assert(timer);
		return *timer;

	}
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
	void Timer::Reset()
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
	void Timer::Start()
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
	void Timer::Stop()
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
	void Timer::Advance()
	{
		if( !usingQPF )
			return;

		stopTime += QPFTicksPerSec/10;
	}

	float Timer::AbsoluteTime() const
	{
		if( !usingQPF )
			return -1.0;

		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( stopTime != 0 )
			qwTime.QuadPart = stopTime;
		else
			QueryPerformanceCounter( &qwTime );

		float fTime = qwTime.QuadPart / (float) QPFTicksPerSec;

		return fTime;
	}


	//--------------------------------------------------------------------------------------
	float Timer::CurrentTime() const
	{
		if( !usingQPF )
			return -1.0;

		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( stopTime != 0 )
			qwTime.QuadPart = stopTime;
		else
			QueryPerformanceCounter( &qwTime );

		float fAppTime = (float) ( qwTime.QuadPart - baseTime ) / (float) QPFTicksPerSec;

		return fAppTime;
	}


	float Timer::ElapsedTime()
	{
		if( !usingQPF )
			return -1.0;

		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( stopTime != 0 )
			qwTime.QuadPart = stopTime;
		else
			QueryPerformanceCounter( &qwTime );

		float fElapsedTime = (float) ( qwTime.QuadPart - lastElapsedTime ) / (float) QPFTicksPerSec;
		lastElapsedTime = qwTime.QuadPart;

		return fElapsedTime;
	}

	void Timer::Update()
	{
		static double lastTime = Timer::GlobalTimer().CurrentTime();
		float currentTime = Timer::GlobalTimer().CurrentTime();

		deltaFrame = currentTime - lastTime;
		deltaFrame = std::max(deltaFrame, 0.0);

		lastTime = currentTime;
	}

}