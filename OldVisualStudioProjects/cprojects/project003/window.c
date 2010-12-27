#include "project003.h"

#include <gl/gl.h>
#include <gl/glu.h>

window_globals_t win;

view_thing_t view;

void getdisplaymodes (void);


BOOL bSetupPixelFormat(HDC hDC);
void ShutdownVid ();
void ScreenShot (char *filename);


long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;

	switch(message)
    {
	case WM_SYSKEYUP:
	case WM_KEYUP:
		KeyUp(wParam);
		break;
	case WM_KEYDOWN: 
    case WM_SYSKEYDOWN:
		KeyDown(wParam);
		break;

	case WM_ACTIVATE:
	    return TRUE;

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
        win.maindc = BeginPaint(win.hWndMain, &ps);
        EndPaint(win.hWndMain, &ps);
        return TRUE;

	case WM_DESTROY:		
		ShutdownVid();
		PostQuitMessage(0);
        break;
	case WM_MOUSEMOVE:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL InitWindow (HANDLE hInstance, int mode, 
				 bool fullscreen, int nCmdShow)
{
	WNDCLASS wc;
	HWND	hWndMain;

	win.hInstance = hInstance;
	win.fullscreen = fullscreen;

	switch (mode)
	{
	default:
	case VidMode320x200:
		win.width = 320;
		win.height = 200;
		break;
	case VidMode400x300:
		win.width = 400;
		win.height = 300;
		break;
	case VidMode640x480:
		win.width = 640;
		win.height = 480;
		break;
	case VidMode800x600:
		win.width = 800;
		win.height = 600;
		break;
	case VidMode1024x768:
		win.width = 1024;
		win.height = 768;
		break;
	case VidMode1600x1200:
		win.width = 1600;
		win.height = 1200;
		break;
	}
    wc.style = CS_DBLCLKS;

	wc.lpfnWndProc = WindowProc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE) hInstance;

	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	wc.hbrBackground = NULL;
	wc.lpszMenuName = NAME;
	wc.lpszClassName = NAME;


    if (!RegisterClass (&wc) ) 
	{
//		MessageBox(NULL, "Could not register window", "shit!", MB_OK);
		Error ("Could not register window");
        return FALSE; 
	}

	if (!win.fullscreen)	{
		hWndMain = CreateWindowEx(
			0,
			NAME, 
			TITLE,
			WS_OVERLAPPED |
			WS_CAPTION |
			WS_THICKFRAME,
			win.top_x,
			win.top_y,
			win.width,
			win.height,
			NULL,
			NULL,
			hInstance,
			NULL);
	}
	else 
	{
		hWndMain = CreateWindowEx(
			WS_EX_TOPMOST,
			NAME, 
			TITLE, 
			WS_VISIBLE | 
			WS_POPUP, 
			win.top_x,
			win.top_y,
			win.width,
			win.height,
			NULL,
			NULL,
			hInstance,
			NULL);
	}

	win.hWndMain = hWndMain;

    if (!win.hWndMain)
      return FALSE;

    ShowWindow(win.hWndMain, nCmdShow);
    UpdateWindow(win.hWndMain);

	win.maindc = GetDC(win.hWndMain);

	bSetupPixelFormat(win.maindc);

	win.baseRC = wglCreateContext( win.maindc );
	
	if (!win.baseRC)
	{
//		MessageBox(NULL, "Could not initialize GL", "shit!", MB_OK);
		Error ("Could not initialize GL");
		return FALSE;
	}
	if (!wglMakeCurrent( win.maindc, win.baseRC ))
	{
		//MessageBox(NULL, "wglMakeCurrent failed", "shit!", MB_OK);
		Error ("wglMakeCurrent failed");
		return FALSE;
	}

	if (fullscreen)
	{
		int i;
		getdisplaymodes();
		
		for (i = 0; i < win.nummodes; i++)
		{
			if ((win.width == win.modelist[i].pixelwidth) &&
				(win.height == win.modelist[i].pixelheight) &&
				(win.modelist[i].bitsperpixel >= 15))
			{

				win.devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				win.devmode.dmBitsPerPel = win.modelist[i].bitsperpixel;
				win.devmode.dmPelsWidth = win.modelist[i].pixelwidth;
				win.devmode.dmPelsHeight = win.modelist[i].pixelheight;
				win.devmode.dmSize = sizeof (win.devmode);

				if (ChangeDisplaySettings (&win.devmode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
				{
					ShowWindow(win.hWndMain, SW_SHOWNORMAL);
				//	MessageBox(NULL, "Next View", "Views", MB_OK);
					break;
				}
			}
		}
//		ChangeDisplaySettings (&win.devmode, CDS_FULLSCREEN);
//		ShowWindow(win.hWndMain, SW_SHOWNORMAL);
	}

	return TRUE;
}
void ShutdownVid ()
{	
	if (win.fullscreen)
		ChangeDisplaySettings (NULL, CDS_FULLSCREEN);

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( win.baseRC );
	ReleaseDC( win.hWndMain, win.maindc );

}
void getdisplaymodes (void)
{
	int i, nummodes = 0;
	DEVMODE	devmode;
	bool stat;

	for (i=0; i < MAX_MODES; i++)
	{
		stat = EnumDisplaySettings (NULL, i, &devmode);
		
		if (!stat)
			break;

		if ((devmode.dmBitsPerPel >= 15) &&
			(devmode.dmPelsWidth <= 1600) &&
			(devmode.dmPelsHeight <= 1200) &&
			(nummodes < MAX_MODES))
		{
			devmode.dmFields = DM_BITSPERPEL |
							   DM_PELSWIDTH |
							   DM_PELSHEIGHT;
			if (ChangeDisplaySettings (&devmode, CDS_TEST | CDS_FULLSCREEN) ==
					DISP_CHANGE_SUCCESSFUL)
			{
				win.modelist[i].modenum = i;
				win.modelist[i].bitsperpixel = devmode.dmBitsPerPel;
				win.modelist[i].pixelheight = devmode.dmPelsHeight;
				win.modelist[i].pixelwidth = devmode.dmPelsWidth;
			}
		/*	if ((win.width == win.modelist[i].pixelwidth) &&
				(win.height == win.modelist[i].pixelheight) &&
				(win.modelist[i].bitsperpixel >= 15))
			{
				win.devmode = devmode;
			}*/
		}
		nummodes++;
	}
	win.nummodes = nummodes;
}

BOOL bSetupPixelFormat(HDC hDC)
{
    static PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
	1,				// version number
	PFD_DRAW_TO_WINDOW 		// support window
	|  PFD_SUPPORT_OPENGL 	// support OpenGL
	|  PFD_DOUBLEBUFFER ,	// double buffered
	PFD_TYPE_RGBA,			// RGBA type
	24,					// 24-bit color depth
	0, 0, 0, 0, 0, 0,	// color bits ignored
	0,					// no alpha buffer
	0,					// shift bit ignored
	0,					// no accumulation buffer
	0, 0, 0, 0, 		// accum bits ignored
	32,					// 32-bit z-buffer	
	0,					// no stencil buffer
	0,					// no auxiliary buffer
	PFD_MAIN_PLANE,		// main layer
	0,					// reserved
	0, 0, 0				// layer masks ignored
    };
    int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0 )
    {
//        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
		Error ("ChoosePixelFormat failed");
        return FALSE;
    }

    if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
    {
//        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
		Error ("SetPixelFormat failed");
        return FALSE;
    }

    return TRUE;
}
