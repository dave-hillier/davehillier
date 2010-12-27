
#include "Matrix.h"


/*
=============
Multiply
=============
this = lhs * rhs

 lhs or rhs can be equal to this since all results are stored in a temporary
 matrix.
*/
void Matrix::Multiply(Matrix &lhs, Matrix &rhs)
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
	Set(t);
}

/*
=============
Multiply
=============
Multiply a vector through a matrix.

 dest can be equal to source since a temp vector is used.
*/
void Matrix::Multiply(Vector &src, Vector &dest) 
{
	Vector t;
	
	t.x = m[0][0] * src.x + m[1][0] * src.y + m[2][0] * src.z + m[3][0];
	t.y = m[0][1] * src.x + m[1][1] * src.y + m[2][1] * src.z + m[3][1];
	t.z = m[0][2] * src.x + m[1][2] * src.y + m[2][2] * src.z + m[3][2];
	
	dest.Set(t);
}

// xyzw
void Matrix::Multiply(float src[4], float  dest[4]) 
{
	float x,y,z,w;
	
	x = m[0][0] * src[0] + m[1][0] * src[1] + m[2][0] * src[2] + m[3][0] * src[3];
	y = m[0][1] * src[0] + m[1][1] * src[1] + m[2][1] * src[2] + m[3][1] * src[3];
	z = m[0][2] * src[0] + m[1][2] * src[1] + m[2][2] * src[2] + m[3][2] * src[3];
	w = m[0][2] * src[0] + m[1][2] * src[1] + m[2][2] * src[2] + m[3][2] * src[3];
	
	dest[0] = x;
	dest[1] = y;
	dest[2] = z;
	dest[3] = w;	
}

/*
=============
Rotate
=============
Rotate this matrix a la OpenGL glRotate().  The formulas for this code were
taken from the OpenGL man pages.  

 The effect of this call is : this = this * rotation.
*/
void Matrix::Rotate(float angle, Vector &vec)
{
	Matrix ret;
	float c = (float)cos(angle);
	float s = (float)sin(angle);
	float mc = 1.0f - c;
	float x = vec.x;
	float y = vec.y;
	float z = vec.z;
	
	ret.m[0][0] = (x * x * mc) + c;
	ret.m[0][1] = (y * x * mc) + (z * s);
	ret.m[0][2] = (x * z * mc) - (y * s);
	
	ret.m[1][0] = (x * y * mc) - (z * s);
	ret.m[1][1] = (y * y * mc) + c;
	ret.m[1][2] = (y * z * mc) + (x * s);
	
	ret.m[2][0] = (x * z * mc) + (y * s);
	ret.m[2][1] = (y * z * mc) - (x * s);
	ret.m[2][2] = (z * z * mc) + c;
	
	Multiply(*this, ret);
}

/*
=============
Translate
=============
Translate the matrix.

 The effect of this call is : this = this * translation.
*/
void Matrix::Translate(Vector &vec)
{
	Matrix t;
	
	t.m[3][0] = vec.x;
	t.m[3][1] = vec.y;
	t.m[3][2] = vec.z;
	
	Multiply(*this, t);
}

/*
=============
Translate
=============
Translate the matrix.

 The effect of this call is : this = this * translation.
*/
void Matrix::Translate(float x, float y, float z)
{
	Matrix t;
	
	t.m[3][0] = x;
	t.m[3][1] = y;
	t.m[3][2] = z;
	
	Multiply(*this, t);
}

