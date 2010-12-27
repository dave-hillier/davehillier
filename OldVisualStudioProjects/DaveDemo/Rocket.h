// Rocket.h: interface for the Rocket class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __ROCKET__
#define __ROCKET__
#include "unit.h"
	
class Rocket : public Unit
{
public:
	Rocket(Vector3f pos, Vector3f dir);
	virtual ~Rocket();
	virtual void Update();
	virtual void Render();

	Unit *owner;

private:
//	float time_stamp;
	float life;
	
	int model, tex_id;

};

class HomingRocket : public Unit
{
public:
	HomingRocket(Vector3f pos, Vector3f dir);
	virtual ~HomingRocket();
	virtual void Update();
	virtual void Render();

	Unit *target, *owner;

private:
//	float time_stamp;
	float life;
	
	int model, tex_id;

};

#endif //__ROCKET__
