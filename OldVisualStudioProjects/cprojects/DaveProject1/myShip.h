// myShip.h: interface for the myShip class.
//
//////////////////////////////////////////////////////////////////////
// this is a template for the player
#include "Quat.h"
#include "Mathlib.h"
using namespace platformMath;

class myShip  
{
public:
	myShip();
	virtual ~myShip();

	void Update();
	void Fire();
	void Render();
	
	Vector3 org, vel, acc;

	void GetDirection (Vector3 &dir);
	
	float throttle;
	float throttle_inc;

	float engine_pitch;
	float engine_power;
	float roll, pitch, heading;
	float turning;

	float fire_delay;
	bool fired;

private:
	ms3d_model model1;
	ms3d_model model2;

	float mass;
	
/*	bool dirty;
	Quat axis[3];
	Vector3 rotation;
	Matrix matrix;*/
};

