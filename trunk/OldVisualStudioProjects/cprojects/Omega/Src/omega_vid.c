#include "omega_common.h"
#include "omega_gl.h"


vidmode			modelist[MAX_VIDMODES];


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


void VidShutdown (void)
{
   	HGLRC hRC;
   	HDC	  hDC;

	hRC = wglGetCurrentContext();
    hDC = wglGetCurrentDC();

    wglMakeCurrent(NULL, NULL);

    if (hRC)
		wglDeleteContext(hRC);

	if (hDC && ghWnd)
		ReleaseDC(ghWnd, hDC);

	if (current_vidmode.fullscreen)
		ChangeDisplaySettings (NULL, 0);

	if (ghdc && ghWnd)
		ReleaseDC (ghWnd, ghdc);


}
void VidInitFullScreen (HINSTANCE hInstance)
{
	DEVMODE	devmode;
	bool more_modes = true;
	int modenum = 0, nummodes = 0;

	while (more_modes)
	{
		more_modes = EnumDisplaySettings (NULL, modenum, &devmode);

		if ((devmode.dmBitsPerPel >= 15) &&
			(devmode.dmPelsWidth <= MAX_WIDTH) &&
			(devmode.dmPelsHeight <= MAX_HEIGHT) &&
			(nummodes < MAX_VIDMODES))
		{
			devmode.dmFields = DM_BITSPERPEL |
				DM_PELSWIDTH |
				DM_PELSHEIGHT;

			if (ChangeDisplaySettings (&devmode, CDS_TEST | CDS_FULLSCREEN) ==
				DISP_CHANGE_SUCCESSFUL)
			{
				modelist[nummodes].bpp = devmode.dmBitsPerPel;
				modelist[nummodes].width = devmode.dmPelsWidth;
				modelist[nummodes].height = devmode.dmPelsHeight;
				modelist[nummodes].fullscreen = true;

				sprintf(modelist[nummodes].desc, "%dx%dx%d", 
					modelist[nummodes].width,
					modelist[nummodes].height,
					modelist[nummodes].bpp);

				nummodes++;
			}
		}
		modenum++;
	}
}

bool VidGoFullScreen (void)
{
	DEVMODE	devmode;

	devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	devmode.dmBitsPerPel = current_vidmode.bpp;
	devmode.dmPelsWidth = current_vidmode.width;

	devmode.dmPelsHeight = current_vidmode.height;
	devmode.dmSize = sizeof (devmode);

	if (ChangeDisplaySettings (&devmode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
		return true;
	else
		return false;
}

bool VidGoWindowed (void)
{
	if (current_vidmode.fullscreen)
	{
		if (ChangeDisplaySettings (NULL, 0) == DISP_CHANGE_SUCCESSFUL)
		{
			return true;
		}
	}
	return false;
}


void VidInit (void)
{
	ConPrintf("VidInit\n");
	ghdc = GetDC(ghWnd);
	bSetupPixelFormat(ghdc);

	ghrc = wglCreateContext( ghdc );

	if (!ghrc)
	{
		MessageBox(NULL, "Could not initialize GL", "ERROR", MB_OK);
		// quit
	}
	if (!wglMakeCurrent( ghdc, ghrc ))
	{
		MessageBox(NULL, "wglMakeCurrent failed", "ERROR", MB_OK);
		// quit
	}

	VidInitFullScreen(ghInstance);
	if (current_vidmode.fullscreen)
		VidGoFullScreen();
	
}

