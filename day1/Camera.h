
#ifndef __CAMERAH__
#define __CAMERAH__


#include "maths/matrix.h"
#include "maths/quaternion.h"

class Joystick;

class Camera
{
public:

	Camera() : lastUpdate_(0), position_(0,0,0), forward_(0,0,-1), pitch_(0), yaw_(0) {}
	virtual ~Camera() {}

	void position(const vector3 &pos) { position_ = pos; }

	const vector3 &position() const { return position_; }
	const vector3 &forward() const { return forward_; }

	void translate(const vector3 &pos) { position_ += pos; }
	void adjustPitch(float p) { pitch_ += p; }
	void adjustYaw(float y) { yaw_ += y; }

	const matrix44 &modelView() const { return modelView_; }
	const quaternion &rotateInverse() const { return orientation_; }	// Why do I have to use the inverse?

	virtual void update();

protected:
	
	matrix44 modelView_;
	quaternion orientation_;
	unsigned lastUpdate_;
	vector3 position_;
	vector3 forward_;
	float pitch_, yaw_;

};

#endif
