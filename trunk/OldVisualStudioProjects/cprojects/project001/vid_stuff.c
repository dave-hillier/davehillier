#include "project001.h"


HDC		maindc;
HGLRC	baseRC;
HWND	mainwindow;

double	rotation;
void GL_Init (void);
particle *AddParticle (particle	*prev, 
					   vector3f pos, 
					   vector3f vel, 
					   vector4f	colour,
					   double timestamp,
					   int	flags, 
					   float size);

extern particle *particles;

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

void vid_start (void)
{ 
//	HDC		hdc;
 
	maindc = GetDC(mainwindow);
	bSetupPixelFormat(maindc);

	baseRC = wglCreateContext( maindc );

	if (!baseRC)
		MessageBox(NULL, "Could not initialize GL", "shit", MB_OK);
//	Sys_Error ("Could not initialize GL (wglCreateContext failed).\n\nMake sure you in are 65535 color mode, and try running -window.");
	if (!wglMakeCurrent( maindc, baseRC ))
		MessageBox(NULL, "wglMakeCurrent failed", "shit", MB_OK);
//	Sys_Error ("wglMakeCurrent failed");
	GL_Init();

} 

void vid_end (void)
{
   	HGLRC hRC;
   	HDC	  hDC;

//	if (vid_initialized)
//	{
//		vid_canalttab = false;
		hRC = wglGetCurrentContext();
    	hDC = wglGetCurrentDC();

    	wglMakeCurrent(NULL, NULL);

    	if (hRC)
    	    wglDeleteContext(hRC);

		if (hDC && mainwindow)
			ReleaseDC(mainwindow, hDC);

//		if (modestate == MS_FULLDIB)
//			ChangeDisplaySettings (NULL, 0);

		if (maindc && mainwindow)
			ReleaseDC (mainwindow, maindc);

//		AppActivate(false, false);
//	}
}
void polarView(GLdouble radius, GLdouble twist, GLdouble latitude, 
           GLdouble longitude) 
{ 
    glTranslated(0.0, 0.0, -radius); 
    glRotated(-twist, 0.0, 0.0, 1.0); 
    glRotated(-latitude, 1.0, 0.0, 0.0); 
    glRotated(longitude, 0.0, 0.0, 1.0);      
} 

GLvoid resize( GLsizei width, GLsizei height ) 
{ 
    GLfloat aspect; 
 
    glViewport( 0, 0, width, height ); 
 
    aspect = (GLfloat) width / height; 
 
    glMatrixMode( GL_PROJECTION ); 
    glLoadIdentity(); 
    gluPerspective( 45.0, aspect, 3.0, 7.0 ); 
    glMatrixMode( GL_MODELVIEW ); 
}     

void GL_Init (void)
{
	glClearColor (0.0, 0.0, 0,0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST); 

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    
	glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_FLAT);

	glColor3f(1.0, 1.0, 1.0);
	glOrtho(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0); 
  
	glEnable (GL_ALPHA_TEST);

  
  // Fog test
 /* glEnable(GL_FOG);
    {
        GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};

        glFogi (GL_FOG_MODE, GL_LINEAR);
        glFogfv (GL_FOG_COLOR, fogColor);
        glFogf (GL_FOG_DENSITY, 0.35);
        glHint (GL_FOG_HINT, GL_DONT_CARE);
        glClearColor(0.75, 0.4, 0.1, 1.0);
    }
*/
	rotation = 0;
	particles = NULL;
}
#define NUM_PARTICLES 50

void draw (void)
{
	

	rotation += 0.001;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glPointSize(10);
	glBegin(GL_POINTS); 
	{

		int i;
		GLfloat x,y, r=1, g=0, b=0, inc=0;
		inc = 6.0/NUM_PARTICLES;

		for (i=0; i < NUM_PARTICLES; i++)
		{
			x = cos(rotation + ((float)i/NUM_PARTICLES+rotation) * (M_PI*2))*75;
			y = sin(rotation + ((float)i/NUM_PARTICLES+rotation) * (M_PI*2))*75;

			glColor3f(r, g, b);
			if (r >= 1 && g < 1 && b <= 0)
			{
				r = 1;
				g += inc;// need to figure out
			}
			if (g >= 1 && r > 0 && b < 1)
			{
				r -= inc;
				g = 1;
			}
			if (g >= 1 && r <= 0 && b < 1)
			{
				r = 0;
				b += inc;
				g = 1;
			}
			if (b >= 1 && g > 0 && r <= 0)
			{
				r = 0;
				g -= inc;
				b = 1;
			}
			if (b >= 1 && r < 1 && g <= 0)
			{
				r += inc;
				g = 0;
				b = 1;
			}
			if (b > 0 && r >= 1 && g <= 0)
			{
				r = 1;
				g = 0;
				b -= inc;
			}
			glVertex3f(x, cos(i + rotation*20)*10, y);
		}
	}

	glEnd();
	glFlush();

	SwapBuffers(maindc);
}