// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////


class Timer  
{
public:
	float GetTime();
	float Delta();
	void Update();
	Timer();
	virtual ~Timer();
private:
	unsigned long starttime;
	int framelength;
};

extern Timer timer;