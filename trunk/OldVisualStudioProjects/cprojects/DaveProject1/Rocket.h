// Rocket.h: interface for the Rocket class.
//
//////////////////////////////////////////////////////////////////////

#include "unit.h"

class Rocket : public Unit
{
public:
	Rocket(Vector3 pos, Vector3 dir, Vector3 targ, Vector3 firer_vel);
	virtual ~Rocket();
	virtual void Update();
	virtual void Render();

	Vector3 target;

	float time_stamp;
	float life;
	
};
