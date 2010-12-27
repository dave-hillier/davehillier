#include <windows.h>
#include "common.h"

#define NUM_PARTICLES 50
int rotation = 0;
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
void glMyInit ()
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
}

void RenderFrame ()
{

	rotation += 0.001;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glPointSize(10);
	glBegin(GL_LINES); 
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
	SwapBuffers(ghdc);
}