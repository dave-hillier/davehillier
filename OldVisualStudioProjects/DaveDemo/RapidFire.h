// RapidFire.h: interface for the RapidFire class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __RAPIDFIRE__
#define __RAPIDFIRE__
#include "unit.h"
#include "ribbon.h"
	
class RapidFire : public Unit
{
public:
	RapidFire(Vector3f pos, Vector3f dir, int dam =5);
	virtual ~RapidFire();
	virtual void Update();
	virtual void Render();

//	float time_stamp;
	float life;

	Unit *owner;

	Ribbon vrib, hrib;
	
};

#endif //__RAPIDFIRE__
