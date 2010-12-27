// Turret.h: interface for the Turret class.
//
//////////////////////////////////////////////////////////////////////
#include "new_src/Actor.h"

class Turret : public Actor 
{
public:
	Turret(int t);
	virtual ~Turret();
	void Update ();
	void Render ();
	void GunVector (Vector3f *forward);

	Vector3f gun_ang;
	unsigned int type;
	unsigned int barrel;

};
