
#ifndef _Matrix_h_
#define _Matrix_h_

#include <memory.h>

#include "Vector3.h"

// column row
// 0  4  8  12
// 1  5  9  13
// 2  6  10 14
// 3  7  11 15
//
// right -> (0, 4, 8)
// up    -> (1, 5, 9)
// in    -> (2, 6, 10)

class Matrix
{
public:
   Matrix() {identity();}

   float *get(void)                  {return mArray;}
   float get(int col, int row) const {return m[col][row];}

   // TODO operators for the basic ops (ie add sub mul)
   inline void set(const Matrix &mat);
   inline void identity(void);
   inline void transpose(Matrix &dest) const;
   inline void add(const Matrix &src);  
   inline void subtract(const Matrix &src);

   void multiply(const Matrix &lhs, const Matrix &rhs);
   void multiply(const Vector3 &src, Vector3 &dest) const;

   void rotate(float angle, const Vector3 &vec);
   void translate(const Vector3 &vec);
   void translate(float x, float y, float z);

   operator float*(void)      {return mArray;}
   float *operator[](int ndx) {return m[ndx];}

   operator const float*(void)  const    {return mArray;}
   const float *operator[](int ndx) const {return m[ndx];}

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
inline void Matrix::set(const Matrix &mat)
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
inline void Matrix::identity(void)
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
inline void Matrix::transpose(Matrix &dest) const
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
   dest.set(temp);
}

/*
=============
Add
=============
   Add a matrix to this matrix.
*/
inline void Matrix::add(const Matrix &src)
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
inline void Matrix::subtract(const Matrix &src)
{
   for (int i = 0; i < 16; ++i)
      mArray[i] -= src.mArray[i];
}


#endif // _Matrix_h_