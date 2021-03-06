/*
Project Omega 29/12/00

omega_main.c

  Whats going in this file???
  WinMain
  WinProc
  RunFrame
  Any config settings/command line

*/

#include "omega_common.h"
#include "omega_gl.h"
#include "resource.h"

int mouseX, mouseY;
void ModelInit();
#define WM_MOUSEWHEEL 52431

/*
Globals
*/
HDC				ghdc;
HGLRC			ghrc;
HWND			ghWnd;
HINSTANCE		ghInstance;
vidmode			current_vidmode;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS	wc;
    MSG			msg;
	HWND		hwnd_dialog;
	
	ghInstance = hInstance;
	// Create splash
	hwnd_dialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SS), NULL, NULL);

	if (hwnd_dialog)
	{
		ShowWindow (hwnd_dialog, SW_SHOWDEFAULT);
		UpdateWindow (hwnd_dialog);
		SetForegroundWindow (hwnd_dialog);
	}

	ConInit();
	//CommandLine ();
	LoadConfig ();

    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    wc.lpfnWndProc   = (WNDPROC)WindowProc; 
    wc.cbClsExtra    = 0; 
    wc.cbWndExtra    = 0; 
    wc.hInstance     = hInstance; 
    wc.hIcon         = LoadIcon (hInstance, APPNAME); 
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL; 
    wc.lpszClassName = APPNAME; 
	
	
    if (!RegisterClass (&wc) ) 
        return FALSE; 

	TimeInit();
	TimeUpdate();
	

    ghWnd = CreateWindowEx ( 
		current_vidmode.fullscreen ? WS_EX_TOPMOST : 0,
		APPNAME, 
		APPNAME,
		current_vidmode.fullscreen ? WS_POPUP | WS_VISIBLE : WS_CAPTION,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		current_vidmode.width,
		current_vidmode.height,
		NULL,
		NULL,
		(HINSTANCE)hInstance,
		NULL);

    if (!ghWnd) 
        return FALSE; 
	
    /* show and update main window */ 
    ShowWindow (ghWnd, nCmdShow); 
    UpdateWindow (ghWnd);

	VidInit();
	glOmegaInit();	
	TextureInit ();
	pInitManager();
	TextInit();
	RoamInit("maps/map2.raw");
	ModelInit();
	SkyInit();
	InitEntities();
	DestroyWindow (hwnd_dialog);

	// remove splash do before fullscreen


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
	vec3_t f;
	float m[16];
	
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	VectorSet(f, -m[2], -m[6], -m[10]);
	VectorScale(f, 10, f);

	switch(message)
    {
	case WM_MOUSEWHEEL:
		{
			
			if ((int)wParam > 0) {
				ConPrintf("HellO!");
				}
			else
				
			break;
		}
			//	wParam up or down 
			//	mouseX = LOWORD(lParam);
			//	mouseY = HIWORD(lParam);
		break;
	case WM_MOUSEMOVE: 
		mouseX = -LOWORD(lParam);
		mouseY = HIWORD(lParam);
		break;

	case WM_LBUTTONDOWN:
		VectorAdd(viewer.pos, f, viewer.pos);
		break;
	case WM_RBUTTONDOWN:
		VectorSubtract(viewer.pos, f, viewer.pos);
	case WM_MBUTTONDOWN:
		break;
		
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		break;

    case WM_SYSKEYDOWN:
		break;
	case WM_KEYDOWN: 
		KeyDown(wParam);
		break;

	case WM_PAINT:
        ghdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return TRUE;

	case WM_DESTROY:
		VidShutdown();
		SaveConfig();
		PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void RunFrame (void)
{
	// Renders all the objects
	glOmegaRenderFrame();
	// 
	// Moves particle systems around
	pRunFrame ();

	CountFPS();
}

void LoadConfig (void)
{
	FILE	*settings = fopen ("settings.cfg", "r");
	bool	fullscreen = 0;
	int		height = 480, width = 640, bpp = 16;// defaults?


	if (settings)
	{
		while (!feof(settings))
		{
			char word[100];

			fscanf(settings,"%s", word);

			if (stricmp(word, "width") == 0)
				fscanf(settings,"%d", &width);
			else if (stricmp(word, "height") == 0)
				fscanf(settings,"%d", &height);
			else if (stricmp(word, "fullscreen") == 0)
				fscanf(settings,"%d", &fullscreen);
			else if (stricmp(word, "bpp") == 0)
				fscanf(settings,"%d", &bpp);
		}
		fclose (settings);
	}

	current_vidmode.height = height;
	current_vidmode.width = width;
	current_vidmode.bpp = bpp;
	current_vidmode.fullscreen = fullscreen;

	sprintf(current_vidmode.desc, "%dx%dx%d", current_vidmode.width,
											current_vidmode.height,
											current_vidmode.bpp);
}

void SaveConfig (void)
{
	FILE	*settings = fopen ("settings.cfg", "w");

	if (settings)
	{
		fprintf(settings,	"width\t%d\nheight\t%d\n"
							"bpp\t%d\nfullscreen\t%d\n",
							current_vidmode.width,
							current_vidmode.height,
						 	current_vidmode.bpp,
							current_vidmode.fullscreen);
		fclose (settings);
	}
}

