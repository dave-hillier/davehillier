#include <windows.h>
#include <gl/gl.h>
#include "resource.h"
#include "Types.h"
#include "timer.h"
#include "glapp.h"
#include "input.h"
#include "camera.h"
#include "Vector3.h"
#include "mp3.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE	hInst;								// current instance
HDC			hDC;
HGLRC		hGLRC; 
								// current device context
TCHAR szTitle[] = "Daves demo framework";		// The title bar text
TCHAR szWindowClass[] = "TESTGL";				// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

Mp3 testmp3;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	
	// Initialize global strings
	MyRegisterClass(hInstance);
	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	

	glapp.initgl();
	// Main message loop:
	// splash screen
	testmp3.Load("nerd.mp3");
	testmp3.Play();

	// load stuff here

	do
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
		input.Update();
		timer.Update();
		glapp.display();				// Draw The Scene
		SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
	}
	while(WM_QUIT != msg.message);

	testmp3.Stop();
	testmp3.Cleanup();
	
	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	
	hInst = hInstance; // Store instance handle in our global variable
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
		return FALSE;
	
	hDC = GetDC(hWnd);

	if (!hDC)
		return FALSE;
	
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
			1,                     // version number 
			PFD_DRAW_TO_WINDOW |   // support window 
			PFD_SUPPORT_OPENGL |   // support OpenGL 
			PFD_DOUBLEBUFFER,      // double buffered 
			PFD_TYPE_RGBA,         // RGBA type 
			24,                    // 24-bit color depth 
			0, 0, 0, 0, 0, 0,      // color bits ignored 
			0,                     // no alpha buffer 
			0,                     // shift bit ignored 
			0,                     // no accumulation buffer 
			0, 0, 0, 0,            // accum bits ignored 
			32,                    // 32-bit z-buffer     
			0,                     // no stencil buffer 
			0,                     // no auxiliary buffer 
			PFD_MAIN_PLANE,        // main layer 
			0,                     // reserved 
			0, 0, 0                // layer masks ignored 
	}; 
	int  iPixelFormat; 
	
	iPixelFormat = ChoosePixelFormat(hDC, &pfd);
	
	// make that the pixel format of the device context 
	SetPixelFormat(hDC, iPixelFormat, &pfd); 	
	
	hGLRC = wglCreateContext (hDC); 
 
	// make it the calling thread's current rendering context
	wglMakeCurrent (hDC, hGLRC);
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	return TRUE;
}


void Destroy ()
{
	// make the rendering context not current 
	wglMakeCurrent (NULL, NULL) ; 

	// delete the rendering context 
	wglDeleteContext (hGLRC); 
}
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)  // message that will be supported by the OS 
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	
	switch (message) 
	{
	case WM_MOUSEWHEEL:
		/*
		** this chunk of code theoretically only works under NT4 and Win98
		** since this message doesn't exist under Win95
		*/
		if ( ( short ) HIWORD( wParam ) > 0 )
		{
			input.Down( K_MWHEELUP);
			input.Up( K_MWHEELUP);
		}
		else
		{
			input.Down( K_MWHEELDOWN );
			input.Up( K_MWHEELDOWN );
		}
		break;
	case WM_LBUTTONDOWN:
		input.Down(K_MOUSE1);
		break;
	case WM_LBUTTONUP:
		input.Up(K_MOUSE1);
		break;
	case WM_RBUTTONDOWN:
		input.Down(K_MOUSE2);
		break;
	case WM_RBUTTONUP:
		input.Up(K_MOUSE2);
		break;
	case WM_MBUTTONDOWN:
		input.Down(K_MOUSE3);
		break;
	case WM_MBUTTONUP:
		input.Up(K_MOUSE3);
		break;
	case WM_SIZE:
		glapp.resize(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		input.Down(wParam);
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		input.Up(wParam);
	case WM_COMMAND:
		break;

	case WM_MOVE:
		break;
	case WM_PAINT:
	//	hDC = BeginPaint(hWnd, &ps);
	//	EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		Destroy();
		PostQuitMessage(0);
	//	exit(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
