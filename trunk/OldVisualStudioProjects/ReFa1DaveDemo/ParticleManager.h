// ParticleManager.h: interface for the ParticleManager class.
//
//////////////////////////////////////////////////////////////////////
#include "mathlib.h"
#include "Vector3.h"

#include <vector>

class Particle 
{
public:
	Particle();
	~Particle();
	void Render ();
	void Update ();

	float time_stamp;
	float life;

	float size_start;
	float size_end;

	float colour_start[4];
	float colour_end[4];
	
	unsigned int texid;

	Vector3f vel, acc, org; 

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

	void TestSys(Vector3f pos);
	void Smoke(Vector3f pos);
	void Explosion(Vector3f pos);
	void Trail(Vector3f pos);
	void Zap(Vector3f pos);
	void Explosion2(Vector3f pos);
	void Dirt(Vector3f pos);
	void Smoke2(Vector3f pos);
private:
protected:
	std::vector<Particle> parts;
};

extern ParticleManager pman;
