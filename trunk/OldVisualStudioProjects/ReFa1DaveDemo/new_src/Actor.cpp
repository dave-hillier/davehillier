
#include "Actor.h"

#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "../terrain.h"

#include "Environment.h"


Actor::Actor() : expired(false)
{
	vel.Clear();
	acc.Clear();
}

Actor::~Actor()
{

}

bool Actor::TraceActor(Vector3f pick_start, Vector3f pick_dir) {
	float maxT[3] = {0,0,0};
	float coord[3];
	int i;

	// Convert ray into the unit's coordinate system
	pick_start.Sub(org);

	/*	pick_start.RotateZ(ang.z);
	pick_start.RotateX(-ang.x);
	pick_start.RotateY(-ang.y);
	pick_dir.RotateZ(ang.z);
	pick_dir.RotateX(-ang.x);
	pick_dir.RotateY(-ang.y);*/

	pick_start.RotateZ(-ang.z);
	pick_start.RotateX(ang.x);
	pick_start.RotateY(-ang.y);// extra 180?

	pick_dir.RotateZ(-ang.z);
	pick_dir.RotateX(ang.x);
	pick_dir.RotateY(-ang.y);




	// Find candidate planes.
	for (i=0; i<3; i++) {
		if (pick_start.vec[i] < bounds.min.vec[i]) {
			coord[i] = bounds.min.vec[i];
			// Calculate T distances to candidate planes
			if ((unsigned int &)pick_dir.vec[i])
				maxT[i] = (bounds.min.vec[i] - pick_start.vec[i]) / pick_dir.vec[i];
			else
				maxT[i] = -1.0f;
		}
		else if (pick_start.vec[i] > bounds.max.vec[i]) {
			coord[i] = bounds.max.vec[i];
			// Calculate T distances to candidate planes
			if ((unsigned int &)pick_dir.vec[i])
				maxT[i] = (bounds.max.vec[i] - pick_start.vec[i]) / pick_dir.vec[i];
			else
				maxT[i] = -1.0f;
		}
	}

	// Get largest of the maxT's for final choice of intersection
	int plane = 0;
	if (maxT[1] > maxT[plane])
		plane = 1;
	if (maxT[2] > maxT[plane])
		plane = 2;

	// Check final candidate actually inside box
	if ((unsigned int &)(maxT[plane]) & 0x80000000)
		return false;

	for (i=0; i<3; i++) {
		if (i != plane) {
			coord[i] = pick_start.vec[i] + maxT[plane] * pick_dir.vec[i];
			if (coord[i] < bounds.min.vec[i] || coord[i] > bounds.max.vec[i])
				return false;
		}
	}

	return true;
}

// there is a possible line of site, but its not infront
bool Actor::Visible(Actor *target)
{
	Vector3f dir, temp;
	float len;

	dir = target->org - org;
	len = dir.Length();
	dir.Normalise();

	terrain.PickPoint(&temp, org, dir); 

	temp = temp - org;

	if (temp.Length() < len)
		return false;
	else
		return true;
}

bool Actor::InFront(Actor *target)
{
	Vector3f dir1, dir2;
	dir1 = target->org - org;
	dir1.Normalise();

	AngleVectors(ang, &dir2, NULL, NULL);
	float dot = dir1.Dot(dir2);

	if (dot > 0.3)
		return true;	
	else
		return false;	
}


void Actor::Render ()
{
	glPushMatrix();
	glTranslatef (org.x,  org.y,  org.z);

	/*	glRotatef (ang.x,  1, 0, 0);
	glRotatef (ang.z,  0, 0, 1);
	glRotatef (ang.y,  0, 1, 0);*/
	glRotatef (180,  0, 1, 0);
	glRotatef (ang.y,  0, 1, 0);
	glRotatef (-ang.x,  1, 0, 0);
	glRotatef (ang.z,  0, 0, 1);

	const float scale = Environment::instance().scale();
	glScalef(scale, scale, scale);

	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(bounds.min.x, bounds.max.y, bounds.min.z);
	glVertex3f(bounds.max.x, bounds.max.y, bounds.min.z);
	glVertex3f(bounds.max.x, bounds.max.y, bounds.max.z);
	glVertex3f(bounds.min.x, bounds.max.y, bounds.max.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(bounds.min.x, bounds.min.y, bounds.min.z);
	glVertex3f(bounds.max.x, bounds.min.y, bounds.min.z);
	glVertex3f(bounds.max.x, bounds.min.y, bounds.max.z);
	glVertex3f(bounds.min.x, bounds.min.y, bounds.max.z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(bounds.min.x, bounds.min.y, bounds.min.z);
	glVertex3f(bounds.min.x, bounds.max.y, bounds.min.z);

	glVertex3f(bounds.max.x, bounds.min.y, bounds.min.z);
	glVertex3f(bounds.max.x, bounds.max.y, bounds.min.z);

	glVertex3f(bounds.max.x, bounds.min.y, bounds.max.z);
	glVertex3f(bounds.max.x, bounds.max.y, bounds.max.z);

	glVertex3f(bounds.min.x, bounds.min.y, bounds.max.z);
	glVertex3f(bounds.min.x, bounds.max.y, bounds.max.z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();

}
