/*
   Copyright (C) 2002 Nate Miller nathanm@uci.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
#include "Camera.h"
Camera camera;

// !!TODO put this in MathLib
static const Vector3 worldAxis[3] = 
{
   Vector3(1.0f, 0.0f, 0.0f),
   Vector3(0.0f, 1.0f, 0.0f),
   Vector3(0.0f, 0.0f, 1.0f),
};

Camera::Camera()
{
   position.Clear();
   rotation.Clear();
   dirty = 1;
}

void Camera::SetRotateX(const float &amnt)
{
   rotation[0] = amnt;
   rotation.Clamp(-360.0f, 360.0f);
  
   dirty = 1;
}

void Camera::SetRotateY(const float &amnt)
{
   rotation[1] = amnt;
   rotation.Clamp(-360.0f, 360.0f);
  
   dirty = 1;
}

void Camera::SetRotateZ(const float &amnt)
{
   rotation[2] = amnt;
   rotation.Clamp(-360.0f, 360.0f);
  
   dirty = 1;
}

void Camera::RotateX(const float &amnt)
{
   rotation[0] += amnt;
   rotation.Clamp(-360.0f, 360.0f);
  
   dirty = 1;
}

void Camera::RotateY(const float &amnt)
{
   rotation[1] += amnt;
   rotation.Clamp(-360.0f, 360.0f);

   dirty = 1;
}

void Camera::RotateZ(const float &amnt)
{
   rotation[2] += amnt;
   rotation.Clamp(-360.0f, 360.0f);

   dirty = 1;
}

void Camera::Translate(const Vector3 &trans)
{
   position.Add(trans);

   dirty = 1;
}

// When there is no rotation, direction is assumed to be (0 0 -1)
void Camera::GetDirection(Vector3 &dir)
{  
   if (!dirty)
   {
      dir[0] = -matrix[0][2];
      dir[1] = -matrix[1][2];
      dir[2] = -matrix[2][2];
   }
   else
   {
      Matrix m;
   
      GetMatrix(m);

      dir[0] = -m[0][2];
      dir[1] = -m[1][2];
      dir[2] = -m[2][2];
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

   tMat[3][0] = -position[0];
   tMat[3][1] = -position[1];
   tMat[3][2] = -position[2];

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

