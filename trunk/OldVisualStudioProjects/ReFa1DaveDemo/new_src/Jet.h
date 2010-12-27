#pragma once

#include "Actor.h"
#include "../Ribbon.h"
#include "../Vector3.h"

class Jet;

class JetController
{
public:
	virtual ~JetController() {}
	virtual void die(Jet *jet) = 0;
	virtual void update(Jet *jet) = 0;
	virtual void getAimDirection(const Jet *jet, Vector3f *dir) const = 0;

	virtual float power() const = 0;
	virtual float rudder() const = 0;
	virtual float pitchControl() const = 0;
	virtual bool fire1Enabled() const = 0;
	virtual bool fire2Enabled() const = 0;
};

class Jet : public Actor
{
public:
	Jet(JetController *c);
	virtual ~Jet();

	virtual void Render();
	virtual void Update();

	void Fire1();
	void Fire2();
	float fire_delay1;
	float fire_delay2;
	bool InFront2(Actor *target);
	bool Collision (Actor *unit);

	// TODO restrict these, encapsultate
	//float power;			// Forward!
	//float rudder;			// left right
	//float pitch_control;	// up down
	//bool fire1, fire2;

	float rudder() const { return controller->rudder(); }
	float pitchControl() const{ return controller->pitchControl(); }
	bool fire1Enabled() const{ return controller->fire1Enabled(); }		
	bool fire2Enabled() const{ return controller->fire1Enabled(); }		
private:
	int model1, model2, model3;
	unsigned int texture_id;
	float fan_rot, bounce_time;
	float power;

	// firing etc
	void Restrict ();

	Ribbon ribbon1, ribbon2;


	JetController *controller;
};