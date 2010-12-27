
//
#include "project001.h"

HDC   ghdc; 
HGLRC ghrc; 
extern HWND	mainwindow;
static double		pfreq;
double		curtime = 0.0;
static double		lastcurtime = 0.0;
static int			lowshift;

long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void vid_start (void);
void draw (void);
double Time (void);
void InitTime(void);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS    wc;
	double			time, oldtime, newtime;

    MSG        msg; 

    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    wc.lpfnWndProc   = (WNDPROC)WindowProc; 
    wc.cbClsExtra    = 0; 
    wc.cbWndExtra    = 0; 
    wc.hInstance     = hInstance; 
    wc.hIcon         = LoadIcon (hInstance, NAME); 
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL; 
    wc.lpszClassName = NAME; 
 
	
    if (!RegisterClass (&wc) ) 
        return FALSE; 
 	

    mainwindow = CreateWindowEx ( 
		   WS_EX_TOPMOST,
		   NAME, 
           NAME,
		   WS_OVERLAPPED |
		   WS_CAPTION |
		   WS_THICKFRAME,
           0,
           0,
           WIDTH,
		   HEIGHT,
           NULL,
           NULL,
           (HINSTANCE)hInstance,
           NULL);

    if (!mainwindow) 
        return FALSE; 
 
    /* show and update main window */ 
    ShowWindow (mainwindow, nCmdShow); 
 
    UpdateWindow (mainwindow);
        
	vid_start();
	InitTime();
	oldtime = Time();

    while (1) { 
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } else { 
                return TRUE; 
            } 
        } 
    	newtime = Time ();
		time = newtime - oldtime;
		oldtime = newtime;
		draw(); 
    } 
 
 
}

long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

	switch(message)
    {
	 case WM_ACTIVATE:
	    return 0;

     case WM_CREATE:
        break;

     case WM_MOVE:
	    break;

     case WM_SIZE:
	    break;

     case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

	 case WM_ERASEBKGND:
        return TRUE;

     case WM_PAINT:
        ghdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return TRUE;

     case WM_DESTROY:
		 // Remove gl
		ScreenShot();
		vid_end();
		printp(particles);
	   	PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void InitTime()
{
	LARGE_INTEGER	PerformanceFreq;
	unsigned int	lowpart, highpart;

	if (!QueryPerformanceFrequency (&PerformanceFreq))
		MessageBox (NULL, "No hardware timer available", "Error", MB_OK);

	lowpart = (unsigned int)PerformanceFreq.LowPart;
	highpart = (unsigned int)PerformanceFreq.HighPart;
	lowshift = 0;

	while (highpart || (lowpart > 2000000.0))
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / (double)lowpart;
	
	lastcurtime = curtime = 0.0;
}

double Time (void)
{
	static int			sametimecount;
	static unsigned int	oldtime;
	static int			first = 1;
	LARGE_INTEGER		PerformanceCount;
	unsigned int		temp, t2;
	double				time;

	QueryPerformanceCounter (&PerformanceCount);

	temp = ((unsigned int)PerformanceCount.LowPart >> lowshift) |
		   ((unsigned int)PerformanceCount.HighPart << (32 - lowshift));

	if (first)
	{
		oldtime = temp;
		first = 0;
	}
	else
	{
	// check for turnover or backward time
		if ((temp <= oldtime) && ((oldtime - temp) < 0x10000000))
		{
			oldtime = temp;	// so we can't get stuck
		}
		else
		{
			t2 = temp - oldtime;

			time = (double)t2 * pfreq;
			oldtime = temp;

			curtime += time;

			if (curtime == lastcurtime)
			{
				sametimecount++;

				if (sametimecount > 100000)
				{
					curtime += 1.0;
					sametimecount = 0;
				}
			}
			else
			{
				sametimecount = 0;
			}

			lastcurtime = curtime;
		}
	}

    return curtime;
}
