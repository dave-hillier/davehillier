// Ribbon.cpp: implementation of the Ribbon class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "timer.h"

#include "Ribbon.h"
 
RibbonPoint::RibbonPoint()
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
	head = NULL;
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
	for (RibbonPoint *r = head; r; r = r->next)
	{
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

// the list should always be ordered by age
	RibbonPoint *temp = NULL;
	for (RibbonPoint *r = head; r; r = r->next)
	{
		if ((life > 0) && (life + r->time_stamp < time))
		{
			if (r->prev)
				temp = r->prev;
			else
				temp = r->next;

			if (r == head)
			{

				head = r->next;
				if (head)
					head->prev = NULL;
				delete r;
			}
			else if (r->next == NULL)
			{
				if (r->prev)
					r->prev->next = NULL;

				delete r;
			}
			else
			{
				r->prev->next = r->next;
				r->next->prev = r->prev;
				delete r;
			}
			r = temp;

		}
	}

}


void Ribbon::AddPoint (Vector3f pt1, Vector3f pt2)
{
	RibbonPoint *ptr = new RibbonPoint();
	ptr->pt1 = pt1;
	ptr->pt2 = pt2;

	ptr->next = head;
	if (ptr->next)
		ptr->next->prev = ptr;

	ptr->prev = NULL;
	head = ptr;
}
