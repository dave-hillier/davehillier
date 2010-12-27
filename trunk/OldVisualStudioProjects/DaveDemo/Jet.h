// Jet.h: interface for the Jet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __JET__
#define __JET__
#include "Unit.h"
#include "Ribbon.h"

//////////////////////////////////////////////////////////////////////
// interface for the PlayerJet class.
//
//////////////////////////////////////////////////////////////////////

class PlayerJet  
{
public:
	PlayerJet();
	virtual ~PlayerJet();

	void Update ();

	float power;			// Forward!
	float rudder;			// left right
	float pitch_control;	// up down	


	bool fire1, fire2;
};

//////////////////////////////////////////////////////////////////////
// interface for the AIJet class.
//
//////////////////////////////////////////////////////////////////////
class AIJet 
{
public:
	AIJet();
	virtual ~AIJet();

	void Update ();

	float power;			// Forward!
	float rudder;			// left right
	float pitch_control;	// up down

	bool fire1, fire2;
	Unit *parent;

	int state;
	float state_time;
};



class Jet : public Unit
{
public:
	Jet();
	virtual ~Jet();

	virtual void Render();
	virtual void Update();

	float power;			// Forward!
	float rudder;			// left right
	float pitch_control;	// up down

	PlayerJet *client;
	AIJet *ai;

	void Fire1();
	void Fire2();
	float fire_delay1;
	float fire_delay2;
	bool InFront2(Unit *target);
	bool Collision (Unit *unit);

private:
	int model1, model2, model3;
	int texture_id;
	float fan_rot, bounce_time;
	bool test;

	// firing etc
	void Restrict ();

	Ribbon ribbon1, ribbon2;
};

#endif // __JET__