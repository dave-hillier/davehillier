// Unit.cpp: implementation of the Unit class.
// Should be an abstract class for objects
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "terrain.h"
#include "Unit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//float unit_scale = 0.00125f;
float unit_scale = 0.0034f;//0.0017f;
//float unit_scale = 0.05f;

//Unit *uhead = NULL;
Unit::Unit()
{
	next = uman.uhead;
	if (next)
		next->prev = this;

	prev = NULL;
	uman.uhead = this;
	delete_me = false;

	vel.Clear();
	acc.Clear();

}

Unit::~Unit()
{

}

Unit *Unit::Delete ()
{
	if (next && prev)
	{
		prev->next = next;
		next->prev = prev;
		delete this;
		return prev;
	}
	if (!next && prev)
	{
		prev->next = NULL;
		delete this;
		return prev;
	}
	else
	{
		if (next)
			next->prev = prev;
		uman.uhead = next;
		delete this;
		return uman.uhead;
	}
}

bool Unit::TraceUnit(Vector3f pick_start, Vector3f pick_dir) {
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
bool Unit::Visible(Unit *target)
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

bool Unit::InFront(Unit *target)
{
	Vector3f dir1, dir2;
	dir1 = target->org - org;
	dir1.Normalise();

	AngleVectors(&dir2, NULL, NULL);
	float dot = dir1.Dot(dir2);

	if (dot > 0.3)
		return true;	
	else
		return false;	
}

void Unit::AngleVectors (Vector3f *forward, Vector3f *right, Vector3f *up)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = (float)Deg2Rad(ang.y);
	sy = (float)sin((float)angle);
	cy = (float)cos((float)angle);
	angle = (float)Deg2Rad(ang.x);
	sp = (float)sin((float)angle);
	cp = (float)cos((float)angle);
	angle = (float)Deg2Rad(ang.z);
	sr = (float)sin((float)angle);
	cr = (float)cos((float)angle);

	if (forward)
	{
		forward->z = -cp*cy;
		forward->x = -cp*sy;
		forward->y = sp;
	/*	forward->z = cp*cy;
		forward->x = cp*sy;
		forward->y = -sp;*/
	}
	if (right)
	{
		right->z = -(-1*sr*sp*cy+-1*cr*-sy);
		right->x = -(-1*sr*sp*sy+-1*cr*cy);
		right->y = 1*sr*cp;
	}
	if (up)
	{
		up->z = (cr*sp*cy+-sr*-sy);
		up->x = (cr*sp*sy+-sr*cy);
		up->y = cr*cp;
	}
}

void Unit::Render ()
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

	glScalef(unit_scale, unit_scale, unit_scale);

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

void UManager::Render ()
{
	Unit *u;
	for (u = uhead; u; u = u->next)
	{
		u->Render();
	}
}
void UManager::Update ()
{
	Unit *u, *dead = NULL;
	for (u = uhead; u; u = u->next)
	{
		if (u->org.z > terrain.height-1)
			u->org.z = (float)terrain.height-1;
		else if (u->org.z < 0.0f)
			u->org.z = 0.0f;
		if (u->org.x > terrain.width-1)
			u->org.x = (float)terrain.width-1;
		else if (u->org.x < 0.0f)
			u->org.x = 0.0f;
		if (dead)
		{
			dead->Delete();
			dead = NULL;
		}
		u->Update();
		if (u->delete_me)
			dead = u;
	}
}
UManager::UManager ()
{
}

UManager uman;


Unit *player_unit;
void SetPlayer (Unit *unit)
{
	player_unit = unit;
}

Unit *GetPlayer ()
{
	return player_unit;
}
