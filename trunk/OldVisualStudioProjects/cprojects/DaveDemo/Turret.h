// Turret.h: interface for the Turret class.
//
//////////////////////////////////////////////////////////////////////
#include "unit.h"

class Turret : public Unit 
{
public:
	Turret(int t);
	virtual ~Turret();
	void Update ();
	void Render ();
	void GunVector (Vector3f *forward);

	Vector3f gun_ang;
	int type;
	int barrel;

};
