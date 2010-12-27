#ifndef __MATRIX_H__
#define __MATRIX_H__
//---------------------------------------------------------------------------
#include "vector.h"
//---------------------------------------------------------------------------

// column row
// 0  4  8  12
// 1  5  9  13
// 2  6  10 14
// 3  7  11 15


// is this correct? have i got it the wrong way around?
// right -> (0, 4, 8)
// up    -> (1, 5, 9)
// in    -> (2, 6, 10)

struct Matrix
{
	Matrix() {Identity();}
	
	float *Get(void)                  {return mArray;}
	float Get(int col, int row) {return m[col][row];}
	
	inline void Set(Matrix &mat);
	inline void Identity(void);
	inline void Transpose(Matrix &dest);
	inline void Add(Matrix &src);  
	inline void Subtract(Matrix &src);
	
	void Multiply(Matrix &lhs, Matrix &rhs);
	void Multiply(Vector &src, Vector &dest);
	void Multiply(float src[4], float  dest[4]);

	void Rotate(float angle, Vector &vec);
	void Translate(Vector &vec);
	void Translate(float x, float y, float z);
	
	inline Vector Right() { return Vector(mArray[0], mArray[4], mArray[8]); };
	inline Vector Up() { return Vector(mArray[1], mArray[5], mArray[9]); };
	inline Vector In() { return Vector(mArray[2], mArray[6], mArray[10]); };
	
	operator float*(void)      {return mArray;}
	float *operator[](int ndx) {return m[ndx];}
	
	
	
protected:
	union
	{
		float m[4][4];
		float mArray[16];
	};
	
};

/*
=============
Set
=============
Set the matrix to the passed matrix.
*/
inline void Matrix::Set(Matrix &mat)
{
	m[0][0] = mat.m[0][0];
	m[0][1] = mat.m[0][1];
	m[0][2] = mat.m[0][2];
	m[0][3] = mat.m[0][3];
	
	m[1][0] = mat.m[1][0];
	m[1][1] = mat.m[1][1];
	m[1][2] = mat.m[1][2];
	m[1][3] = mat.m[1][3];
	
	m[2][0] = mat.m[2][0];
	m[2][1] = mat.m[2][1];
	m[2][2] = mat.m[2][2];
	m[2][3] = mat.m[2][3];
	
	m[3][0] = mat.m[3][0];
	m[3][1] = mat.m[3][1];
	m[3][2] = mat.m[3][2];
	m[3][3] = mat.m[3][3];
}

/*
=============
Identity
=============
Set the matrix to identity.
*/
inline void Matrix::Identity(void)
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;
	
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

/*
=============
Transpose
=============
Put the transpose of the matrix into dest.
*/
inline void Matrix::Transpose(Matrix &dest)
{
	Matrix temp;
	
	temp.m[0][0] = m[0][0];
	temp.m[0][1] = m[1][0];
	temp.m[0][2] = m[2][0];
	temp.m[0][3] = m[3][0];
	temp.m[1][0] = m[0][1];
	temp.m[1][1] = m[1][1];
	temp.m[1][2] = m[2][1];
	temp.m[1][3] = m[3][1];
	temp.m[2][0] = m[0][2];
	temp.m[2][1] = m[1][2];
	temp.m[2][2] = m[2][2];
	temp.m[2][3] = m[3][2];
	temp.m[3][0] = m[0][3];
	temp.m[3][1] = m[1][3];
	temp.m[3][2] = m[2][3];
	temp.m[3][3] = m[3][3];
	dest.Set(temp);
}

/*
=============
Add
=============
Add a matrix to this matrix.
*/
inline void Matrix::Add(Matrix &src)
{
	for (int i = 0; i < 16; ++i)
		mArray[i] += src.mArray[i];
}

/*
=============
Subtract
=============
Subtract a matrix from this matrix.
*/
inline void Matrix::Subtract(Matrix &src)
{
	for (int i = 0; i < 16; ++i)
		mArray[i] -= src.mArray[i];
}


//---------------------------------------------------------------------------
#endif	//__MATRIX_H__