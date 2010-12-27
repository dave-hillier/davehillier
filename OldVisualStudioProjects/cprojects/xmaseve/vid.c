#include <windows.h>
#include "common.h"



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

	if (hDC && mainwindow)
		ReleaseDC(mainwindow, hDC);

	if (modelist[currentmode].fullscreen)
		ChangeDisplaySettings (NULL, 0);

	if (ghdc && mainwindow)
		ReleaseDC (mainwindow, ghdc);


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
			(devmode.dmPelsWidth <= 2000) &&
			(devmode.dmPelsHeight <= 2000) &&
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

				//	MessageBox(NULL, modelist[nummodes].desc, "Adding mode", MB_OK);
				nummodes++;
			}
		}
		modenum++;
	}
}

bool VidGoFullScreen (int modenum)
{
	DEVMODE	devmode;
	devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	devmode.dmBitsPerPel = modelist[modenum].bpp;
	devmode.dmPelsWidth = modelist[modenum].width;

	devmode.dmPelsHeight = modelist[modenum].height;
	devmode.dmSize = sizeof (devmode);

	ChangeDisplaySettings (&devmode, CDS_FULLSCREEN);
	return true;
}

void VidInit ()
{
	ghdc = GetDC(mainwindow);
	bSetupPixelFormat(ghdc);

	baseRC = wglCreateContext( ghdc );

	if (!baseRC)
		MessageBox(NULL, "Could not initialize GL", "ERROR", MB_OK);
	if (!wglMakeCurrent( ghdc, baseRC ))
		MessageBox(NULL, "wglMakeCurrent failed", "ERROR", MB_OK);

	VidInitFullScreen(ghInstance);
//	VidGoFullScreen(0);
	glMyInit();

}