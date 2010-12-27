
// Libraries required
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


// Library headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <assert.h>

// Includes
#include "Common.h"
#include "Timer.h"
#include "FirstPersonCamera.h"
#include "DebugLog.h"

FirstPersonCamera TestCam;

// TODO List

// Basic fonts
// Logging
// 3d maths
// Camera PRE Matrix/Vectors <- pinch the cam from D3D SDK empty project
// Input keyboard/mouse/joypad

// Texture manager? 
// Threads? <- use boost
// Multi threaded resource manager to handle errors and async loading

// Terrain
// Characters
// Static objects?
// Collision detection
// TODO Move to seperate file
// TODO remove win32 specifics from interface
namespace Base {

	class GLContext
	{
	public:
		GLContext() : renderContext(NULL)
		{

		}
		~GLContext()
		{
		}

		bool Destroy()
		{
			if (renderContext) 
			{
				wglDeleteContext(renderContext); 
				renderContext = NULL;
			}

			return true;
		}

		bool Initialize(HDC deviceContext, HWND hWnd)
		{
			bool result = true;
			if (!SetupPixelFormat(deviceContext)) 
				result = false; 

			renderContext = wglCreateContext(deviceContext); 
			if (!renderContext)
			{
				MessageBox(NULL, "Could not initialize GL", "ERROR", MB_OK);
				PostQuitMessage (0); 
			}
			if (!wglMakeCurrent(deviceContext, renderContext))
			{
				MessageBox(NULL, "wglMakeCurrent failed", "ERROR", MB_OK);
				PostQuitMessage (0); 
			}
			RECT rect; 
			GetClientRect(hWnd, &rect); 
			Resize(rect.right, rect.bottom); 

			return result;
		}

		void Resize( uint w, uint h ) 
		{ 
			glViewport( 0, 0, w, h ); 

			glMatrixMode( GL_PROJECTION ); 
			
			float aspect = (float) w / h; 
			const float *matrix = (const float*)TestCam.GetProjMatrix()->m;
			glLoadMatrixf(matrix);

			glMatrixMode( GL_MODELVIEW ); 
			width = w;
			height = h;
		}

		void Render()
		{
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST); 
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
			glClearColor(0,0,0,0);

			glLineWidth(1);

			// Camera here
			TestCam.FrameMove( static_cast<float>(Timer::GlobalTimer().FrameDelta()) );

			const float *matrix = (float*)TestCam.GetViewMatrix()->m; 
			glLoadMatrixf(matrix);
			glColor4f(1,0,0,1);
			glPushMatrix();
			glTranslatef(0,0,-20);
			DrawLineCube(10);
			glTranslatef(0,0,-20);
			DrawLineCube(10);
			glTranslatef(0,0,-20);
			DrawLineCube(10);
			glPopMatrix();

			glColor4f(1,1,0,1);
			glPushMatrix();
			glTranslatef(0,0,20);
			DrawLineCube(10);
			glTranslatef(0,0,20);
			DrawLineCube(10);
			glTranslatef(0,0,20);
			DrawLineCube(10);
			glPopMatrix();

			glColor4f(1,1,1,1);
			glPushMatrix();
			glTranslatef(-20,0,0);
			DrawLineCube(10);
			glTranslatef(-20,0,0);
			DrawLineCube(10);
			glTranslatef(-20,0,0);
			DrawLineCube(10);
			glPopMatrix();

			glColor4f(0,1,1,1);
			glPushMatrix();
			glTranslatef(20,0,0);
			DrawLineCube(10);
			glTranslatef(20,0,0);
			DrawLineCube(10);
			glTranslatef(20,0,0);
			DrawLineCube(10);
			glPopMatrix();
		}

	private:
		void DrawLineCube(float size)
		{
			float l = size/2;
			glBegin(GL_LINE_STRIP);
			// Front
			glVertex3f( l,  l, l);
			glVertex3f( l, -l, l);
			glVertex3f(-l, -l, l);
			glVertex3f(-l,  l, l);
			glVertex3f( l,  l, l);
			// back
			glVertex3f( l,  l, -l);
			glVertex3f( l, -l, -l);
			glVertex3f(-l, -l, -l);
			glVertex3f(-l,  l, -l);
			glVertex3f( l,  l, -l);
			glEnd();

			// missing edges
			glBegin(GL_LINES);
			glVertex3f( l, -l, -l);
			glVertex3f( l, -l,  l);
			glVertex3f(-l, -l, -l);
			glVertex3f(-l, -l,  l);
			glVertex3f(-l,  l, -l);
			glVertex3f(-l,  l,  l);
			glEnd();

		}

		bool SetupPixelFormat(HDC hDC)
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
				MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);// TODO ASSERT replace
				return false;
			}

			if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
			{
				MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);// TODO ASSERT replace
				return false;
			}

			return true;
		}

		HGLRC			renderContext;
		uint width;
		uint height;
	};
}


namespace Base {

