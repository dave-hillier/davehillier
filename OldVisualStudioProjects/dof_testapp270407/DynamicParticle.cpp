#include "DynamicParticle.h"
#include "Timer.h"

DynamicParticle::DynamicParticle(const vector3 &pos, const vector3 &vel, float mass) : 
	lastUpdate_(0), position_(pos), velocity_(vel), mass_(1), gravity_(0,-9.81f, 0)
{	
	Timer t;
	lastUpdate_ = t.getTime();
}

DynamicParticle::~DynamicParticle(void)
{
}

bool collide(const vector3 &pos)
{
	return pos.y < 0;
}
void DynamicParticle::update()
{
	Timer t;
	unsigned timeNow = t.getTime();
	float delta = (timeNow - lastUpdate_)/1000.f;
	delta = (delta < 0.1f) ? delta : 0.1f;

	// TODO drag
	vector3 acceleration = (gravity_ + forceAcculumator_) / mass_;
	velocity_ += acceleration * delta;
	vector3 new_position = position_ + velocity_ * delta;

	// TODO collide?
	if (collide(new_position))
	{
		vector3 normal(0,1.0f,0);
		float dp = -velocity_ % normal;
		vector3 reflected_vel = velocity_ + normal * 2 * dp;
		
		velocity_ = reflected_vel * 0.5f;
		new_position = position_ + velocity_ * delta;
	}

	position_ = new_position;
    lastUpdate_ = timeNow;
	forceAcculumator_ = vector3(0,0,0);
}

void DynamicParticle::applyForce(const vector3 &force)
{
	forceAcculumator_ += force;
}

