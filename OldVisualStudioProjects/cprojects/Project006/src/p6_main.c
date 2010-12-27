// Lib headers
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "p6_common.h"

// defines
#define APPNAME "Project Six"

// Globals 
// hopefully i shouldnt need to access these anywhere
// else
HDC				ghDC;
HGLRC			ghRC;
HWND			ghWnd;
HINSTANCE		ghInstance;

bool fullscreen = false;
int height= 480, width = 640;

// prototypes
long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
						   WPARAM wParam, LPARAM lParam);
void RunFrame ();
// p6_glmain.c
BOOL bSetupPixelFormat(HDC hdc);
GLvoid resize( GLsizei width, GLsizei height );
GLvoid initializeGL(GLsizei, GLsizei); 
GLvoid display(GLvoid); 
// p6_timing.c
void CountFPS (void);
// p6_text.c
void initText();

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS	wc;
	MSG			msg;

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	wc.lpfnWndProc		= (WNDPROC)WindowProc; 
	wc.cbClsExtra		= 0; 
	wc.cbWndExtra		= 0; 
	wc.hInstance		= hInstance; 
	wc.hIcon			= LoadIcon (hInstance, APPNAME); 
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL; 
	wc.lpszClassName	= APPNAME; 
	
	
	if (!RegisterClass (&wc) ) 
		return FALSE; 

	ghWnd = CreateWindowEx ( 
		fullscreen ? WS_EX_TOPMOST : 0,
		APPNAME, 
		APPNAME,
		fullscreen ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPEDWINDOW,//WS_CAPTION,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		(HINSTANCE)hInstance,
		NULL);

	if (!ghWnd) 
		return FALSE; 
	
	/* show and update main window */ 
	ShowWindow (ghWnd, nCmdShow); 
	UpdateWindow (ghWnd);

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
	RECT rect; 
 
	switch(message)
	{
	case WM_CREATE: 
		ghDC = GetDC(hWnd); 
		if (!bSetupPixelFormat(ghDC)) 
			PostQuitMessage (0); 
 
		ghRC = wglCreateContext(ghDC); 
		if (!ghRC)
		{
			MessageBox(NULL, "Could not initialize GL", "ERROR", MB_OK);
			PostQuitMessage (0); 
		}
		if (!wglMakeCurrent(ghDC, ghRC))
		{
			MessageBox(NULL, "wglMakeCurrent failed", "ERROR", MB_OK);
			PostQuitMessage (0); 
		}
		GetClientRect(hWnd, &rect); 
		initializeGL(rect.right, rect.bottom); 
		// Other Initialisation should go here
		initText();
		TimeInit();
		TerrainInit("hmap.raw");

		break; 
	case WM_PAINT:
		ghDC = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return TRUE;

	case WM_SIZE: 
		GetClientRect(hWnd, &rect); 
		resize(rect.right, rect.bottom); 
		break; 

	case WM_CLOSE: 
		if (ghRC) 
			wglDeleteContext(ghRC); 
		if (ghDC) 
			ReleaseDC(hWnd, ghDC); 
		ghRC = 0; 
		ghDC = 0; 
		DestroyWindow (hWnd); 
		break; 
 
	case WM_DESTROY: 
		if (ghRC) 
			wglDeleteContext(ghRC); 
		if (ghDC) 
			ReleaseDC(hWnd, ghDC); 
		PostQuitMessage (0); 
		break; 

	case WM_KEYDOWN: 
		KeyDown(wParam);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);

}

BOOL bSetupPixelFormat(HDC hDC)
{
    static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,				// version number
		PFD_DRAW_TO_WINDOW 		// support window
		|  PFD_SUPPORT_OPENGL 	// support OpenGL
		|  PFD_DOUBLEBUFFER ,	// double buffered
		PFD_TYPE_RGBA,//COLORINDEX// RGBA type
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
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
    {
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    return TRUE;
}

void RunFrame ()
{
	CountFPS();

	// keep till last
	display();
	SwapBuffers(ghDC);
}