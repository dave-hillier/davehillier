#include "omega_common.h"
#include "omega_gl.h"


#include <math.h>
extern float	fps;
extern vidmode	current_vidmode;
extern HDC		ghdc;
view			viewer;

bool showfps;

void glOmegaPerspective ( float fovy, float aspect,
		     float zNear, float zFar )
{
	float xmin, xmax, ymin, ymax;
	// my modified one which sets my view struct up aswell as the frustum

	ymax = zNear * tan( fovy * PI / 360.0 );
	ymin = - ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void glOmegaInit (void)
{
	float	aspect, fov = 90;
	viewer.fov = fov;

	glMatrixMode( GL_PROJECTION ); 
    aspect = (float)current_vidmode.width / (float)current_vidmode.height;
    glOmegaPerspective( fov/2, aspect, 1,  4096 );

	calcfrustumnormals();

	glClearColor (0.0, 0.0, 0.0, 0);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST); 
	glEnable(GL_LIGHT0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	viewer.pos[0] = 512;
	viewer.pos[1] = 200;
	viewer.pos[2] = 512;
	viewer.angles[0] = 0;
	viewer.angles[1] = 0;
	viewer.angles[2] = 0;
	ConPrintf("GlInit\n");

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
		glVertex3f (1.0, 0.0, 0.0);
		glColor3f  (0.0, 1.0, 0.0);
		glVertex3f (0.0, 1.0, 0.0);
		glVertex3f (0.0, 0.0, 0.0);
		glColor3f  (0.0, 0.0, 1.0);
		glVertex3f (0.0, 0.0, 0.0);
		glVertex3f (0.0, 0.0, 1.0);
	}
	glEnd();
	glPopMatrix();
}


void glOmegaRenderFrame (void)
{
	extern int textsize;
	vec3_t v = { 1, 1, 1 };

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glRotatef (-viewer.angles[2],  1, 0, 0);// up/down
	glRotatef (-viewer.angles[0],  0, 1, 0);// roll
	glRotatef (-viewer.angles[1],  0, 0, 1);// left/right
	
	glTranslatef (-viewer.pos[0],  -viewer.pos[1],  -viewer.pos[2]);
	// since these have changed
	calcfrustumnormals();

	RoamFrame();

	viewer.angles[2] = (float)mouseY*2;
	viewer.angles[0] = (float)mouseX*2;
	ModelFrame();
	RunEntities();
	SkyRender();
	pRenderSystems ();
	ConRender();
	drawfog();

	textsize = 12;
//	TextPrint(10, 450, "\\G(0 30 30)\\g+\\c(64 255 255)FPS:\\g-\\G(120 30 30)\\c(255 255 255) %f\n\\g+\\d+Nice text\\g-\\d-\n", fps);
	if (showfps)
		TextPrint(current_vidmode.width - 100, current_vidmode.height - 20, "FPS: %.0f\n", fps);

	SwapBuffers(ghdc);
}