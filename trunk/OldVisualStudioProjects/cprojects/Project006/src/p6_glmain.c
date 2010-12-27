// p6_glmain.c
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "p6_common.h"
vec3_t	viewpos;
vec3_t  viewangles;
void p6Perspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
   // set clipping box
}

GLvoid resize( GLsizei w, GLsizei h ) 
{ 
	GLfloat aspect; 

	glViewport( 0, 0, w, h ); 

	aspect = (GLfloat) w / h; 

	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity(); 
	p6Perspective( 45.0, aspect, 1.0, 4096.0 ); 

	glMatrixMode( GL_MODELVIEW ); 
	// set the globals
	height = h;
	width = w;
}

GLvoid initializeGL(GLsizei w, GLsizei h)
{
	GLfloat aspect; 

	glViewport( 0, 0, w, h ); 

	aspect = (GLfloat) w / h; 

	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity(); 
	p6Perspective( 45.0, aspect, 1.0, 4096.0 ); 
	glMatrixMode( GL_MODELVIEW ); 

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

}
GLvoid setView(GLvoid)
{
	glRotatef (-viewangles[2],  1, 0, 0);// up/down x
	glRotatef (-viewangles[0],  0, 1, 0);// roll y 
	glRotatef (-viewangles[1],  0, 0, 1);// left/right z
	
	glTranslatef (-viewpos[0],  -viewpos[1],  -viewpos[2]);	
}
GLvoid display(GLvoid)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
	// set the view
	setView();
	TerrainFrame();
	// Draw world/objects

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// do overlay bit here
	{
		char str[60];
		sprintf(str, "Frame Rate %.0f", fps);
		drawString(8 , height - 30, str);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
