#include <windows.h>
#include "common.h"


vidmode_t modelist[MAX_VIDMODES];

HDC		ghdc;
HGLRC	baseRC;
HWND	mainwindow;
HINSTANCE ghInstance;
int currentmode = 0;
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int oldmx = 0, oldmy = 0, my = 0, mx = 0;

void RunFrame ()
{
	RenderFrame ();
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS    wc;
    MSG        msg; 
	ghInstance = hInstance;

    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    wc.lpfnWndProc   = (WNDPROC)WindowProc; 
    wc.cbClsExtra    = 0; 
    wc.cbWndExtra    = 0; 
    wc.hInstance     = hInstance; 
    wc.hIcon         = LoadIcon (hInstance, NAME); 
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL; 
    wc.lpszClassName = NAME; 
	
	
    if (!RegisterClass (&wc) ) 
        return FALSE; 
	

    mainwindow = CreateWindowEx ( 
		WS_EX_TOPMOST,
		NAME, 
		NAME,
		WS_OVERLAPPED | WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
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
	
	VidInit();

    while (true) { 
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
		RunFrame();	
    } 
	
	
}

long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
						   WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

	switch(message)
    {
	case WM_KEYDOWN: 
    case WM_SYSKEYDOWN:
		KeyDown(wParam);
		break;
	case WM_ACTIVATE:
		return 0;

	case WM_CREATE:
        break;

	case WM_MOVE:
		break;

	case WM_SIZE:

		break;

	case WM_MOUSEMOVE:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);

		if ((mx <= 0) || (my <= 0) || (mx >= WIDTH - 10) || (my >= HEIGHT - 10))
			SetCursorPos(WIDTH/2, HEIGHT/2);
		// make sure jerkiness is prevented
		oldmx = mx;
		oldmy = my;
		break;
	}
		
	case WM_ERASEBKGND:
        return TRUE;

	case WM_PAINT:
        ghdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return TRUE;

	case WM_DESTROY:
		// Remove gl
		VidShutdown();
		PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

