#pragma once

#include "Jet.h"
#include "DebugLog.h"
#include "../Overlay.h"
#include "../Timer.h"
#include "../Camera.h"
#include "../Crosshair.h"
#include "Joystick.h"


Joystick joy; // TODO singleton to manage the connection of the joysticks

class JetPlayerController : public JetController
{
public:
	JetPlayerController() : rudder_control(0), pitch_control(0), fire1(false), fire2(false)
	{

	}

	void die(Jet *jet)
	{
		for (std::list<Actor*>::iterator itt = Environment::instance().units.begin(); itt != Environment::instance().units.begin(); ++itt)
		{
			Actor *un = *itt;
			if (un->name == "Start")
			{
				// TODO 
				// Reset the position to be that of start
				return;
			}
		}
	}

	void update(Jet *jet)
	{
		overlay.health = jet->health/100.0f;
		overlay.vel = jet->vel.Length()/3500.0f;

		joy.update(0);
		if ( jet->health > 0 )
		{
			// TODO Client update
			rudder_control -= joy.axis(0) * 30 * 0.25f;

			if (rudder_control > 75)
				rudder_control = 75;
			else if (rudder_control < -75)
				rudder_control = -75;

			if (rudder_control > 1)
				rudder_control -= 20 * timer.Delta();
			else if (rudder_control < -1)
				rudder_control += 20 * timer.Delta();

			pitch_control += joy.axis(1) * 30 * 0.5f;

			if (pitch_control > 1)
				pitch_control -= 30 * timer.Delta();
			else if (pitch_control < -1)
				pitch_control += 30 * timer.Delta();


			// try get a target
			fire1 = joy.button(0);
			fire2 = joy.button(1);

			//if (input.State('C'))
			//{
			//	jet->acc.Clear();
			//	jet->vel.Clear();
			//	jet->power = 0;
			//}
		}

		Vector3f forward, up;
		AngleVectors(jet->ang, &forward, NULL, &up);
		camera.Chase(jet->org, forward, up, jet->ang);

		//debugLog << "ang" << jet->ang.x << " " << jet->ang.y  << " " << jet->ang.z << std::endl;
	}
	void getAimDirection(const Jet *jet, Vector3f *dir) const 
	{	
		*dir = xhair.dir;
	}

	virtual float power() const { return true ? 1.f : 0.f; }
	virtual float rudder() const { return rudder_control; }
	virtual float pitchControl() const { return pitch_control; }
	virtual bool fire1Enabled() const { return fire1; }
	virtual bool fire2Enabled() const { return fire2; }
private:
	float rudder_control;
	float pitch_control;
	bool fire1, fire2;

};