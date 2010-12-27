// Unit.h: interface for the Unit class.
//
//////////////////////////////////////////////////////////////////////
#include "Mathlib.h"
using namespace platformMath;

class Unit  
{
public:
	Unit();
	virtual ~Unit();
	virtual void Update() {};
	virtual void Render() {};
	Vector3 org, vel, acc, grav;
	float mass;

	Unit *next, *prev;
};

class UManager  
{
public:
	void Delete (Unit *p);
	void Render ();
	void Update ();
};

extern UManager uman;
