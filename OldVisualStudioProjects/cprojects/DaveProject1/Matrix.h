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
#ifndef __MATRIXH__
#define __MATRIXH__

/*
   WORKLOG
      (08/03/01)
         - Trashed the old matrix class because it sucked.
   TODO
      - make this class usable.
*/
#include "Mathlib.h"
#include <memory.h>

namespace platformMath {

// column row
// 0  4  8  12
// 1  5  9  13
// 2  6  10 14
// 3  7  11 15
//
// right -> (0, 4, 8)
// up    -> (1, 5, 9)
// in    -> (2, 6, 10)

struct Matrix
{
   Matrix() {Identity();}

   float *Get(void)                  {return mArray;}
   float Get(int col, int row) const {return m[col][row];}

   inline void Set(const Matrix &mat);
   inline void Identity(void);
   inline void Transpose(Matrix &dest) const;
   inline void Add(const Matrix &src);  
   inline void Subtract(const Matrix &src);

   void Multiply(const Matrix &lhs, const Matrix &rhs);
   void Multiply(const Vector3 &src, Vector3 &dest) const;

   void Rotate(float angle, const Vector3 &vec);
   void Translate(const Vector3 &vec);
   void Translate(float x, float y, float z);

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
inline void Matrix::Set(const Matrix &mat)
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
inline void Matrix::Transpose(Matrix &dest) const
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
inline void Matrix::Add(const Matrix &src)
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
inline void Matrix::Subtract(const Matrix &src)
{
   for (int i = 0; i < 16; ++i)
      mArray[i] -= src.mArray[i];
}

}

#endif