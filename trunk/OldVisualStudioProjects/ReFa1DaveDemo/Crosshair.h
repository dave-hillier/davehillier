// Crosshair.h: interface for the Crosshair class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __CROSSHAIR__
#define __CROSSHAIR__

#include "Mathlib.h"

class Crosshair  
{
public:
	Crosshair();
	virtual ~Crosshair();
	void Display();
	void Update();
	float colour[4];
	bool TargetLock (Vector3f pos);
	void Ray (Vector3f &v);
	Actor *target;
	Vector3f dir;
private:
	float x, y;
	int size;
//	int range;
};
extern Crosshair xhair;

#endif // __CROSSHAIR__