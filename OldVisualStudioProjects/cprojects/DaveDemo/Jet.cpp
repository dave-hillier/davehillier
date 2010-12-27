// Jet.cpp: implementation of the Jet class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "ModelManager.h"
#include "texturemanager.h"
#include "ParticleManager.h"

#include "Timer.h"
#include "Camera.h"
#include "Input.h"
#include "Overlay.h"
#include "Console.h"


#include "Terrain.h"
#include "Jet.h"
#include "Crosshair.h"
#include "Rocket.h"
#include "RapidFire.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Jet::Jet()
{

	org.Set(256, 50, 256); // arbitary position
	vel.Clear();
	acc.Clear();

	strcpy(name, "Jet");
	// load model, textures
	model1 = mman.Load("./models/orca0.ms3d");
	model2 = mman.Load("./models/fan_engine.ms3d");
	model3 = mman.Load("./models/fan.ms3d");
	texture_id = texture.LoadTexture("./pics/envtest1.jpg");

	grav = Vector3f(0, -9.81f * 50, 0);// forgot the reason 
													// behind this scale
	mass = 1000; // set mass

	// set bbox
	bounds.max.x = 45;
	bounds.max.y = 26;
	bounds.max.z = 85;

	bounds.min.x = -45;
	bounds.min.y = -26;
	bounds.min.z = -150;

	bounds.min *= unit_scale;
	bounds.max *= unit_scale;

	power = 0;
	rudder = 0;
	pitch_control = 0;

	ai = NULL;
	client = NULL;

	fan_rot = 0;
	health = 100;
	test = false;
}

Jet::~Jet()
{

}
void Jet::Render()
{
//	Unit::Render(); // bbox?

	texture.Bind(texture_id);
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

	glScalef(unit_scale, unit_scale, unit_scale);

	mman.Render(model1);
	// engines
	glPushMatrix();
	glRotatef(-pitch_control+90, 1,0,0);

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

	ribbon1.Render();
	ribbon2.Render();

}

// TODO replace with proper detection
bool Jet::Collision (Unit *unit)
{
	if (unit == this)
		return false;

	// could do from each point of bbox
	return unit->TraceUnit(org, vel);
}

