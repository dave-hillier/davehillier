// AIJet.cpp: implementation of the AIJet class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <gl/gl.h>
#include "Input.h"
#include "Timer.h"
#include "Console.h"

#include "Jet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AIJet::AIJet()
{
	rudder = 0; 
	power = 0;
	pitch_control = 0;

	fire1 = false;
	fire2 = false;


}

AIJet::~AIJet()
{

}

void AIJet::Update ()
{
	Unit *unit, *targ = NULL;
	Vector3f f, r, u;
	parent->AngleVectors(&f, &r, &u);

	// avoid other units/collisions
	// if something directly infront
	// then try move left or right to avoid
	// perhaps an arbitary choice whether its left, right, up or down

	// find target
/*	for (unit = uman.uhead; unit; unit = unit->next)
	{
		if (parent->Visible(unit) && 
			(stricmp("player", unit->name) == 0) &&
			(unit->health > 0))
		{
			targ = unit;
			break;
		}
	}*/
	unit = GetPlayer();
	if (parent->Visible(unit) && (unit->health > 0))
		targ = unit;

	if (targ)
	{
		Vector3f vec = targ->org - parent->org;
/*
#ifdef _DEBUG

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINES);

		glColor3f(1,0,0);
		
		glVertex3fv(parent->org + vec);
		glVertex3fv(parent->org);

		glColor3f(0,1,0);
		glVertex3fv(parent->org + f);
		glVertex3fv(parent->org);

		glColor3f(0,1,1);
		glVertex3fv(parent->org + u);
		glVertex3fv(parent->org);

		glColor3f(1,1,0);
		glVertex3fv(parent->org + r);
		glVertex3fv(parent->org);
		glEnd();

		glEnable(GL_TEXTURE_2D);
#endif // _DEBUG*/

		// fix turning across 360
		float desired_y = (float)atan2(vec.x, vec.z);
		desired_y = (float)Rad2Deg(desired_y);

		desired_y += 180;
		if (desired_y < 0)
			desired_y += 360;
		else if (desired_y > 360)
			desired_y -= 360;

		if (desired_y != parent->ang.y)
			rudder = desired_y - parent->ang.y; 

		if (rudder > 180)
			rudder -= 360;

		rudder *= 2; // prob depend on difficulty

		float vecy = vec.y;
		vec.y = 0;
		float xz = vec.Length(); 
		float desired_x = (float)Rad2Deg(atan2(vecy, xz));

	//	con->Printf("desired pitch = %.2f", desired_x);
		if (desired_x != parent->ang.x)
			pitch_control = desired_x - parent->ang.x;

		if (pitch_control > 180)
			pitch_control -= 360;

		pitch_control *= 2;

		vec.y = vecy;
		float range = vec.Length();
		if (range > 300*50*unit_scale)
			power = 1;
		else if (range > 100*50*unit_scale)
			power = 1.25;
		else
			power = 0.5f;

		//power = 0; // stay still bitch

		fire1 = false;
		fire2 = false;

		if (((Jet *)parent)->InFront2(targ))
		{		
			// maybe fire a missile
			fire2 = true;

			// check for obstacles
			float l1, l2;
			l2 = vec.Length();
			vec.Normalise();
			for (Unit *u = uman.uhead; u; u = u->next)
			{

				if ((u != targ) && (u != parent) && (u->TraceUnit(parent->org, vec)))
				{
					l1 = (u->org - parent->org).Length();
					if (l1 < l2)
						fire2 = false;

				}
			}

		}
	}


	// power should depend on range
	// they should go as fast as they can to catch enemy
	// once in range dont need to go as fast as 
	// its easier to hit the target

	// if target is directly infront
	// fire

	// things to modify for skill
	// extrapolate target
	// deviation, so they cant just whack u from miles away
	// 

}
