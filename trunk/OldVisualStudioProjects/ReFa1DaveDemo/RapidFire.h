// RapidFire.h: interface for the RapidFire class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __RAPIDFIRE__
#define __RAPIDFIRE__
#include "new_src/Actor.h"
#include "ribbon.h"
	
class RapidFire : public Actor
{
public:
	RapidFire(Vector3f pos, Vector3f dir, int dam =5);
	virtual ~RapidFire();
	virtual void Update();
	virtual void Render();

//	float time_stamp;
	float life;

	Actor *owner;

	Ribbon vrib, hrib;
	
};

#endif //__RAPIDFIRE__
