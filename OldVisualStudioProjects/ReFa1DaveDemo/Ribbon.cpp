// Ribbon.cpp: implementation of the Ribbon class.
//
//////////////////////////////////////////////////////////////////////
#include "Ribbon.h"

#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "timer.h"
#include <algorithm>
 
Ribbon::Point::Point(const Vector3f &p1, const Vector3f &p2) : pt1(p1), pt2(p2)
{
	time_stamp = timer.GetTime();
}

//////////////////////////////////////////////////////////////////////
// Ribbon Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Ribbon::Ribbon()
{
	colour_start[0] = 1;
	colour_start[1] = 0;
	colour_start[2] = 0;
	colour_start[3] = 0.75;
	colour_end[0] = 1;
	colour_end[1] = 1;
	colour_end[2] = 0;
	colour_end[3] = 0;

	life = 0.1f;
}

Ribbon::~Ribbon()
{

}

void Ribbon::Render()
{
	float time = timer.GetTime();

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	for (std::deque<Point>::iterator itt = points.begin(); itt != points.end(); ++itt)
	{
		Point *r = &*itt;
		float colour[4];
		float blend = (time - r->time_stamp)/life;

		if (blend > 1)
			blend = 1;

		colour[0] = (1-blend) * colour_start[0] + (blend) * colour_end[0];
		colour[1] = (1-blend) * colour_start[1] + (blend) * colour_end[1];
		colour[2] = (1-blend) * colour_start[2] + (blend) * colour_end[2];
		colour[3] = (1-blend) * colour_start[3] + (blend) * colour_end[3];

		glColor4fv(colour);
		glVertex3fv(r->pt1);
		glVertex3fv(r->pt2);
	}
	glEnd();
	glDepthMask(GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

}
	// has to dynamically grow for varying frame rates

void Ribbon::Update()
{
	float time = timer.GetTime();
	float delta = timer.Delta();

	if ((life > 0) && (life + points.front().time_stamp < time))
	{
		points.pop_front();
	}
}

void Ribbon::AddPoint (Vector3f pt1, Vector3f pt2)
{
	points.push_back(Point(pt1, pt2));
}
