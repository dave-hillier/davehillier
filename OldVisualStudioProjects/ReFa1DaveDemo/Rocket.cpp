// Rocket.cpp: implementation of the Rocket class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "ms3d_model.h"
#include "new_src/TextureManager.h"
#include "modelmanager.h"
#include "ParticleManager.h"
#include "timer.h"
#include "terrain.h"
#include "Rocket.h"
#include "Console.h"
#include "new_src/Environment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Rocket::Rocket(Vector3f pos, Vector3f dir)
{
	//
	vel = dir*7500.0;
	org = pos;
	grav = Vector3f (0, -9.81f *40, 0);
	mass = 10;

	acc.Clear();

	time_stamp = timer.GetTime();
	life = 10.f;

	name = "Rocket";
}

Rocket::~Rocket()
{

}


void Rocket::Update()
{
	Actor *u = NULL;

	if (expired)
		return;

	if (life + time_stamp < timer.GetTime()) {
		expired = true; 
	}
	else if (org.y < terrain.GetHeight(org.x, org.z))	{
		expired = true;
		for (int i = 0; i < 10; i++)
		{
			pman.Explosion(org);
		}
	}
	else
	{
		Vector3f v = vel; v.Normalise();
//		for (u = Environment::instance().uhead; u; u = u->next)
		for(std::list<Actor*>::iterator itt = Environment::instance().units.begin(); itt != Environment::instance().units.end(); ++itt)
		{
			Actor *u = *itt;
			if ((u != this) && (u != owner) && (u->TraceActor(org, v)))
			{
				float l1, l2;
				l1 = (u->org - org).Length();
				l2 = Environment::instance().scale() * vel.Length() * timer.Delta();
				if (l1 < l2)
				{
					expired = true;

					for (int i = 0; i < 10; i++)
						pman.Explosion(org);

					u->health -= 50;
					con->Printf("hit %s health %d\n", u->name, u->health);
					break;
				}
			}
		}
	}

//	vel += acc * timer.Delta();
	org += Environment::instance().scale() * vel * timer.Delta();
	if (u)
		org = u->org;

	pman.Smoke(org);
//	//pman.TestSys(org);

	// if (target)
	// home-in

	
}
void Rocket::Render()
{
/*	glPushMatrix();
	TextureManager::Instance().Apply(tex_id);

    glTranslatef (org.x,  org.y,  org.z);

	glRotatef (ang.y,  0, 1, 0);
	glRotatef (ang.z,  0, 0, 1);
	glRotatef (-ang.x,  1, 0, 0);
	glScalef(Environment::instance().scale(), Environment::instance().scale(), Environment::instance().scale());
	mman.Render(model);
	glPopMatrix();*/

}



HomingRocket::HomingRocket(Vector3f pos, Vector3f dir)
{
	//
	vel = dir*7500.0;
	org = pos;
	grav = Vector3f (0, -9.81f *40, 0);
	mass = 10;

	acc.Clear();

	time_stamp = timer.GetTime();
	life = 30.f;
	target = NULL;

	name = "HomingRocket";
}

HomingRocket::~HomingRocket()
{

}


void HomingRocket::Update()
{
	Actor *u = NULL;

	if (expired)
		return;

	if (life + time_stamp < timer.GetTime()) {
		expired = true; 
	}
	else if (org.y < terrain.GetHeight(org.x, org.z))	{
		expired = true;
		//for (int i = 0; i < 10; i++)
		//{
		//	pman.Explosion(org);
		//}
	}
	else
	{
		Vector3f v = vel; v.Normalise();
		if (target)
		{
			Vector3f dir1, dir2;
			float speed = vel.Length();
			dir1 = target->org - org;
			dir1.Normalise();

			vel = (dir1 * 0.25 + v * 0.75)  * speed;
		}		

//		for (u = Environment::instance().uhead; u; u = u->next)
		for (std::list<Actor*>::iterator itt = Environment::instance().units.begin(); itt != Environment::instance().units.end(); ++itt)
		{
			Actor *u = *itt;
			if ((u == this) || (u == owner))
				continue;

			if (!target && (u->health > 0) && Visible(u) )
			{
				Vector3f dir1, dir2;
				dir1 = u->org - org;
				float range = dir1.Length();
				dir1 *= 1/range;
				
				float dot = dir1.Dot(v);
				if ((range < 25) && (dot > 0.99))
				{
					target = u;
					//con->Printf("%s\n", target->name);
				}
			}



			if (u->TraceActor(org, v))
			{
				float l1, l2;
				l1 = (u->org - org).Length();
				l2 = Environment::instance().scale() * vel.Length() * timer.Delta();
				if (l1 < l2)
				{
					expired = true;

					//for (int i = 0; i < 10; i++)
					//	pman.Explosion(org);

					u->health -= 50;
					con->Printf("hit %s health %d\n", u->name, u->health);
					break;
				}
			}
		}
	}

//	vel += acc * timer.Delta();
	org += Environment::instance().scale() * vel * timer.Delta();
	if (u)
		org = u->org;

	////pman.Smoke(org);
//	//pman.TestSys(org);

	// if (target)
	// home-in

	
}
void HomingRocket::Render()
{
/*	glPushMatrix();
	TextureManager::Instance().Apply(tex_id);

    glTranslatef (org.x,  org.y,  org.z);

	glRotatef (ang.y,  0, 1, 0);
	glRotatef (ang.z,  0, 0, 1);
	glRotatef (-ang.x,  1, 0, 0);
	glScalef(Environment::instance().scale(), Environment::instance().scale(), Environment::instance().scale());
	mman.Render(model);
	glPopMatrix();*/

}