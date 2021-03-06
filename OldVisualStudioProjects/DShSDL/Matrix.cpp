
#include "Matrix.h"



/*
=============
Multiply
=============
   this = lhs * rhs

   lhs or rhs can be equal to this since all results are stored in a temporary
matrix.
*/
void Matrix::multiply(const Matrix &lhs, const Matrix &rhs)
{
   Matrix t;

   t.m[0][0] = rhs.m[0][0] * lhs.m[0][0] + rhs.m[0][1] * lhs.m[1][0] + 
               rhs.m[0][2] * lhs.m[2][0] + rhs.m[0][3] * lhs.m[3][0];
   t.m[1][0] = rhs.m[1][0] * lhs.m[0][0] + rhs.m[1][1] * lhs.m[1][0] + 
               rhs.m[1][2] * lhs.m[2][0] + rhs.m[1][3] * lhs.m[3][0];
   t.m[2][0] = rhs.m[2][0] * lhs.m[0][0] + rhs.m[2][1] * lhs.m[1][0] + 
               rhs.m[2][2] * lhs.m[2][0] + rhs.m[2][3] * lhs.m[3][0];
   t.m[3][0] = rhs.m[3][0] * lhs.m[0][0] + rhs.m[3][1] * lhs.m[1][0] + 
               rhs.m[3][2] * lhs.m[2][0] + rhs.m[3][3] * lhs.m[3][0];
   t.m[0][1] = rhs.m[0][0] * lhs.m[0][1] + rhs.m[0][1] * lhs.m[1][1] + 
               rhs.m[0][2] * lhs.m[2][1] + rhs.m[0][3] * lhs.m[3][1];
   t.m[1][1] = rhs.m[1][0] * lhs.m[0][1] + rhs.m[1][1] * lhs.m[1][1] + 
               rhs.m[1][2] * lhs.m[2][1] + rhs.m[1][3] * lhs.m[3][1];
   t.m[2][1] = rhs.m[2][0] * lhs.m[0][1] + rhs.m[2][1] * lhs.m[1][1] + 
               rhs.m[2][2] * lhs.m[2][1] + rhs.m[2][3] * lhs.m[3][1];
   t.m[3][1] = rhs.m[3][0] * lhs.m[0][1] + rhs.m[3][1] * lhs.m[1][1] + 
               rhs.m[3][2] * lhs.m[2][1] + rhs.m[3][3] * lhs.m[3][1];
   t.m[0][2] = rhs.m[0][0] * lhs.m[0][2] + rhs.m[0][1] * lhs.m[1][2] + 
               rhs.m[0][2] * lhs.m[2][2] + rhs.m[0][3] * lhs.m[3][2];
   t.m[1][2] = rhs.m[1][0] * lhs.m[0][2] + rhs.m[1][1] * lhs.m[1][2] + 
               rhs.m[1][2] * lhs.m[2][2] + rhs.m[1][3] * lhs.m[3][2];
   t.m[2][2] = rhs.m[2][0] * lhs.m[0][2] + rhs.m[2][1] * lhs.m[1][2] + 
               rhs.m[2][2] * lhs.m[2][2] + rhs.m[2][3] * lhs.m[3][2];
   t.m[3][2] = rhs.m[3][0] * lhs.m[0][2] + rhs.m[3][1] * lhs.m[1][2] + 
               rhs.m[3][2] * lhs.m[2][2] + rhs.m[3][3] * lhs.m[3][2];
   t.m[0][3] = rhs.m[0][0] * lhs.m[0][3] + rhs.m[0][1] * lhs.m[1][3] + 
               rhs.m[0][2] * lhs.m[2][3] + rhs.m[0][3] * lhs.m[3][3];
   t.m[1][3] = rhs.m[1][0] * lhs.m[0][3] + rhs.m[1][1] * lhs.m[1][3] + 
               rhs.m[1][2] * lhs.m[2][3] + rhs.m[1][3] * lhs.m[3][3];
   t.m[2][3] = rhs.m[2][0] * lhs.m[0][3] + rhs.m[2][1] * lhs.m[1][3] + 
               rhs.m[2][2] * lhs.m[2][3] + rhs.m[2][3] * lhs.m[3][3];
   t.m[3][3] = rhs.m[3][0] * lhs.m[0][3] + rhs.m[3][1] * lhs.m[1][3] +
               rhs.m[3][2] * lhs.m[2][3] + rhs.m[3][3] * lhs.m[3][3];
   set(t);
}

/*
=============
Multiply
=============
   Multiply a vector through a matrix.
   
   dest can be equal to source since a temp vector is used.
*/
void Matrix::multiply(const Vector3 &src, Vector3 &dest) const
{
  dest(
	   (m[0][0] * src.x() + m[1][0] * src.y() + m[2][0] * src.z() + m[3][0]),
	   (m[0][1] * src.x() + m[1][1] * src.y() + m[2][1] * src.z() + m[3][1]),
	   (m[0][2] * src.x() + m[1][2] * src.y() + m[2][2] * src.z() + m[3][2]));

}

/*
=============
Rotate
=============
   Rotate this matrix a la OpenGL glRotate().  The formulas for this code were
taken from the OpenGL man pages.  
   
   The effect of this call is : this = this * rotation.
*/
void Matrix::rotate(float angle, const Vector3 &vec)
{
   Matrix ret;
   float c = cosf(angle);
   float s = sinf(angle);
   float mc = 1.0f - c;
   float x = vec.x();
   float y = vec.y();
   float z = vec.z();

   ret.m[0][0] = (x * x * mc) + c;
   ret.m[0][1] = (y * x * mc) + (z * s);
   ret.m[0][2] = (x * z * mc) - (y * s);

   ret.m[1][0] = (x * y * mc) - (z * s);
   ret.m[1][1] = (y * y * mc) + c;
   ret.m[1][2] = (y * z * mc) + (x * s);

   ret.m[2][0] = (x * z * mc) + (y * s);
   ret.m[2][1] = (y * z * mc) - (x * s);
   ret.m[2][2] = (z * z * mc) + c;

   multiply(*this, ret);
}

/*
=============
Translate
=============
   Translate the matrix.
   
   The effect of this call is : this = this * translation.
*/
void Matrix::translate(const Vector3 &vec)
{
   Matrix t;

   t.m[3][0] = vec.x();
   t.m[3][1] = vec.y();
   t.m[3][2] = vec.z();
   
   multiply(*this, t);
}

/*
=============
Translate
=============
   Translate the matrix.
   
   The effect of this call is : this = this * translation.
*/
void Matrix::translate(float x, float y, float z)
{
   Matrix t;

   t.m[3][0] = x;
   t.m[3][1] = y;
   t.m[3][2] = z;
   
   multiply(*this, t);
}

