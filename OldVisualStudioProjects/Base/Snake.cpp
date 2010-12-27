#include "Snake.h"

#include <Windows.h>
#include <gl/GL.h>


Snake::Snake() : pos_(100,100,-100), speed_(0), heading_(0)
{

}


Snake::~Snake()
{

}

void Snake::Draw()
{
	glPushMatrix();
	glTranslatef(pos_.x(), pos_.y(), pos_.z());
	glLoadIdentity();
	glColor3f(0,1,0);
	glBegin(GL_LINES);
	glVertex3f(100,100,0);
	glVertex3f(0,100,0);
	glVertex3f(0,0,0);
	glVertex3f(100,0,0);
	glEnd();
	glPopMatrix();
}

void Snake::Update()
{
	pos_.x(pos_.x()+0.0001f*sinf(heading_));
	pos_.y(pos_.y()+0.0001f*cosf(heading_));

}
