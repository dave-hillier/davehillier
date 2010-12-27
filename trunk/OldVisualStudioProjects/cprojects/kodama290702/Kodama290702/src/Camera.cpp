//#include "terrain.h"
#include "SDLTimer.h"
#include "glmain.h"
#include "Camera.h"
Camera *camera;

// This module contains a bit of redundancy at the moment
// but some of the stuff may come in useful later
static const Vector worldAxis[3] = 
{
	Vector(1.0f, 0.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f),
		Vector(0.0f, 0.0f, 1.0f),
};

Camera::Camera()
{
	position.Clear();//
	rotation.Clear();
	dirty = 1;
	
	b_forward = b_back = false;
	b_right = b_left = false;
	b_up = b_down = false;
	b_rot_up = b_rot_down = false;
	b_rot_left = b_rot_right = false;
}

void Camera::SetRotateX(const float &amnt)
{
	rotation.x = amnt;
	rotation.Clamp(-360.0f, 360.0f);
	
	dirty = 1;
}

void Camera::SetRotateY(const float &amnt)
{
	rotation.y = amnt;
	rotation.Clamp(-360.0f, 360.0f);
	
	dirty = 1;
}

void Camera::SetRotateZ(const float &amnt)
{
	rotation.z = amnt;
	rotation.Clamp(-360.0f, 360.0f);
	
	dirty = 1;
}

void Camera::RotateX(const float &amnt)
{
	rotation.x += amnt;
	rotation.Clamp(-360.0f, 360.0f);
	
	dirty = 1;
}

void Camera::RotateY(const float &amnt)
{
	rotation.y += amnt;
	rotation.Clamp(-360.0f, 360.0f);
	
	dirty = 1;
}

void Camera::RotateZ(const float &amnt)
{
	rotation.z += amnt;
	rotation.Clamp(-360.0f, 360.0f);
	
	dirty = 1;
}

void Camera::Translate(const Vector &trans)
{
	position.Add(trans);
	
	dirty = 1;
}

// When there is no rotation, direction is assumed to be (0 0 -1)
void Camera::GetDirection(Vector &dir)
{  
	if (!dirty)
	{
		dir.x = -matrix[0][2];
		dir.y = -matrix[1][2];
		dir.z = -matrix[2][2];
	}
	else
	{
		Matrix m;
		
		GetMatrix(m);
		
		dir.x = -m[0][2];
		dir.y = -m[1][2];
		dir.z = -m[2][2];
	}
}

float *Camera::GetMatrix(void)
{
	if (!dirty)
		return matrix;
	
	axis[0].Identity();
	axis[0].AxisAngleToQuat(worldAxis[0], -rotation[0]);
	
	axis[1].Identity();
	axis[1].AxisAngleToQuat(worldAxis[1], -rotation[1]);
	
	axis[2].Identity();
	axis[2].AxisAngleToQuat(worldAxis[2], -rotation[2]);
	
	Quat res;
	
	res.Multiply(axis[1], axis[0]);
	res.Multiply(res, axis[2]);
	res.ToMatrix(matrix);
	
	Matrix tMat;
	
	tMat[3][0] = -position.x;
	tMat[3][1] = -position.y;
	tMat[3][2] = -position.z;
	
	matrix.Multiply(matrix, tMat);
	
	dirty = 0;
	
	return matrix;
}

void Camera::GetMatrix(Matrix &m)
{
	if (dirty)
		GetMatrix();
	
	m.Set(matrix);
}

void Camera::Update ()
{

	Vector f, r;
//	GetDirection(f);
	f = glapp->modelView.In();
	r = glapp->modelView.Right();
	
	position -= f * (100.0f * timer.Delta() * b_forward); 
	position += f * (100.0f * timer.Delta() * b_back); 
	
	position += r * (100.0f * timer.Delta() * b_right); 
	position -= r * (100.0f * timer.Delta() * b_left); 

	if (position.x < 257)
		position.x += 256;
	else if (position.x > 513)
		position.x -= 256;

	if (position.z < 257)
		position.z += 256;
	else if (position.z > 513)
		position.z -= 256;

	rotation.x += 1.0f * timer.Delta() * b_rot_up;
	rotation.x -= 1.0f * timer.Delta() * b_rot_down;

	rotation.y -= 1.0f * timer.Delta() * b_rot_right;
	rotation.y += 1.0f * timer.Delta() * b_rot_left;

	if (b_forward || b_back || b_left || b_right || 
		b_rot_up || b_rot_down || b_rot_left || b_rot_right)
		dirty = 1;
}


void CmdLookLeftDown(int argc, char *argv[]) { camera->b_rot_left = 1; }
void CmdLookLeftUp(int argc, char *argv[]) { camera->b_rot_left = 0; }
void CmdLookRightDown(int argc, char *argv[]) { camera->b_rot_right = 1; }
void CmdLookRightUp(int argc, char *argv[]) { camera->b_rot_right = 0; }
void CmdLookUpDown(int argc, char *argv[]) { camera->b_rot_up = 1; }
void CmdLookUpUp(int argc, char *argv[]) { camera->b_rot_up = 0; }
void CmdLookDownDown(int argc, char *argv[]) {camera->b_rot_down = 1; }
void CmdLookDownUp(int argc, char *argv[]) { camera->b_rot_down = 0; }

void CmdMoveForwardDown(int argc, char *argv[]) { camera->b_forward = 1; }
void CmdMoveForwardUp(int argc, char *argv[]) { camera->b_forward = 0; }
void CmdMoveBackDown(int argc, char *argv[]) { camera->b_back = 1; }
void CmdMoveBackUp(int argc, char *argv[]) { camera->b_back = 0; }

void CmdMoveRightDown(int argc, char *argv[]) { camera->b_right = 1; }
void CmdMoveRightUp(int argc, char *argv[]) { camera->b_right = 0; }
void CmdMoveLeftDown(int argc, char *argv[]) { camera->b_left = 1; }
void CmdMoveLeftUp(int argc, char *argv[]) { camera->b_left = 0; }
