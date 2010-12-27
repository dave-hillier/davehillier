#pragma once
#include "Updatable.h"
#include "maths/vector.h"

class DynamicParticle :
	public Updatable
{
public:
	DynamicParticle(const vector3 &pos, const vector3 &vel, float mass);
	virtual ~DynamicParticle(void);

    void update();
	const vector3 &position() const { return position_; }

	void applyForce(const vector3 &force);
	void setGravity(const vector3 &g) { gravity_ =g; }
private:
	vector3 position_;
	vector3 velocity_;
	vector3 forceAcculumator_;
	vector3 gravity_;
	unsigned lastUpdate_;
	float mass_;
};
