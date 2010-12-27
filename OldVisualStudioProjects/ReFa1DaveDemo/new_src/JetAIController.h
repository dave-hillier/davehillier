#pragma  once

#include "Jet.h"

class JetAIController : public JetController
{
public:

	void die(Jet *jet)
	{
	}

	void getAimDirection(const Jet *jet, Vector3f *dir) const 
	{	
		AngleVectors(jet->ang, dir, NULL, NULL);
	}

	void update(Jet *jet)
	{
		if ( jet->health > 0 )
		{
			// TODO Search for other jets and avoid them
			// TODO From AI Jet
			Actor *unit, *targ = NULL;
			Vector3f f, r, u;
			AngleVectors(jet->ang, &f, &r, &u);

			unit = Environment::instance().player();
			if (jet->Visible(unit) && (jet->health > 0))
				targ = unit;

			if (targ)
			{
				Vector3f vec = targ->org - jet->org;


				// fix turning across 360
				float desired_y = (float)atan2(vec.x, vec.z);
				desired_y = (float)Rad2Deg(desired_y);

				desired_y += 180;
				if (desired_y < 0)
					desired_y += 360;
				else if (desired_y > 360)
					desired_y -= 360;

				if (desired_y != jet->ang.y)
					rudder_control = desired_y - jet->ang.y; 

				if (rudder_control > 180)
					rudder_control -= 360;

				rudder_control *= 2; // prob depend on difficulty

				float vecy = vec.y;
				vec.y = 0;
				float xz = vec.Length(); 
				float desired_x = (float)Rad2Deg(atan2(vecy, xz));

			//	con->Printf("desired pitch = %.2f", desired_x);
				if (desired_x != jet->ang.x)
					pitch_control = desired_x - jet->ang.x;

				if (pitch_control > 180)
					pitch_control -= 360;

				pitch_control *= 2;

				vec.y = vecy;
	
				float range = vec.Length();
				if (range > 300*50*Environment::instance().scale())
					power_control = 1;
				else if (range > 100*50*Environment::instance().scale())
					power_control = 1.25;
				else
					power_control = 0.5f;

				fire1 = false;
				fire2 = false;

				if (jet->InFront2(targ))
				{		
					// maybe fire a missile
					fire2 = true;

					// check for obstacles
					float l1, l2;
					l2 = vec.Length();
					vec.Normalise();

					for (std::list<Actor*>::iterator itt = Environment::instance().units.begin(); itt != Environment::instance().units.begin(); ++itt)
					{
						Actor *u = *itt;
						if ((u != targ) && (u != jet) && (u->TraceActor(jet->org, vec)))
						{
							l1 = (u->org - jet->org).Length();
							if (l1 < l2)
								fire2 = false;

						}
					}
				}
			}

		}

	}
	virtual float power() const { return fabsf(power_control) < 0.1; }
	virtual float rudder() const { return rudder_control; }
	virtual float pitchControl() const { return pitch_control; }
	virtual bool fire1Enabled() const { return fire1; }
	virtual bool fire2Enabled() const { return fire2; }
private:
	float power_control;
	float rudder_control;
	float pitch_control;
	bool fire1, fire2;

};