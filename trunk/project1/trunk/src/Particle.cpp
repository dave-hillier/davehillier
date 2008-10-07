#include "Particle.h"
#include "RenderManager.h"
#include "maths/matrix.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

void Particle::update(double delta, const vector3& force)
{
	float mass = 1;
	vel_ += force * (float)delta / mass;
	pos_ += vel_ * (float)delta;
}


void Particle::renderVelocityAligned(const vector3& eye, float length, float scale, bool wireFrame) const
{
	vector3 y = vel_;
	y.norm();
	y *= length;
	vector3 x = (pos_ - eye) * y;
	x.norm();
	renderAxisAligned(x,y,scale, wireFrame);
}

void Particle::renderAxisAligned(const vector3& x, const vector3& y, float scale, bool wireFrame) const
{
	vector3 pts[4];
	pts[0] = pos_ + ((-x - y) * scale);
	pts[1] = pos_ + ((x - y) * scale);
	pts[2] = pos_ + ((x + y) * scale);
	pts[3] = pos_ + ((-x + y) * scale);

	if (wireFrame)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(pts[0].x, pts[0].y, pts[0].z);
		glVertex3f(pts[1].x, pts[1].y, pts[1].z);
		glVertex3f(pts[2].x, pts[2].y, pts[2].z);
		glVertex3f(pts[3].x, pts[3].y, pts[3].z);
		glVertex3f(pts[0].x, pts[0].y, pts[0].z);
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		glVertex3f(pts[0].x, pts[0].y, pts[0].z);
		glVertex3f(pts[1].x, pts[1].y, pts[1].z);
		glVertex3f(pts[2].x, pts[2].y, pts[2].z);
		glVertex3f(pts[3].x, pts[3].y, pts[3].z);
		glEnd();
	}
}

void Particle::setColour(double timeNow, float life, const vector4& colourStart, const vector4& colourEnd) const
{
	vector4 colour;
	float f = float(((timeStamp_ + life) - timeNow) / life);
	lerp(colour, colourEnd, colourStart, f);
	glColor4f(colour.x,colour.y,colour.z,colour.w);

}

