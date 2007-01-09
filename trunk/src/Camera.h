
#ifndef __CAMERAH__
#define __CAMERAH__

#include "Service.h"
#include "maths/matrix.h"
#include "maths/quaternion.h"

class Camera : public Service
{
public:

	Camera() : lastUpdate_(0), position_(0,0,0), pitch_(0), yaw_(0) {}
	virtual ~Camera() {}

	void initialise();
	void position(const vector3 &pos) { position_ = pos; }

	const vector3 &position() const { return position_; }
	const vector3 &forward() const { return forward_; }

	void translate(const vector3 &pos) { position_ += pos; }

	const matrix44 &modelView() const { return modelView_; }
	const quaternion &rotateInverse() const { return orientation_; }	// Why do I have to use the inverse?

	void update(double timeNow);

private:
	
	matrix44 modelView_;
	quaternion orientation_;
	double lastUpdate_;
	vector3 position_;
	vector3 forward_;
	float pitch_, yaw_;
};

#endif
