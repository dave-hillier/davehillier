#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "Timer.h"
#include "font.h"
#include "glapp.h"
#include "texturemanager.h"
#include "ParticleManager.h"
#include "camera.h"

#include "ms3d_model.h"
#include "md2_model.h"
#include "myship.h"

#include "unit.h"

myShip *ship;
GLApp glapp;
md2_model tester;

void GLApp::initgl ()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	arial = new Font ("Arial Black");

	texture.LoadTexture("test.tga");
	texture.LoadTexture("drfreak.tga");
	texture.LoadTexture("envtest1.tga");
	texture.LoadTexture("envtest2.tga");

	tester.Load("tris.md2");

	ship = new myShip();
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}


// display
static float a = 0;
void GLApp::display ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLoadMatrixf(camera.GetMatrix());

	glPushMatrix();
	glScalef(5, 5, 5);
	arial->printf("David Hillier");
	glPopMatrix();


	// should switch to overlay mode
	glColor3f(1,1,1);
	glPushMatrix();
	a += timer.Delta() * 360.0f; // one rotation per second?
	if (a > 360.0)
		a = 0.0f;
	glTranslatef(0,0,-50);
	glRotatef(a, 0, 1, 0);
//	glTranslatef(-10,0,0);
	glScalef(5, 5, 5);
	arial->printf("David Hillier");

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);

	glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(50,0,-250);
	texture.Bind("drfreak.tga");
	tester.DrawFrameInt(3,0,0);
	glPopMatrix();

	ship->Render();
	ship->Update();

	uman.Render();
	uman.Update();

	glColor3f(0,1,0);
	for (int i=-300000;i < 300000; i += 4000)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 0, -300000);
		glVertex3f(i, 0, 300000);

		glVertex3f(-300000, 0, i);
		glVertex3f(300000, 0, i);
		glEnd();
	}


//	pman.TestSys(Vector3(0,0,-50));
	pman.Render();
	pman.Update();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	glOrtho(0, width, 0, height, -10000, 10000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// overlay in here
	glColor3f(1,1,1);

	glPushMatrix();
	glTranslatef(10,10,0);
	glScalef(20, 20, 20);

	arial->printf("ship (%.0f %.0f %.0f) pitch %3.0f power %.0f", 
		ship->org[0], ship->org[1], ship->org[2], 
		ship->engine_pitch, ship->engine_power);
	glPopMatrix();   

	glPushMatrix();
	glTranslatef(10,40,0);
	glScalef(20, 20, 20);

	arial->printf("vel %.0f roll %.2f", 
		ship->vel.Length(), ship->roll);

	glPopMatrix();   
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(10,70,0);
	glScalef(20, 20, 20);

	arial->printf("acc %.0f", 
		ship->acc.Length());

	glPopMatrix();   

	glDisable(GL_TEXTURE_2D);
	
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

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,40.f,300000.0f);

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
