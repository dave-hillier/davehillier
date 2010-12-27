// Turret.cpp: implementation of the Turret class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "RapidFire.h"
#include "Rocket.h"

#include "texturemanager.h"
#include "modelmanager.h"
#include "particlemanager.h"
#include "terrain.h"
#include "timer.h"
#include "console.h"

#include "Turret.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int turret_0, turret_1, turret_2;
int turret_block, turret_base, turret_dead;
int metal_tex, concrete_tex;
bool turret_loaded = false;

Turret::Turret(int t)
{
	// setup health, bounds etc
	health = 150;
	type = t;

	bounds.max.x = 200;
	switch (type) // TODO should really make enum
	{
	case 2:
		bounds.max.y = 490;
		break;
	case 1:
		bounds.max.y = 385;
		break;
	case 0:
		bounds.max.y = 450;
		break;
	}
	bounds.max.z = 200;

	bounds.min.x = -200;
	bounds.min.y = 0;
	bounds.min.z = -200;

	bounds.min *= unit_scale;
	bounds.max *= unit_scale;

	sprintf(name, "turret%d", t);

	barrel = 0;

	if (turret_loaded)
		return;
	turret_loaded = true;

	metal_tex = texture.LoadTexture("./pics/bluemetal.jpg");
	concrete_tex = texture.LoadTexture("./pics/concrete.jpg");

	turret_0 = mman.Load("./models/turret_1.ms3d");
	turret_1 = mman.Load("./models/turret_2.ms3d");
	turret_2 = mman.Load("./models/turret_3.ms3d");

	turret_block = mman.Load("./models/turret_block.ms3d");
	turret_base = mman.Load("./models/turret_base.ms3d");
	turret_dead = mman.Load("./models/turret_dead.ms3d");

}

void Turret::GunVector (Vector3f *forward)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = (float)Deg2Rad(gun_ang.y);
	sy = (float)sin((float)angle);
	cy = (float)cos((float)angle);
	angle = (float)Deg2Rad(gun_ang.x);
	sp = (float)sin((float)angle);
	cp = (float)cos((float)angle);
	angle = (float)Deg2Rad(gun_ang.z);
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
}

Turret::~Turret()
{


}

void Turret::Update()
{
	Unit *unit, *targ = NULL;

	org.y += 325*unit_scale;

/*	if (health >0)
	{
		for (unit = uman.uhead; unit; unit = unit->next)
		{
			if (Visible(unit) && (stricmp("player", unit->name) == 0))
			{
				targ = unit;
				break;
			}
		}
	}*/
	unit = GetPlayer();
	if (Visible(unit) && (health >0))
		targ = unit;

	org.y -= 325*unit_scale;

	if (targ)
	{
		Vector3f vec = targ->org - org;
		float desired_y = (float)atan2(vec.x, vec.z);
		desired_y = (float)Rad2Deg(desired_y);

		desired_y += 180;
		if (desired_y < 0)
			desired_y += 360;
		else if (desired_y > 360)
			desired_y -= 360;

		gun_ang.y = desired_y;

		float vecy = vec.y - 325*unit_scale;
		vec.y = 0;
		float xz = vec.Length(); 
		vec.y = vecy;
		float desired_x = (float)Rad2Deg(atan2(vecy, xz));

		gun_ang.x = desired_x;

		Vector3f f;
		GunVector (&f);
		vec = org;
		vec.y += + 325*unit_scale;
		
		if (time_stamp < timer.GetTime() && targ->TraceUnit(vec, f))
		{
			// the only diff is max angles and fire type
			switch (type) // TODO should really make enum
			{
			case 2: // no tilt really but fire homing rockets
				{
					if (gun_ang.x > 15)
						gun_ang.x = 15;
					else if (gun_ang.x < -15)
						gun_ang.x = -15;

					Rocket *r = new Rocket (vec, f);
					r->owner = this;

					time_stamp = timer.GetTime() + 2.0f;				// check range

					break;
				}
			case 1: // tilt loads fire slightly better than rapid fire
				{
					if (gun_ang.x > 65)
						gun_ang.x = 65;
					else if (gun_ang.x < -65)
						gun_ang.x = -65;

					Vector3f offset = Vector3f(-15, 20, 115);
					offset *= unit_scale;
					//offset.RotateZ(-ang.z);
					offset.RotateX(ang.x+gun_ang.x);
					offset.RotateY(ang.y+gun_ang.y);
					vec	+= offset;

					RapidFire *r = new RapidFire (vec, f);
					r->owner = this;

					time_stamp = timer.GetTime() + 0.15f;
					break;
				}
			case 0: // tilt 25deg max fire crazyiness infrequently
				{
					if (gun_ang.x > 25)
						gun_ang.x = 25;
					else if (gun_ang.x < -25)
						gun_ang.x = -25;

					if (!barrel)
						barrel = 1;
					else
						barrel = 0;

					Vector3f offset = Vector3f(-50 + 100.0f*barrel, 20, 420);
					offset *= unit_scale;
					offset.RotateX(ang.x+gun_ang.x);
					offset.RotateY(ang.y+gun_ang.y);
					vec	+= offset;

					RapidFire *r = new RapidFire (vec, f, 50);
					r->owner = this;

					time_stamp = timer.GetTime() + 0.5f;
					break;
				}
			}
		}
	}
}

void Turret::Render()
{
//	Unit::Render();
	glPushMatrix();

	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

    glTranslatef (org.x,  org.y,  org.z);
	glRotatef (ang.y,  0, 1, 0);
	glRotatef (-ang.x,  1, 0, 0);
	glRotatef (ang.z,  0, 0, 1);

	//scale!
	glScalef(unit_scale, unit_scale, unit_scale);
	// render base
	texture.Bind(concrete_tex);

	if (health <= 0)
	{
		mman.Render(turret_dead);

	//	for (int i=0; i < 10; i++)
		{
			Vector3f pt;
			pt.Set(rand() % 10 - 5.0f, 
				rand() % 10 - 5.0f, 
				rand() % 10 - 5.0f);
			pt *= unit_scale;
			pt = org;
			pman.Smoke2(org);
		}
	}
	else	
	{
		mman.Render(turret_block);	

			glPushMatrix();
			// render lower 
			texture.Bind(metal_tex);
			glRotatef (gun_ang.y,  0, 1, 0);
			mman.Render(turret_base);	
			// render gun
				glPushMatrix();
				glTranslatef(0, 325, 0);// already scaled
				glRotatef (gun_ang.x,  1, 0, 0);

				texture.Bind(metal_tex);
				switch (type) // TODO should really make enum
				{
				case 2:
					mman.Render(turret_2);	
					break;
				case 1:
					mman.Render(turret_1);	
					break;
				case 0:
					mman.Render(turret_0);	
					break;
				}

				glPopMatrix();

			glPopMatrix();
	}
	glPopMatrix();
}
