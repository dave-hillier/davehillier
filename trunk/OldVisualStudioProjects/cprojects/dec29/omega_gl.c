#include "omega_common.h"
#include "omega_gl.h"


#include <math.h>
extern float	fps;
extern vidmode	current_vidmode;
extern HDC		ghdc;
view			viewer;

void glOmegaPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void glOmegaInit()
{
	float aspect, fov = 90.0;

	glClearColor (0.0, 0.0, 0,0);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION ); 
    aspect = (float) current_vidmode.width / current_vidmode.height;
    glOmegaPerspective( fov/2, aspect, 1,  4096 );
    glMatrixMode( GL_MODELVIEW ); 

	viewer.pos[0] = 0;
	viewer.pos[1] = 60;
	viewer.pos[2] = 500;
	viewer.angles[0] = 0;
	viewer.angles[1] = 0;
	viewer.angles[2] = -20;

	TextureInit ();
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);	
}

void glOmegaTestObject (float origin[3], float scale[3], float angles[3])
{
	glPushMatrix();

	glScalef (scale[0], scale[1], scale[2]);
	glTranslatef (origin[0], origin[1], origin[2]);

	glRotatef (angles[1],	0, 0, 1);
	glRotatef (-angles[0],	0, 1, 0);
	glRotatef (angles[2],	1, 0, 0);

	// Need to do more matricies to set these to proper origins
	glBegin(GL_LINES);
	{
		// This could be the default object? maybe
		glColor3f  (1.0, 0.0, 0.0);
		glVertex3f (0.0, 0.0, 0.0);
		glVertex3f (10.0, 0.0, 0.0);
		glColor3f  (0.0, 1.0, 0.0);
		glVertex3f (0.0, 10.0, 0.0);
		glVertex3f (0.0, 0.0, 0.0);
		glColor3f  (0.0, 0.0, 1.0);
		glVertex3f (0.0, 0.0, 0.0);
		glVertex3f (0.0, 0.0, 10.0);
	}
	glEnd();
	glPopMatrix();
}
void glOmegaSet2d (void)
{

}

void glOmegaRenderFrame ()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glRotatef (-viewer.angles[2],  1, 0, 0);// up/down
	glRotatef (-viewer.angles[0],  0, 1, 0);// roll
	glRotatef (-viewer.angles[1],  0, 0, 1);// left/right
	glTranslatef (-viewer.pos[0],  -viewer.pos[1],  -viewer.pos[2]);

	pRenderSystems ();
	
	TextPrint(20, 460, "\\c(64 128 128)FPS:\\c(255 10 10) %f\n\\c(64 128 128)"
				"Press 1 for left particle system\n"
				"Press 2 for right particle system\n", fps);

	SwapBuffers(ghdc);
}