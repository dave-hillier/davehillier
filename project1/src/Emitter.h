#pragma once

#include "Renderable.h"

#include "Particle.h"
#include "maths/quaternion.h"
#include <vector>

class Emitter : public Renderable
{

public:
	Emitter();
	virtual ~Emitter();

	// renderable, used for culling
	virtual const vector3& position() const {return pos_; }
	virtual const quaternion& orientation() const { return o_;}
	virtual float radius() const{return 1;}

	void render(double timeNow);
	void removeDeadParticles(double timeNow);
	void createNewParticles(double timeNow);
	void update(double timeNow);


	std::vector<Particle> particles_;

	double rate_;
	double life_;
	double lastEmitter_;
	double lastUpdate_;
	float length_;
	float scale_;
	vector3 pos_;
	vector3 vel_;
	vector3 acc_;
	vector4 colourStart_;
	vector4 colourEnd_;
	quaternion o_;
	bool velocityAlign_;
	bool wireFrame_;

};