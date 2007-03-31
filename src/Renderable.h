#pragma once

class _vector3;
typedef _vector3 vector3;
class quaternion;

class Renderable
{
public:
	virtual ~Renderable() {}
	virtual const vector3& position() const = 0;
	virtual const quaternion& orientation() const = 0;
	virtual float radius() const = 0;
	virtual void render(double update) = 0;
};
