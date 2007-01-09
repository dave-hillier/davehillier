#pragma once


class Timer
{
public:
	Timer();
	~Timer();

	void reset();
	void start();
	void stop();
	void advance();
	double absoluteTime() const;
	double currentTime() const;
	double elapsedTime();

	bool isStopped() const { return timerStopped; }

private:
	bool usingQPF;
	bool timerStopped;
	long long QPFTicksPerSec;

	long long stopTime;
	long long lastElapsedTime;
	long long baseTime;


};
