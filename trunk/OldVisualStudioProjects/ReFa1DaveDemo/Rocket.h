// Rocket.h: interface for the Rocket class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __ROCKET__
#define __ROCKET__
#include "new_src/Actor.h"
	
class Rocket : public Actor
{
public:
	Rocket(Vector3f pos, Vector3f dir);
	virtual ~Rocket();
	virtual void Update();
	virtual void Render();

	Actor *owner;

private:
//	float time_stamp;
	float life;
	
	int model, tex_id;

};

class HomingRocket : public Actor
{
public:
	HomingRocket(Vector3f pos, Vector3f dir);
	virtual ~HomingRocket();
	virtual void Update();
	virtual void Render();

	Actor *target, *owner;

private:
//	float time_stamp;
	float life;
	
	int model, tex_id;

};

#endif //__ROCKET__
