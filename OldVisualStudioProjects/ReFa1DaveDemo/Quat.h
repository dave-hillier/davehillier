
#ifndef __QUATH__
#define __QUATH__

#include "Mathlib.h"

class Matrix;
class Vector3f;

struct Quat
{
   Quat() {Identity();}

   void Identity(void);
   void Normalize(void);

   void Multiply(const Quat &lhs, const Quat &rhs);

   void ToMatrix(Matrix &mat);
   void ToVector(Vector3f &dest);

   void MatrixToQuat(Matrix &mat);
   void AxisAngleToQuat(const Vector3f &axis, float angle);
   void EulerToQuat(float x, float y, float z);
   
   void Slerp(float t, const Quat &from, const Quat &to);

protected:
   float x;
   float y;
   float z;
   float w;
};


#endif