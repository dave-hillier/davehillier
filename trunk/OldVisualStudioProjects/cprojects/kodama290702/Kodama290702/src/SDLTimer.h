#ifndef __TIMER_H__
#define __TIMER_H__

class Timer  
{
public:
	float GetTime();
	float Delta();
	float frame_scale;
	void Update();
	Timer();
	virtual ~Timer();
private:
	unsigned long starttime;
	int framelength;
	unsigned long current_time;
};

extern Timer timer;

#endif