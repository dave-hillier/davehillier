
#ifndef __CAMERAH__
#define __CAMERAH__

#include "Quat.h"
#include "Mathlib.h"
#include "Vector3.h"
#include "Matrix.h"



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
	
	void Translate(const Vector3f &trans);
	void SetPosition(const Vector3f &p)   {position.Set(p); dirty = 1;}
	const Vector3f &GetPosition(void)     {return position;}
	
	// TODO temp, just a test
	void GetDirection(Vector3f &dir);
	
	float *GetMatrix(void);
	void GetMatrix(Matrix &m);
	void Chase (Vector3f pt, Vector3f dir, Vector3f up, Vector3f ang);
	
	Vector3f rotation;
	Vector3f position;
	
protected:
	bool dirty;
	Quat axis[3];
	Matrix matrix;
};
extern Camera camera;
#endif