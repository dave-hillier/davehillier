#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

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

	double Timer::AbsoluteTime() const
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
	double Timer::CurrentTime() const
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


	double Timer::ElapsedTime()
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

	void Timer::Update()
	{
		static double lastTime = Timer::GlobalTimer().CurrentTime();
		double currentTime = Timer::GlobalTimer().CurrentTime();

		deltaFrame = currentTime - lastTime;
		deltaFrame = std::max(deltaFrame, 0.0);

		lastTime = currentTime;
	}

	// TODO MSDN suggestes refreshing the frequency every few frames
	//// TODO Implement a FPS counter? Perhaps this should be a subclass or seperate class
	//void TimeUpdate(void)
	//{
	//	QueryPerformanceCounter(&now);
	//}

	//float TimePassed(void)
	//{
	//	LARGE_INTEGER temp;
	//	QueryPerformanceCounter(&temp);

	//	return (temp.QuadPart - now.QuadPart) * res * 1000.0f;
	//}
	//// TODO  Should probably be on a per-frame basis so as to not cause leaps for the odd glitch
	//void CountFPS (void)
	//{
	//	frames++;

	//	// is this correct?
	//	if (TimePassed() >= 1000)
	//	{
	//		fps = (fps + frames) / 2; 
	//		TimeUpdate();
	//		frames = 0;
	//	}
	//}



}