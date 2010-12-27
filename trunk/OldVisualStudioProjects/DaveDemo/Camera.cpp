#include "terrain.h"
#include "unit.h"
#include "Camera.h"
Camera camera;

static const Vector3f worldAxis[3] = 
{
   Vector3f(1.0f, 0.0f, 0.0f),
   Vector3f(0.0f, 1.0f, 0.0f),
   Vector3f(0.0f, 0.0f, 1.0f),
};

Camera::Camera()
{
   position.Clear();
   rotation.Clear();
   dirty = 1;
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

void Camera::Translate(const Vector3f &trans)
{
   position.Add(trans);

   dirty = 1;
}

// When there is no rotation, direction is assumed to be (0 0 -1)
void Camera::GetDirection(Vector3f &dir)
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

void Camera::Chase (Vector3f pt, Vector3f dir, Vector3f up, Vector3f ang)
{
	Vector3f v1,v2, pos;
	// chase cam
	SetRotateX(ang.x);
	SetRotateZ(0);
	SetRotateY(ang.y);

	v1 =  dir * -4.f + up*0.30f;
	v1.Normalise();

	pos = pt + v1*400*unit_scale;

	float h = terrain.GetHeight(pos.x, pos.z) + 50*unit_scale;
	if (h > pos.y)
		pos.y = h;

	SetPosition(pos);

}