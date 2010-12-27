#include <windows.h>

LARGE_INTEGER	performance; 
LARGE_INTEGER	now; 
LARGE_INTEGER	freq;
float res; 

float fps;
int frames;// loop round?

double starttime;
 
float CurrentTime (void)
{
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);
	return (temp.QuadPart - performance.QuadPart) * res;
}

void TimeInit (void)
{
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&performance);
	res = (float) (1.0f / freq.QuadPart);
	starttime = CurrentTime();

}

void TimeUpdate(void)
{
	QueryPerformanceCounter(&now);
}

float TimePassed(void)
{
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);

	return (temp.QuadPart - now.QuadPart) * res * 1000.0f;
}
void CountFPS (void)
{
	frames++;

	// is this correct?
	if (TimePassed() >= 1000)
	{
		fps = (fps + frames) / 2; 
		TimeUpdate();
		frames = 0;
	}
}
