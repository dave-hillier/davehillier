// Rocket.cpp: implementation of the Rocket class.
//
//////////////////////////////////////////////////////////////////////

#include "Rocket.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "ms3d_model.h"
#include "texturemanager.h"
#include "ParticleManager.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Rocket::Rocket(Vector3 pos, Vector3 dir, Vector3 targ, Vector3 firer_vel)
{
	//
	vel = dir*2.0*firer_vel.Length();
	org = pos;
	grav = Vector3 (0, -9.81f *40, 0);
	target = dir;
	mass = 10;

	acc.Clear();

	time_stamp = timer.GetTime();
	life = 2000;
}

Rocket::~Rocket()
{
	// create an explosion
}


void Rocket::Update()
{

//	vel += acc * timer.Delta();
	org += vel * timer.Delta();

	pman.Smoke(org);


	if (life + time_stamp < timer.GetTime())
	{
		uman.Delete(this);
		return;
	}

//	if (org[0] < 0)
//		delete this;
}
void Rocket::Render()
{

}