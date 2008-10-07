#pragma once

#include "maths/vector.h"

class Particle {
public:
	Particle(const vector3 &p, const vector3 &v, double t) : pos_(p), vel_(v), timeStamp_(t) {}

	void update(double delta, const vector3& acc);
	void renderVelocityAligned(const vector3& eye, float length, float scale, bool wireFrame) const;
	void renderAxisAligned(const vector3& x, const vector3& y, float scale, bool wireFrame) const;
	void setColour(double timeNow, float life, const vector4& colourStart, const vector4& colourEnd) const;

	double timeStamp_;
	vector3 pos_;
	vector3 vel_;

};

