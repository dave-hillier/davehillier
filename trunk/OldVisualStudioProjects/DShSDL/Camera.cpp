
#include "Camera.h"
Camera camera;

static const Vector3 worldAxis[3] = 
{
	Vector3(1.0f, 0.0f, 0.0f),
	Vector3(0.0f, 1.0f, 0.0f),
	Vector3(0.0f, 0.0f, 1.0f),
};

Camera::Camera() : dirty_(true)
{

	
}

// TODO does rotation needs to be clamped?

// TODO eliminate rotation_ use axis_[0].s instead
void Camera::RotateX(float amnt)
{
	rotation_.x(amnt);
	dirty_ = true;
}

void Camera::RotateY(float amnt)
{
	rotation_.y(amnt);
	dirty_ = true;
}

void Camera::RotateZ(float amnt)
{
	rotation_.z(amnt);
	dirty_ = true;
}

void Camera::Translate(const Vector3 &trans)
{
	position_ += trans;
	dirty_ = true;
}
float Camera::RotateX() const { return rotation_.x(); }
float Camera::RotateY() const { return rotation_.y(); }
float Camera::RotateZ() const { return rotation_.z(); }

// When there is no rotation, direction is assumed to be (0 0 -1)
void Camera::Direction(Vector3 &dir)
{  
	if (dirty_)
	{
		Matrix m;
		ViewMatrix(m);
		dir(-m[0][2], -m[1][2], -m[2][2]);
	}
	else
	{
		dir(-matrix_[0][2], -matrix_[1][2], -matrix_[2][2]);
	}
}

float *Camera::ViewMatrix(void)
{
	if (!dirty_)
		return matrix_;

	(axis_[0]).set(worldAxis[0], -rotation_.x());
	(axis_[1]).set(worldAxis[1], -rotation_.y());
	(axis_[2]).set(worldAxis[2], -rotation_.z());

	Quat res;

	res = (axis_[1] * axis_[0]) * axis_[2];
	matrix_ = res.matrix();
	matrix_.translate( -position_ );

	dirty_ = false;

	return matrix_;
}

void Camera::ViewMatrix(Matrix &m)
{

	if (dirty_)
		ViewMatrix();

	m.set(matrix_);
}