void Jet::Update()
{
	Unit *un;

	Vector3f forward;
	Vector3f drag, lift, thrust;


	Unit::AngleVectors(&forward, NULL, NULL);
	if (health > 0)
	{
		ang.y += rudder * timer.Delta();

		if (ang.y < 0)
			ang.y += 360;
		else if (ang.y > 360)
			ang.y -= 360;

		ang.x += pitch_control * timer.Delta();

		ang.z = -rudder;

		if (ang.z > 75)
			ang.z = 75;
		else if (ang.z < -75)
			ang.z = -75;

		lift.Clear();
		lift.y += 500*vel.Length()*(float)cos((float)Deg2Rad(ang.x)); // needs some other multiplier i think
	}
	drag = -vel * 0.35f * vel.Length();

	thrust = forward * power * 2000000.f; // 

	acc = (mass * grav) + thrust + drag + lift;
	acc *= 1.0f/mass;


	vel += acc * timer.Delta();
	Vector3f old_org = org;
	org += unit_scale * vel * timer.Delta();

	float h = terrain.GetHeight(org.x, org.z) + unit_scale * 80.0f;

	{
		bool c = false;
		Vector3f n;

		if (bounce_time < timer.GetTime())
		{
			for (un = uman.uhead; un; un = un->next)
			{
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

			org = old_org +  unit_scale * vel * timer.Delta();
			// should definately turn
			// TODO take into account angluar vel
			// TODO damage, 
		}
	/*	if (org.y < h)
		{
			terrain.GetNormal(org.x, org.z, &n);
			vel = vel.Reflection(-n);
			// damage?
			org = old_org +  unit_scale * vel * timer.Delta();
		}*/
		// TODO terrain impacts
	}
	if (health <= 0)
	{
		health = 0;
		ang.z += 50*timer.Delta();
		if (power > 0)
			power -= 5*timer.Delta();
			// fire particles
		pman.Smoke(org);
		if ((org.y < h) && (!delete_me))
		{
			// explode
			delete_me = true;
			for (int i = 0; i < 10; i++)
				pman.Explosion(org);
			// delete self

			vel.Clear();
			acc.Clear();

			if (client && !test)
			{
				for (un = uman.uhead; un; un = un->next)
				{
					if (stricmp(un->name, "Start") == 0)
					{
						Jet *jet = new Jet ();
						jet->org = un->org;
						jet->ang = un->ang;
						jet->client = new PlayerJet();
						strcpy(jet->name, "Player");
						SetPlayer(jet);
						test = true;
						return;
					}
				}
			}
			return;

		}
	}
	if (client)
	{
		overlay.health = health/100.0f;
		overlay.vel = vel.Length()/3500.0f;

		if ( health > 0 )
		{

			client->Update();
			pitch_control = client->pitch_control;
			power = client->power;
			rudder = client->rudder;


			if (input.State('C'))
			{
				acc.Clear();
				vel.Clear();
				power = 0;
			}

			if (client->fire1)
				Fire1();
			if (client->fire2)
				Fire2();
		}

		Vector3f forward, up;
		AngleVectors(&forward, NULL, &up);
		camera.Chase(org, forward, up, ang);
	}
	else if (ai && ( health > 0 ))
	{
		ai->Update();

		pitch_control = ai->pitch_control;
		power = ai->power;
		rudder = ai->rudder;

		if (ai->fire1)
			Fire1();
		if (ai->fire2)
			Fire2();
	}
	Restrict();

	//if (power > 1)
	{
		Vector3f p1, p2, offset = Vector3f(25, 25, 145);
		offset *= unit_scale;

		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p1 = org + offset;

		offset = Vector3f(18, 0, 140);
		offset *= unit_scale;
		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p2 = org + offset;

		ribbon1.AddPoint(p1, p2);

		offset = Vector3f(-25, 25, 145);
		offset *= unit_scale;

		offset.RotateZ(-ang.z);
		offset.RotateX(ang.x);
		offset.RotateY(ang.y);

		p1 = org + offset;

		offset = Vector3f(-18, 0, 140);
		offset *= unit_scale;
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





	float h = terrain.GetHeight(org.x, org.z) + unit_scale * 70.0f;


	if (org.y < h)
	{
		Vector3f offset = Vector3f(48 * unit_scale, -15 * unit_scale, 0);
		offset.RotateZ(-ang.z);
		offset.RotateY(ang.y);

		org.y = h;
	//	vel.y = 0;

		// perhaps throw debris/dustVector3f(org.x, org.y-unit_scale * 70.0f, org.z) 
		pman.Dirt(org + offset);

		offset = Vector3f(-48 * unit_scale, -15 * unit_scale, 0);
		offset.RotateZ(-ang.z);
		offset.RotateY(ang.y);
		
		pman.Dirt(org + offset);
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

	if (client)
		f = xhair.dir;
	else
		AngleVectors(&f, NULL, NULL);
	HomingRocket *r = new HomingRocket (org, f);
	r->owner = this;

	
}

void Jet::Fire2()
{
	Vector3f f;

	if (fire_delay2 > timer.GetTime())
		return;
	else
		fire_delay2 = timer.GetTime() + 0.2f;

	if (client)
		f = xhair.dir;
	else
		AngleVectors(&f, NULL, NULL);
	
	RapidFire *r = new RapidFire (org, f, 15);
	r->owner = this;

}

bool Jet::InFront2(Unit *target)
{
	Vector3f dir1, dir2;
	dir1 = target->org - org;
	dir1.Normalise();

	AngleVectors(&dir2, NULL, NULL);
	float dot = dir1.Dot(dir2);

	if (dot > 0.99)
		return true;	
	else
		return false;	
}
