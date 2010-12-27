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
/*
   WORKLOG
      (10/06/01)
         - decided to make the single data member public
         - cleaned up the formatting
      (10/02/00)  
         - took out the vec4_t, i never use it
      (09/27/00)
         - misc cleaning
      (09/26/00)
         - started to add non-vec3_t returning stuff for + - / * etc
      (09/09/00)
         - added invert, need to clean up this code
      (08/11/00)
         - added Set(vec3_t &p)
      (07/15/00)
         - now return ref on += -= *= etc, ok?
   TODO
      - clean up
*/

#ifndef __VECTOR3H__
#define __VECTOR3H__

namespace platformMath {

// Anything that returns a Vector shouldn't be used when you need speed
struct Vector3
{
   // constructors
   inline Vector3()
   {
      v[0] = v[1] = v[2] = 0;
   }

   inline Vector3(float px, float py, float pz)
   {
      v[0] = px; 
      v[1] = py; 
      v[2] = pz;
   }

   inline Vector3(const Vector3 &pVec)
   {
      v[0] = pVec.v[0];
      v[1] = pVec.v[1];
      v[2] = pVec.v[2];
   }

   inline Vector3(const float *pVec)
   {
      v[0] = pVec[0];
      v[1] = pVec[1];
      v[2] = pVec[2];
   }

   // assignment
   inline Vector3 operator=(const Vector3 &pVec)
   {
      return Vector3(v[0] = pVec.v[0], v[1] = pVec.v[1], v[2] = pVec.v[2]);
   }

   inline Vector3 operator=(const float *ptr)
   {
      return Vector3(v[0] = ptr[0], v[1] = ptr[1], v[2] = ptr[2]);
   }

   // indexing into the array, no bound checks
   inline const float &operator[](int ndx) const
   {
      return v[ndx];
   }

   inline float &operator[](int ndx)
   {
      return v[ndx];
   }
   
   inline operator float*(void)
   {
      return v;
   }

   // + - / * operations - SLOW
   Vector3 &operator+=(const Vector3 &pVec)
   {
      *this = *this + pVec; 
      return *this;
   }

   Vector3 &operator-=(const Vector3 &pVec)
   {
      *this = *this - pVec;
      return *this;
   }

   Vector3 &operator*=(const Vector3 &pVec)
   {
      *this = *this * pVec;
      return *this;
   }

   Vector3 &operator*=(float val)
   {
      *this = *this * val; 
      return *this;
   }

   Vector3 &operator/=(const Vector3 &pVec)
   {
      *this = *this / pVec; 
      return *this;
   }

   Vector3 &operator/=(float val)
   {
      *this = *this / val; 
      return *this;
   }

   Vector3 operator+(const Vector3 &pVec) const
   {
      return Vector3(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2]);
   }

   Vector3 operator-(const Vector3 &pVec) const
   {
      return Vector3(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2]);
   }

   Vector3 operator*(const Vector3 &pVec) const
   {
      return Vector3(v[0] * pVec.v[0], v[1] * pVec.v[1], v[2] * pVec.v[2]);
   }

   Vector3 operator*(float val) const
   {
      return Vector3(v[0] * val, v[1] * val, v[2] * val);
   }

   friend Vector3 operator*(float val, const Vector3 &v)
   {
      return Vector3(v[0] * val, v[1] * val, v[2] * val);
   }

   Vector3 operator/(const Vector3 &pVec) const
   {
      return Vector3(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2]);
   }

   Vector3 operator/(float val) const
   {
      return Vector3(v[0] / val, v[1] / val, v[2] / val);
   }

   Vector3 operator-(void) const
   {
      return Vector3(-v[0], -v[1], -v[2]);
   } 
   // end slow stuff

   // + - * / operations that don't return a Vector3
   // There are here so we don't get killed for speed
   inline void Clear(void)                    
   {
      v[0] = v[1] = v[2] = 0.0f;
   }

   inline void Set(float x, float y, float z)
   {
      v[0] = x;
      v[1] = y;
      v[2] = z;
   }

   inline void Set(const Vector3 &p)
   {
      v[0] = p[0];
      v[1] = p[1];
      v[2] = p[2];
   }
   
   inline void Add(const Vector3 &a, const Vector3 &b)
   {
      v[0] = a.v[0] + b.v[0];
      v[1] = a.v[1] + b.v[1];
      v[2] = a.v[2] + b.v[2];
   }

   inline void Add(const Vector3 &a)
   {
      v[0] += a.v[0];
      v[1] += a.v[1];
      v[2] += a.v[2];
   }

   inline void Add(const float &x, const float &y, const float &z)
   {
      v[0] += x;
      v[1] += y;
      v[2] += z;
   }

   inline void Subtract(const Vector3 &a, const Vector3 &b)
   {
      v[0] = a.v[0] - b.v[0];
      v[1] = a.v[1] - b.v[1];
      v[2] = a.v[2] - b.v[2];
   }

   inline void Subtract(const Vector3 &a)
   {
      v[0] -= a.v[0];
      v[1] -= a.v[1];
      v[2] -= a.v[2];
   }

   inline void Subtract(const float &x, const float &y, const float &z)
   {
      v[0] -= x;
      v[1] -= y;
      v[2] -= z;
   }

   inline void Multiply(const Vector3 &a, const Vector3 &b)
   {
      v[0] = a.v[0] * b.v[0];
      v[1] = a.v[1] * b.v[1];
      v[2] = a.v[2] * b.v[2];
   }

   inline void Multiply(const Vector3 &a)
   {
      v[0] *= a.v[0];
      v[1] *= a.v[1];
      v[2] *= a.v[2];
   }

   // !!TODO check for divide by 0 on these?
   inline void Divide(const Vector3 &a, const Vector3 &b)
   {
      v[0] = a.v[0] / b.v[0];
      v[1] = a.v[1] / b.v[1];
      v[2] = a.v[2] / b.v[2];
   }

   inline void Divide(const Vector3 &a)
   {
      v[0] /= a.v[0];
      v[1] /= a.v[1];
      v[2] /= a.v[2];
   }

   inline void Scale(float val)
   {
      v[0] *= val;
      v[1] *= val;
      v[2] *= val;
   }

   inline void Fabs(const Vector3 &src)
   {
      v[0] = MathLib::Abs(src.v[0]);
      v[1] = MathLib::Abs(src.v[1]);
      v[2] = MathLib::Abs(src.v[2]);
   }
   void Fabs(void)
   {
      v[0] = MathLib::Abs(v[0]);
      v[1] = MathLib::Abs(v[1]);
      v[2] = MathLib::Abs(v[2]);
   }

   inline float Dot(const Vector3 &pVec) const
   {
      return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
   }

   void Normalize(void);
   float Length(void) const;
   void Cross(const Vector3 &p, const Vector3 &q);
   void Clamp(float min, float max);

   float GetDistance(const Vector3 &dest) const;

   void RotateX(float amnt, Vector3 &dest) const;
   void RotateY(float amnt, Vector3 &dest) const;
   void RotateZ(float amnt, Vector3 &dest) const;

   // project v onto 'onto'
   void Project(const Vector3 &v, const Vector3 &onto); 
   // project v onto the plane formed by 'v1' and 'v2'
   void ProjectPlane(const Vector3 &v, const Vector3 &v1, const Vector3 &v2);
   
   void Lerp(const Vector3 &start, const Vector3 &end, float val);

