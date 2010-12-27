// Crosshair.cpp: implementation of the Crosshair class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "texturemanager.h"
#include "timer.h"
#include "input.h"
#include "glapp.h"
#include "ms3d_model.h"
#include "unit.h"
#include "font.h"
#include "terrain.h"

#include "Crosshair.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Crosshair xhair;
Crosshair::Crosshair()
{
	x = glapp.width/2.f;
	y = glapp.height/2.f;
	colour[0] = 1;
	colour[1] = 1;
	colour[2] = 1;
	colour[3] = 1;
	size = 20;
//	range = 100;
}

Crosshair::~Crosshair()
{

}

void Crosshair::Update()
{
	float old_x, old_y;

	old_x = x;
	old_y = y;

	x += input.mouse_x;
	y += input.mouse_y;

	if (x < glapp.width/10.f)
		x = glapp.width/10.f;
	else if (x > glapp.width-glapp.width/10.f)
		x = glapp.width-glapp.width/10.f;
	
	if (y < glapp.height/10.f)
		y = glapp.height/10.f;
	else if (y > glapp.height-glapp.height/10.f)
		y = glapp.height-glapp.height/10.f;

	if (!input.State(VK_RBUTTON))
	{
		if (x < glapp.width/2-5)
			x += (100*timer.Delta());
		else if (x > glapp.width/2+5)
			x -= (100*timer.Delta());

		if (y < glapp.height/2-5)
			y += (100*timer.Delta());
		else if (y > glapp.height/2+5)
			y -= (100*timer.Delta());
	}

	
	//x = (old_x + x)/2;
	//y = (old_y + y)/2;
	Ray(dir);


/*	for (Unit *u = uman.uhead; u; u = u->next)
	{
		if (TargetLock(u->org))
		{
			target = u;
			colour[0] = 1;
			colour[1] = 0;
			colour[2] = 0;
			colour[3] = 1;
		}
		else
		{
			colour[0] = 1;
			colour[1] = 1;
			colour[2] = 1;
			colour[3] = 1;
		}
	}*/

}

void Crosshair::Display()
{
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4fv(colour);
	texture.Bind("./pics/crosshair.tga");
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x+size/2, y+size/2);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x-size/2, y+size/2);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x-size/2, y-size/2);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x+size/2, y-size/2);
	glEnd();
	glDisable(GL_BLEND);

}
	Vector3f testhit;
bool Crosshair::TargetLock (Vector3f pos)
{
	return false;
}

void Crosshair::Ray (Vector3f &v)
{
	static GLdouble np[3], fp[3];

	gluUnProject(x, y, 0,
		glapp.modelViewMatrix, glapp.projectionMatrix, glapp.viewPort,
		&np[0], &np[1], &np[2]);

	gluUnProject(x, y, 1,
		glapp.modelViewMatrix, glapp.projectionMatrix, glapp.viewPort,
		&fp[0], &fp[1], &fp[2]);
	

	v = Vector3f((float )fp[0], (float )fp[1], (float )fp[2]);
	v -= Vector3f((float )np[0], (float )np[1], (float )np[2]);
	v.Normalise();

}
