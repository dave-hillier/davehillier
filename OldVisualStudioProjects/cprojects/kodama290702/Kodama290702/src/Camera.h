
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Quat.h"

// Position you pass camera is where the camera is in space.
//
// The matrix the camera generates will negate the position so it can be
// used in OpenGL for example.
struct Camera
{
	Camera();
	
	void SetRotateX(const float &amnt);
	void SetRotateY(const float &amnt);
	void SetRotateZ(const float &amnt);
	
	void RotateY(const float &amnt);
	void RotateX(const float &amnt);
	void RotateZ(const float &amnt);
	
	void Translate(const Vector &trans);
	void SetPosition(const Vector &p)   {position.Set(p); dirty = 1;}
	const Vector &GetPosition(void)     {return position;}
	
	
	void GetDirection(Vector &dir);// temp, just a test
	
	float *GetMatrix(void);
	void GetMatrix(Matrix &m);
	
	void Update();

	bool b_forward, b_back, b_right, b_left, b_up, b_down;
	bool b_rot_up,b_rot_down, b_rot_left,b_rot_right;

protected:
	Vector rotation;
	Vector position;

	bool dirty;
	Quat axis[3];
	Matrix matrix;
};
extern Camera *camera;

#endif