#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>

#include "common.h"

void Perspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void GLInit ()
{
	float aspect;
	glClearColor (0.0, 0.0, 0,0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST); 

//	glEnable(GL_LIGHT0);//Ive not used this before may need deleting
//	glLightfv(GL_LIGHT0, GL_POSITION, lightPos); 

//	glEnable(GL_TEXTURE_2D); // always enabled
//	glEnable(GL_CULL_FACE); // always enabled

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION ); 
    aspect = (float) width / height;
    Perspective( 45.0, aspect, 1,  4096 ); // 45 = fov/2
    glMatrixMode( GL_MODELVIEW ); 

	getmdhptr("knot.mdh");
	
	inittext();
}
void drawsticks ()
{
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
} 
void Render (void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	// Rotate to view, but what type???
// pilot view
	
	glRotatef (-view_angles[2],  1, 0, 0);
	glRotatef (-view_angles[0],  0, 1, 0);
	glRotatef (-view_angles[1],  0, 0, 1);
	glTranslatef (-view_pos[0],  -view_pos[1],  -view_pos[2]);
	// Draw objects

	drawsticks();
	TestText();
	renderModels();
/*
// polar view
	glTranslated(0.0, 0.0, -distance);
	glRotated(-twist, 0.0, 0.0, 1.0);
	glRotated(-elevation, 1.0, 0.0, 0.0);
	glRotated(azimuth, 0.0, 0.0, 1.0);
*/
	
	// Render stuff
	// do a vertex array later
	// Set view to ortho

	SwapBuffers(ghDC);
}