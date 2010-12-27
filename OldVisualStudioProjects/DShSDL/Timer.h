#pragma once

namespace Base 
{
	class Timer
	{
	public:
		static Timer &GlobalTimer();
		Timer(void);
		~Timer(void);

		void Reset();
		void Start();
		void Stop();
		void Advance();
		float AbsoluteTime() const;
		float CurrentTime() const;
		float ElapsedTime();

		bool IsStopped() const { return timerStopped; }

		void Update();

		double FrameDelta() { return deltaFrame; }

	private:
		bool usingQPF;
		bool timerStopped;
		long long QPFTicksPerSec;

		long long stopTime;
		long long lastElapsedTime;
		long long baseTime;

		double deltaFrame;

	};

} 