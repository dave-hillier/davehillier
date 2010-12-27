// ParticleManager.h: interface for the ParticleManager class.
//
//////////////////////////////////////////////////////////////////////
#include "mathlib.h"
using namespace platformMath;


class Particle 
{
public:
	Particle();
	virtual ~Particle();
	void Render ();
	void Update ();

	Particle *next, *prev;

	float time_stamp;
	float life;

	float size_start;
	float size_end;

	float colour_start[4];
	float colour_end[4];
	
	int texid;

	Vector3 vel, acc, org; 

	// update?
	// delete?
	// collide
	// (de)constructor?
//private:
	float size;
	float colour[4];

};

class ParticleManager  
{
public:
	ParticleManager();
	virtual ~ParticleManager();
	void Render ();
	void Update ();
	void InitSystems();
	void TestSys(Vector3 pos);
	void Smoke(Vector3 pos);
private:
protected:
};

extern ParticleManager pman;
