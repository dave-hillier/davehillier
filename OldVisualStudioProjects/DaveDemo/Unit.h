// Unit.h: interface for the Unit class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __UNIT__
#define __UNIT__
#include "Mathlib.h"
using namespace platformMath;

class Unit  
{
public:
	Unit();
	virtual ~Unit();

	// management
	Unit *next, *prev;
	bool delete_me;
	Unit *Delete ();

	// do nothing
	virtual void Update() {};
	virtual void Render(); // draws bounds

	Vector3f org, vel, acc, grav, ang;
	float mass;
	Bounds bounds;

	char name[64];

	void AngleVectors (Vector3f *forward, Vector3f *right, Vector3f *up);

	bool TraceUnit(Vector3f pick_start, Vector3f pick_dir);
	bool Visible(Unit *target);
	bool InFront(Unit *target);

	float time_stamp;
	int health, damage;
};

class UManager  
{
public:
	UManager ();
	void Delete (Unit *p);
	void Render ();
	void Update ();
	Unit *uhead;
};

extern UManager uman;
extern float unit_scale;

Unit *GetPlayer ();
void SetPlayer (Unit *unit);

#endif // __UNIT__