#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"

#include "Timer.h"
#include "font.h"
#include "glapp.h"
#include "texturemanager.h"
#include "ParticleManager.h"
#include "camera.h"
#include "frustum.h"

#include "ms3d_model.h"
#include "md2_model.h"
#include "jet.h"
#include "terrain.h"
#include "unit.h"
#include "Crosshair.h"
#include "Overlay.h"
#include "ase_model.h"
#include "Console.h"
void LoadStaticMedia();

extern float unit_scale;


GLApp glapp;

PFNGLMULTITEXCOORD1FARBPROC	glMultiTexCoord1fARB	= NULL;
PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB	= NULL;
PFNGLMULTITEXCOORD3FARBPROC	glMultiTexCoord3fARB	= NULL;
PFNGLMULTITEXCOORD4FARBPROC	glMultiTexCoord4fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB	= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB= NULL;

void GLApp::initgl ()
{
	// maybe splash here?
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
	glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
	glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");


	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.3f, 0.4f, 0.9f, 0.0f);

	arial = new Font ("Arial Black");

	texture.LoadTexture("./pics/envtest1.jpg");
	texture.LoadTexture("./pics/sky.jpg");
	texture.LoadTexture("./pics/crosshair.tga");
	texture.LoadTexture("./pics/spectrum.jpg");
	LoadStaticMedia();

	terrain.Init("map1");

}


// display
static float a = 0;
void DrawSky();
void DrawSea();
void GLApp::display ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLoadMatrixf(camera.GetMatrix());
	gen_clipmat();
	xhair.Update();

	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glEnable(GL_TEXTURE_2D);

	DrawSky();
	DrawSea();

	terrain.Render(camera.position);//();

	uman.Render();
	uman.Update();


	pman.Render();
	pman.Update();

	// switch to overlay
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	glOrtho(0, width, 0, height, -10000, 10000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// overlay in here
	// use a class
	overlay.Display();
	overlay.Update();
	xhair.Display();
	con->Render();


	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(10,10,0);
	glScalef(20, 20, 20);

	float fps = 1.0f/timer.Delta();
	arial->printf("FPS: %.2f", fps);
	glPopMatrix();   
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
void GLApp::resize(int w, int h)
{
	width = w;
	height = h;
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,unit_scale*50*0.1,768.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
void R_RotateForEntity (entity_t *e)
{
    glTranslatef (e->origin[0],  e->origin[1],  e->origin[2]);

    glRotatef (e->angles[1],  0, 0, 1);
    glRotatef (-e->angles[0],  0, 1, 0);
    glRotatef (e->angles[2],  1, 0, 0);
}*/
void DrawSky ()
{
	float i, j, d;


//	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(camera.position.x, camera.position.y, camera.position.z);
	glColor3f(1,1,1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	texture.Bind("./pics/sky.jpg");

	for (j=-5; j<=5; j+=1.0f) {
		glBegin(GL_TRIANGLE_STRIP);
		for (i=-5; i<=5; i+=1.0f) {
			d = (float )sqrt(i*i+j*j);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, j/2);
			glVertex3f(i, 1-d/3, j);
			d = j + 1.0f;
			d = (float )sqrt(i*i+d*d);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, (j+1.0f)/2);
			glVertex3f(i, 1-d/3, (j+1.0f));
		}
		glEnd();
	}
	glPushMatrix();
	glScalef(1,-1,1);
	for (j=-5; j<=5; j+=1.0f) {
		glBegin(GL_TRIANGLE_STRIP);
		for (i=-5; i<=5; i+=1.0f) {
			d = (float )sqrt(i*i+j*j);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, j/2);
			glVertex3f(i, 1-d/3, j);
			d = j + 1.0f;
			d = (float )sqrt(i*i+d*d);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, (j+1.0f)/2);
			glVertex3f(i, 1-d/3, (j+1.0f));
		}
		glEnd();
	}
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
}


void DrawSea () {
/*	static float cosa=0, sea_move;
	cosa += 0.01;

	sea_move += 0.001*cos(cosa);

	glDisable(GL_CULL_FACE);
//	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glBindTexture(GL_TEXTURE_2D, terrain_texture.sea1);
	texture.Bind("sea1.tga");

	glActiveTextureARB(GL_TEXTURE1_ARB);
//	glBindTexture(GL_TEXTURE_2D, terrain_texture.sea2);
	texture.Bind("sea2.tga");

	glColor4f(0.6, 0.7, 1.0, 0.3);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0+sea_move, 200+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0+sea_move, 200-sea_move);
	glVertex3f(-1024.0f, 0.0f,  1024.0f);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 200+sea_move, 200+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 200+sea_move, 200-sea_move);
	glVertex3f( 1024.0f, 0.0f,  1024.0f);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 200+sea_move, 0+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 200+sea_move, 0-sea_move);
	glVertex3f( 1024.0f, 0.0f, -1024.0f);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0+sea_move, 0+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0+sea_move, 0-sea_move);
	glVertex3f(-1024.0f, 0.0f, -1024.0f);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0, 1.0, 1.0, 1.0);

//	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);


	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
*/
	// draw sun?
}

