
#ifndef __CAMERAH__
#define __CAMERAH__

#include "Quat.h"
#include "Matrix.h"

// Position you pass camera is where the camera is in space.
//
// The matrix the camera generates will negate the position so it can be
// used in OpenGL for example.
class Camera
{
public:
	Camera();
	
	void RotateX(float amnt);
	void RotateY(float amnt);
	void RotateZ(float amnt);

	float RotateX() const;
	float RotateY() const;
	float RotateZ() const;

	void Translate(const Vector3 &trans); // Ie move the position
	void Position(const Vector3 &p)		{position_ = p; dirty_ = true;} // set the position
	const Vector3 &Position(void) const	{return position_;}
	
	void Direction(Vector3 &dir);
	
	float *ViewMatrix(void);
	void ViewMatrix(Matrix &m);
	
private:
	Vector3 rotation_;
	Vector3 position_;
	
protected:
	bool dirty_;
	Quat axis_[3];
	Matrix matrix_;
};
extern Camera camera;
#endif