//protected:
	float v[3];

};

inline void Vector3::Normalize(void) 
{
	float length, len = 0;

	length = Length();

	if (length == 0)
		return;

	len = 1.0f / length;

	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

inline float Vector3::Length(void) const
{
   double length = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
	
	return (float) sqrt(length);
}

inline void Vector3::Cross(const Vector3 &p, const Vector3 &q)
{
	v[0] = (p.v[1] * q.v[2]) - (p.v[2] * q.v[1]);
	v[1] = (p.v[2] * q.v[0]) - (p.v[0] * q.v[2]);
	v[2] = (p.v[0] * q.v[1]) - (p.v[1] * q.v[0]);
}

// !!TODO this sucks, fix it
inline void Vector3::Clamp(float min, float max)
{
   if (v[0] > max)
      v[0] = v[0] - max;
   if (v[0] < min)
      v[0] = max + v[0];

   if (v[1] > max)
      v[1] = v[1] - max;
   if (v[1] < min)
      v[1] = max + v[1];

   if (v[2] > max)
      v[2] = v[2] - max;
   if (v[2] < min)
      v[2] = max + v[2];
}

inline float Vector3::GetDistance(const Vector3 &dest) const
{
   float d1 = dest[0] - v[0];
   float d2 = dest[1] - v[1];
   float d3 = dest[2] - v[2];
   
   return (float) sqrt((d1 * d1) + (d2 * d2) + (d3 * d3));
}

inline void Vector3::RotateX(float amnt, Vector3 &dest) const
{
   float s = MathLib::Sin(amnt);
   float c = MathLib::Cos(amnt);
   float y = v[1];
   float z = v[2];

   dest[0] = v[0];
   dest[1] = (y * c) - (z * s);
   dest[2] = (y * s) + (z * c);
}

inline void Vector3::RotateY(float amnt, Vector3 &dest) const
{
   float s = MathLib::Sin(amnt);
   float c = MathLib::Cos(amnt);
   float x = v[0];
   float z = v[2];

   dest[0] = (x * c) + (z * s);
   dest[1] = v[1];
   dest[2] = (z * c) - (x * s);
}

inline void Vector3::RotateZ(float amnt, Vector3 &dest) const
{
   float s = MathLib::Sin(amnt);
   float c = MathLib::Cos(amnt);
   float x = v[0];
   float y = v[1];

   dest[0] = (x * c) - (y * s);
   dest[1] = (y * c) + (x * s);
   dest[2] = v[2];
}

inline void Vector3::Project(const Vector3 &v, const Vector3 &onto)
{
   (*this).Set(v);
   (*this).Scale((v.Dot(onto) / onto.Dot(onto)));
}

inline void Vector3::ProjectPlane(const Vector3 &v, const Vector3 &v1,
 const Vector3 &v2)
{
   Vector3 t;

   t.Project(v, v2);
   (*this).Project(v, v1);
   (*this).Add(t);   
}

inline void Vector3::Lerp(const Vector3 &start, const Vector3 &end, float val)
{
   float t = 1.0f - val;

   v[0] = (t * start.v[0]) + (val * end.v[0]);
   v[1] = (t * start.v[1]) + (val * end.v[1]);
   v[2] = (t * start.v[2]) + (val * end.v[2]);
}

}

#endif