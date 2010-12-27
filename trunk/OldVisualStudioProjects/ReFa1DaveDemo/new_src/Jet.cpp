
#include "Jet.h"

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <algorithm>

#include "TextureManager.h"
#include "Environment.h"
#include "DebugLog.h"

#include "../ModelManager.h"
#include "../ParticleManager.h"

#include "../Timer.h"
#include "../Camera.h"
#include "../Overlay.h"
#include "../Console.h"


#include "../Terrain.h"
#include "../Crosshair.h"
#include "../Rocket.h"
#include "../RapidFire.h"

namespace
{
	void drawLine(const Vector3f &from,const Vector3f &dir)
	{
	//	glDisable(GL_TEXTURE_2D);
	//	glBegin(GL_LINES);
	//	glVertex3f(from.x, from.y, from.z);

	//	Vector3f to = from+dir*100;
	//	glVertex3f(to.x, to.y, to.z);
	//	glEnd();
	//	glEnable(GL_TEXTURE_2D);
	}
}

Jet::Jet(JetController *c) : controller(c), power(0)
{

	org.Set(256, 50, 256); // arbitary position
	vel.Clear();
	acc.Clear();

	name = "Jet";
	// load model, textures
	model1 = mman.Load("./models/orca0.ms3d");
	model2 = mman.Load("./models/fan_engine.ms3d");
	model3 = mman.Load("./models/fan.ms3d");

	TextureManager::Instance().Load("./pics/envtest1.jpg", texture_id);

	grav = Vector3f(0, -9.81f, 0);// forgot the reason 
	// behind this scale
	mass = 1000; // set mass

	// set bbox
	bounds.max.x = 45;
	bounds.max.y = 26;
	bounds.max.z = 85;

	bounds.min.x = -45;
	bounds.min.y = -26;
	bounds.min.z = -150;

	bounds.min *= Environment::instance().scale();
	bounds.max *= Environment::instance().scale();

	//power = 0.5f;
	//rudder = 0;
	//pitch_control = 0;

	fan_rot = 0;
	health = 100;
}

Jet::~Jet()
{

}
void Jet::Render()
{
	//	Actor::Render(); // bbox?

	TextureManager::Instance().Apply(texture_id);
	//	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// do texture effects?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);

	glColor3f(1,1,1);

	glPushMatrix();

	glTranslatef (org.x,  org.y,  org.z);

	glRotatef (180,  0, 1, 0);
	glRotatef (ang.y,  0, 1, 0);
	glRotatef (-ang.x,  1, 0, 0);
	glRotatef (ang.z,  0, 0, 1);

	const float scale = Environment::instance().scale();
	glScalef(scale, scale, scale);

	mman.Render(model1);
	// engines
	glPushMatrix();
	glRotatef(-pitchControl()+90, 1,0,0);

	mman.Render(model2);

	fan_rot += 100*vel.Length() * timer.Delta();

	glPushMatrix();
	glTranslatef (48,0,0);
	glRotatef(fan_rot, 0, 1, 0);
	mman.Render(model3);
	glPopMatrix();
	glPushMatrix();
	glTranslatef (-48,0,0);
	glRotatef(fan_rot, 0, 1, 0);
	mman.Render(model3);
	glPopMatrix();



	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);


	glColor3f(0,0,1);
	drawLine(org, vel);
	glColor3f(0,1,0);
	drawLine(org, acc);
	glColor3f(0,1,1);
	drawLine(org, grav);


	ribbon1.Render();
	ribbon2.Render();

}

// TODO replace with proper detection
bool Jet::Collision (Actor *unit)
{
	if (unit == this)
		return false;

	// could do from each point of bbox
	return unit->TraceActor(org, vel);
}

