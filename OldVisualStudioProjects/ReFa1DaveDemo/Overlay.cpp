// Overlay.cpp: implementation of the Overlay class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glapp.h"
#include "new_src/TextureManager.h"
#include "Overlay.h"
Overlay overlay;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Overlay::Overlay()
{

}

Overlay::~Overlay()
{

}

void Overlay::Display()
{
	if (vel > 1)
		vel = 1;
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	TextureManager::Instance().Apply("./pics/spectrum.jpg");
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 1.0f-vel);
		glVertex2i(glapp.width-16, 32+(int)(256*vel));
		glTexCoord2f(1.0f, 1.0f-vel);
		glVertex2i(glapp.width-32, 32+(int)(256*vel));
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(glapp.width-32, 32);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(glapp.width-16, 32);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 1.0f-health);
		glVertex2i(glapp.width-48, 32+(int)(256*health));
		glTexCoord2f(1.0f, 1.0f-health);
		glVertex2i(glapp.width-64, 32+(int)(256*health));
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(glapp.width-64, 32);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(glapp.width-48, 32);
	glEnd();


}

void Overlay::Update()
{ 

}

