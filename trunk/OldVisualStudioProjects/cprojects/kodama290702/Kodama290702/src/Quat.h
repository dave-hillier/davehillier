
#ifndef __QUAT_H__
#define __QUAT_H__
#include "matrix.h"
#include "vector.h"


struct Quat
{
   Quat() {Identity();}

   void Identity(void);
   void Normalize(void);

   void Multiply(const Quat &lhs, const Quat &rhs);

   void ToMatrix(Matrix &mat);
   void ToVector(Vector &dest);

   void MatrixToQuat(Matrix &mat);
   void AxisAngleToQuat(const Vector &axis, float angle);
   void EulerToQuat(float x, float y, float z);
   
   void Slerp(float t, const Quat &from, const Quat &to);

protected:
   float x;
   float y;
   float z;
   float w;
};


#endif