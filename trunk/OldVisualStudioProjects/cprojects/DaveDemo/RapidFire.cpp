// RapidFire.cpp: implementation of the RapidFire class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

//#include "ms3d_model.h"
//#include "texturemanager.h"

#include "ParticleManager.h"
#include "Ribbon.h"
#include "timer.h"
#include "terrain.h"
#include "Console.h"

#include "RapidFire.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RapidFire::RapidFire(Vector3f pos, Vector3f dir, int dam)
{
	damage = dam;
	vel = dir*20000.0;
	org = pos;
	grav = Vector3f (0, -9.81f *40, 0);
	mass = 1;

	acc.Clear();

	time_stamp = timer.GetTime();
	life = 1.f;

	strcpy(name, "RapidFire");


	vrib.colour_start[0] = hrib.colour_start[0] = 0;
	vrib.colour_start[1] = hrib.colour_start[1] = 1;
	vrib.colour_start[2] = hrib.colour_start[2] = 0;
	vrib.colour_start[3] = hrib.colour_start[3] = 0.5;
	vrib.colour_end[0] = hrib.colour_end[0] = 1;
	vrib.colour_end[1] = hrib.colour_end[1] = 1;
	vrib.colour_end[2] = hrib.colour_end[2] = 0;
	vrib.colour_end[3] = hrib.colour_end[3] = 0;
	vrib.life = hrib.life = 0.1f;
}

RapidFire::~RapidFire()
{

}

#define ZAP_SIZE 0.005f
void RapidFire::Update()
{
	Unit *u = NULL;

	if (delete_me)
		return;

	if (life + time_stamp < timer.GetTime())
		delete_me = true; 
	else if (org[1] < terrain.GetHeight(org[0], org[2]))
	{
		for (int i = 0; i < 5; i++)
			pman.Explosion2(org);
	//	con->Printf("hit terrain");
		delete_me = true;
	}
	else
	{
		Vector3f v = vel; 
		v.Normalise();

		for (u = uman.uhead; u; u = u->next)
		{
			if ((u->health > 0) && (u != this) && (u != owner) && (u->TraceUnit(org, v)))
			{
				float l1, l2;
				l1 = (u->org - org).Length();
				l2 = unit_scale * vel.Length() * timer.Delta();
				if (l1 < l2)
				{
					delete_me = true;
					vel.Clear();

					for (int i = 0; i < 5; i++)
						pman.Explosion2(org);

					u->health -= damage;

					con->Printf("hit %s health %d\n", u->name, u->health);
					break;
				}
			}
		}
	}
	org += unit_scale * vel * timer.Delta();

	if (u)
		org = u->org;
//	pman.Trail(org);

	Vector3f p1 = org, p2 = org;
	p1.x -= ZAP_SIZE;
	p2.x += ZAP_SIZE;

	hrib.AddPoint(p1, p2);

	p1.x += ZAP_SIZE;
	p2.x -= ZAP_SIZE;

	p1.y += ZAP_SIZE;
	p2.y -= ZAP_SIZE;

	vrib.AddPoint(p1, p2);

	hrib.Update();
	vrib.Update();
	
}

void RapidFire::Render()
{
	vrib.Render();
	hrib.Render();
}