void Jet::Update()
{
	const float scale = Environment::instance().scale();
	Actor *un;

	Vector3f forward;
	
	AngleVectors(ang, &forward, NULL, NULL);
	if (health > 0)
	{

		ang.y += rudder() * timer.Delta();

		if (ang.y < 0)
			ang.y += 360;
		else if (ang.y > 360)
			ang.y -= 360;

		ang.x += pitchControl() * timer.Delta();

		ang.z = -rudder();

		if (ang.z > 75)
			ang.z = 75;
		else if (ang.z < -75)
			ang.z = -75;
	}
	const float airDensity = (200/scale) / powf(org.y / scale, 2); // hmm tooo dense too low
	Vector3f drag = -vel * vel.Length() * 0.35f /** airDensity*/; // 0.35f arbitary 

	Vector3f thrust = forward * (power*0.5f+0.5f) * 4000000; // 2000000 a big number?! power from 0.5 to 2

	const float coeff = 2 * cosf(Deg2Rad(ang.x)) + 1; // arbitary
	const float speed = vel.Length();
	float liftY = coeff * airDensity * speed * speed; // from here http://en.wikipedia.org/wiki/Lift_(force)
	Vector3f lift(0, liftY, 0);
	
	acc = ((mass * grav) + thrust + drag + lift) / mass;
	vel += acc * timer.Delta();

	Vector3f old_org = org;

	org += scale * vel * timer.Delta();

	const float h = terrain.GetHeight(org.x, org.z) + scale * 80.0f;
	org.y = std::max<float>(h, org.y); // never allow below the terrain

	{
		bool c = false;
		Vector3f n;

		if (bounce_time < timer.GetTime())
		{
			for (std::list<Actor*>::iterator itt = Environment::instance().units.begin(); itt != Environment::instance().units.begin(); ++itt)
			{
				//Actor *
				un = *itt;
				if (Collision(un) && (un->health > 0))
				{
					con->Printf("%s", un->name);
					c = true;
					break;
				}
			}
		}
		if (c)
		{
			bounce_time = timer.GetTime() + 0.1f;

			n = (org - un->org);
			n.Normalise();

			// not true physics
			vel = 0.7f*vel.Reflection(n);
			un->vel = 0.7f*un->vel.Reflection(n);

			org = old_org +  Environment::instance().scale() * vel * timer.Delta();
			// should definately turn
			// TODO take into account angluar vel
			// TODO damage, 
		}
		/*	if (org.y < h)
		{
		terrain.GetNormal(org.x, org.z, &n);
		vel = vel.Reflection(-n);
		// damage?
		org = old_org +  Environment::instance().scale() * vel * timer.Delta();
		}*/
		// TODO terrain impacts
	}
	pman.Smoke(org);
	if (health <= 0)
	{
		health = 0;
		ang.z += 50*timer.Delta();

		// TODO reduce power over time
		if (power > 0 && !controller->power())
			power -= 5*timer.Delta();
		else if (power < 1 && !controller->power())
			power += 5*timer.Delta();
		if (power > 1)
			power = 1;

		// fire particles
		//
		if ((org.y < h) && (!expired))
		{
			// explode
			controller->die(this);

			expired = true;
			//	for (int i = 0; i < 10; i++)
			//		pman.Explosion(org);
			// delete self

			//vel.Clear();
			//acc.Clear();

			return;

		}
	}


	controller->update(this);

	if (health > 0 )
	{
		// TODO change this
		if (fire1Enabled())
			Fire1();
		if (fire1Enabled())
			Fire2();
	}

	Restrict();

	//if (power > 1)
	{
		Vector3f p1, p2, offset = Vector3f(25, 25, 145);
		offset *= Environment::instance().scale();

		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p1 = org + offset;

		offset = Vector3f(18, 0, 140);
		offset *= Environment::instance().scale();
		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p2 = org + offset;

		ribbon1.AddPoint(p1, p2);

		offset = Vector3f(-25, 25, 145);
		offset *= Environment::instance().scale();

		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p1 = org + offset;

		offset = Vector3f(-18, 0, 140);
		offset *= Environment::instance().scale();
		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p2 = org + offset;

		ribbon2.AddPoint(p2, p1);
	}
	ribbon1.Update();
	ribbon2.Update();
}


#define RESTRICT_ANGLE 85

void Jet::Restrict ()
{
	// prevent leaving the map
	/*	if ((org.z > terrain.height-terrain.height/15) &&
	(ang.y > 180 - RESTRICT_ANGLE) &&
	(ang.y < 180 + RESTRICT_ANGLE))
	rudder = 45;
	else if ((org.z < terrain.height/15) &&
	(ang.y > 360 - RESTRICT_ANGLE) &&
	(ang.y < 0 + RESTRICT_ANGLE))
	rudder = 45;
	else if ((org.x > terrain.width-terrain.width/15) &&
	(ang.y > 90 - RESTRICT_ANGLE) &&
	(ang.y < 90 + RESTRICT_ANGLE))
	rudder = 45;
	else if ((org.x < terrain.width/15) &&
	(ang.y > 270 - RESTRICT_ANGLE) &&
	(ang.y < 270 + RESTRICT_ANGLE))
	rudder = 45;*/

	const float scale = Environment::instance().scale();

	float h = terrain.GetHeight(org.x, org.z) + scale * 70.0f;

	if (org.y < h)
	{
		Vector3f offset = Vector3f(48 * scale, -15 * scale, 0);
		offset.RotateZ(-ang.z);
		offset.RotateY(ang.y);

		org.y = h;
		//	vel.y = 0;

		// perhaps throw debris/dustVector3f(org.x, org.y-scale * 70.0f, org.z) 
		//pman.Dirt(org + offset);

		offset = Vector3f(-48 * scale, -15 * scale, 0);
		offset.RotateZ(-ang.z);
		offset.RotateY(ang.y);

		//pman.Dirt(org + offset);
		// pitch up?
	}
	// restrict height
}


void Jet::Fire1()
{
	Vector3f f;
	if (fire_delay1 > timer.GetTime())
		return;
	else
		fire_delay1 = timer.GetTime() + 0.5f;

	controller->getAimDirection(this, &f);

	HomingRocket *r = new HomingRocket (org, f);
	r->owner = this;
	Environment::instance().add(r);

}

void Jet::Fire2()
{
	Vector3f f;

	if (fire_delay2 > timer.GetTime())
		return;
	else
		fire_delay2 = timer.GetTime() + 0.2f;

	controller->getAimDirection(this, &f);

	RapidFire *r = new RapidFire (org, f, 15);
	r->owner = this; // TODO move to ctor
	Environment::instance().add(r);
}

bool Jet::InFront2(Actor *target)
{
	Vector3f dir1, dir2;
	dir1 = target->org - org;
	dir1.Normalise();

	AngleVectors(ang, &dir2, NULL, NULL);
	float dot = dir1.Dot(dir2);

	if (dot > 0.99)
		return true;	
	else
		return false;	
}