	// TODO Move to seperate file
	// TODO Remove win32 specifics from the interface
	// TODO Remove gl context specifics from interface
	class MainGLApp 
	{
	public:
		MainGLApp(const char* name, HINSTANCE hInstance, HINSTANCE hPrevInstance, 
			LPSTR lpCmdLine, int nCmdShow, bool fullscreenMode, 
			uint widthOfScreen, uint heightOfScreen) : 
		width(widthOfScreen), height(heightOfScreen), fullscreen(fullscreenMode),
			appName(name)
		{
			assert(NULL == self);
			self = this;
			context = new GLContext;
			bool res = CreateAppWindow( hInstance, nCmdShow );
			assert(res);
		}

		~MainGLApp()
		{
			assert(NULL != self);
			self = NULL;
			delete context;

		}

		static MainGLApp &Instance()
		{
			assert(self != NULL);
			return *self;
		}

		GLContext &Context() 
		{
			assert(context);
			return *context;
		}

		void MainLoop() 
		{
			Timer::GlobalTimer().Start();
			double lastTime = 0;
			bool quit = false;
			while (!quit) { // TODO !quit

				MSG			msg;

				while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
				{ 
					if (GetMessage(&msg, NULL, 0, 0) ) 
					{ 
						TranslateMessage(&msg); 
						DispatchMessage(&msg); 
					} else { 
						quit = true;
					} 
				} 

				Timer::GlobalTimer().Update();
				Context().Render();
				SwapBuffers(deviceContext);
			}
		}

		static long FAR PASCAL WndProc(HWND hWnd, UINT message, 
			WPARAM wParam, LPARAM lParam)
		{
			return Instance().WindowProc(hWnd, message, wParam, lParam);
		}

		long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
			WPARAM wParam, LPARAM lParam)
		{
			PAINTSTRUCT ps;
			RECT rect; 

			if ( TestCam.HandleMessages(hWnd, message, wParam, lParam) )
			{
				return (long FAR)DefWindowProc(hWnd, message, wParam, lParam);
			}

			switch(message)
			{
			case WM_CREATE: 
				deviceContext = GetDC(hWnd); 
				Context().Initialize(deviceContext, hWnd);

				break; 
			case WM_PAINT:
				deviceContext = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				return TRUE;

			case WM_SIZE: 
				GetClientRect(hWnd, &rect); 
				Context().Resize(rect.right, rect.bottom); 
				width = rect.right;
				height = rect.bottom;
				break; 

			case WM_CLOSE: 
				Context().Destroy();
				if (deviceContext) 
					ReleaseDC(hWnd, deviceContext); 
				deviceContext = NULL; 
				DestroyWindow (hWnd); 
				break; 

			case WM_DESTROY: 
				Context().Destroy();
				if (deviceContext) 
					ReleaseDC(hWnd, deviceContext); 
				PostQuitMessage (0); 
				break; 

			case WM_KEYDOWN: 
				//KeyDown(wParam);
				break;
			}
			return (long FAR)DefWindowProc(hWnd, message, wParam, lParam);

		}

		uint Width() const { return width; }
		uint Height() const { return height; }

	private:

		// Private methods
		bool CreateAppWindow( HINSTANCE hInstance, int nCmdShow ) 
		{
			WNDCLASS	wc;

			wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
			wc.lpfnWndProc		= (WNDPROC)WndProc; 
			wc.cbClsExtra		= 0; 
			wc.cbWndExtra		= 0; 
			wc.hInstance		= hInstance; 
			wc.hIcon			= LoadIcon (hInstance, appName); 
			wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground	= NULL;
			wc.lpszMenuName		= NULL; 
			wc.lpszClassName	= appName; 


			if (!RegisterClass (&wc) ) 
				return false; 

			ghWnd = CreateWindowEx ( 
				fullscreen ? WS_EX_TOPMOST : 0,
				appName, 
				appName,
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
				return false; 

			/* show and update main window */ 
			ShowWindow (ghWnd, nCmdShow); 
			UpdateWindow (ghWnd);

			// TODO Change to fullscreen proper

			return true;
		}

		// Member variables

		HDC				deviceContext;
		HWND			ghWnd;
		HINSTANCE		ghInstance;

		static MainGLApp *self;
		GLContext *context;

		uint width;
		uint height;
		bool fullscreen;
		const char *appName;
	};
	MainGLApp *MainGLApp::self = NULL;
}


void Test(); 
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, int nCmdShow)
{
	bool fullscreen = false;

	// TODO Move to setting
	//if(MessageBox(HWND_DESKTOP, "Want to use fullscreen mode?",	
	//	"Fullscreen?", MB_YESNO|MB_ICONQUESTION) == IDYES)
	//{
	//	fullscreen = true;
	//}
	Base::debugLog << "String " << (int)10190101 << " " << (float)3.1400000000000009f << std::endl;

	Base::MainGLApp app(
		"Dave's App",
		hInstance, 
		hPrevInstance, 
		lpCmdLine, 
		nCmdShow,
		fullscreen, 640, 480);

	app.MainLoop();

	
}