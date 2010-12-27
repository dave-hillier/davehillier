#ifndef __VECTOR_H__
#define __VECTOR_H__

//---------------------------------------------------------------------------
#include <cmath>
//---------------------------------------------------------------------------
#define PI			(3.14159265359f)
#define DEG2RAD(a)	(PI/180*(a))
#define RAD2DEG(a)	(180/PI*(a))
//---------------------------------------------------------------------------
class Vector {
public:
	// Constructors
	__inline Vector() { x=0; y=0; z=0; }
	__inline Vector(const Vector& v) { x=v.x; y=v.y; z=v.z; }
	__inline Vector(const float x, const float y, const float z) { this->x=x; this->y=y; this->z=z; }
	__inline Vector(const float *v) { x=vec[0]; y=vec[1]; z=vec[2]; }
	__inline Vector(const float a) { x = y = z = a; }
	// Destructor
	__inline ~Vector() { }

	// Simple arithmetic
	__inline Vector& operator = (const Vector& v) { x=v.x; y=v.y; z=v.z; return *this;    }
	__inline Vector  operator + (const Vector& v) { return Vector(x+v.x, y+v.y, z+v.z); }
	__inline Vector  operator - (const Vector& v) { return Vector(x-v.x, y-v.y, z-v.z); }
	__inline Vector  operator * (const Vector& v) { return Vector(x*v.x, y*v.y, z*v.z); }
	__inline Vector  operator / (const Vector& v) { return Vector(x/v.x, y/v.y, z/v.z); }
	__inline Vector  operator + (const float a)     { return Vector(x+a,   y+a,   z+a);   }
	__inline Vector  operator - (const float a)     { return Vector(x-a,   y-a,   z-a);   }
	__inline Vector  operator * (const float a)     { return Vector(x*a,   y*a,   z*a);   }
	__inline Vector  operator / (const float a)     { return Vector(x/a,   y/a,   z/a);   }
	__inline Vector  operator - (void)              { return Vector(-x,    -y,    -z);    }
	__inline friend Vector  operator * (float a, const Vector& v) { return Vector(a*v.x, a*v.y, a*v.z); }

	// More arithmetic
	__inline Vector& operator += (const Vector& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	__inline Vector& operator -= (const Vector& v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	__inline Vector& operator *= (const Vector& v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
	__inline Vector& operator /= (const Vector& v) { x/=v.x; y/=v.y; z/=v.z; return *this; }
	__inline Vector& operator += (const float a)     { x+=a;   y+=a;   z+=a;   return *this; }
	__inline Vector& operator -= (const float a)     { x-=a;   y-=a;   z-=a;   return *this; }
	__inline Vector& operator *= (const float a)     { x*=a;   y*=a;   z*=a;   return *this; }
	__inline Vector& operator /= (const float a)     { x/=a;   y/=a;   z/=a;   return *this; }

	// Faster arithmetic that doesn't create a new Vector each time
	__inline Set(const float x, const float y, const float z) { this->x=x; this->y=y; this->z=z; }
	__inline Set(const float a) { x=y=z=a; }
	__inline Set(const Vector& v) { x=v.x; y=v.y; z=v.z; }
	__inline Set(const Vector *v) { x=v->x; y=v->y; z=v->z; }
	__inline Add(const float x, const float y, const float z) { this->x+=x; this->y+=y; this->z+=z; }
	__inline Add(const Vector& v) { x+=v.x; y+=v.y; z+=v.z; }
	__inline Sub(const float x, const float y, const float z) { this->x-=x; this->y-=y; this->z-=z; }
	__inline Sub(const Vector& v) { x-=v.x; y-=v.y; z-=v.z; }
	__inline Mul(const float x, const float y, const float z) { this->x*=x; this->y*=y; this->z*=z; }
	__inline Mul(const Vector& v) { x*=v.x; y*=v.y; z*=v.z; }
	__inline Mul(const float a) { x*=a; y*=a; z*=a; }
	__inline Div(const float x, const float y, const float z) { this->x/=x; this->y/=y; this->z/=z; }
	__inline Div(const Vector& v) { x/=v.x; y/=v.y; z/=v.z; }
	__inline Div(const float a) { x/=a; y/=a; z/=a; }
	__inline Abs(const Vector& v) { x=(float )fabs(v.x); y=(float )fabs(v.y); z=(float )fabs(v.z); }
	__inline Abs() { x=(float )fabs(x); y=(float )fabs(y); z=(float )fabs(z); }
	// Reverse subtract
	__inline RevSub(const float x, const float y, const float z) { this->x=x-this->x; this->y=y-this->y; this->z=z-this->z; }
	__inline RevSub(const Vector& v) { x=v.x-x; y=v.y-y; z=v.z-z; }
	__inline Clear() { x = y = z = 0; }

	// Rotate the vector by an angle in degrees
	__inline RotateX(float angle) {
		float s = (float )sin(DEG2RAD(angle));
		float c = (float )cos(DEG2RAD(angle));
		float temp = y*c - z*s;
		z = z*c + y*s;
		y = temp;
	}
	__inline RotateY(float angle) {
		float s = (float )sin(DEG2RAD(angle));
		float c = (float )cos(DEG2RAD(angle));
		float temp = x*c + z*s;
		z = z*c - x*s;
		x = temp;
	}
	__inline RotateZ(float angle) {
		float s = (float )sin(DEG2RAD(angle));
		float c = (float )cos(DEG2RAD(angle));
		float temp = x*c - y*s;
		y = y*c + x*s;
		x = temp;
	}

	__inline Vector Rotate(float dAngle, Vector& n) {// TODO test
		float c = (float)cos(dAngle);
		float s = (float)sin(dAngle);
		return Vector(	*this * c +
						((n * *this) * (1.0f - c)) * n +
						(n % *this) * s);
	}
	// Cross product
	__inline Vector operator % (const Vector& v) { return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }
	// Vector length
	__inline float Length()  { return sqrtf(x*x + y*y + z*z); }
	// Squared vector length
	__inline float Length2() { return x*x + y*y + z*z; }

	__inline Vector Reflection(Vector n) { 
		return (2*n*(Dot(n)) - *this); };

   __inline void Clamp(float min, float max) {
	   if (vec[0] > max)
		  vec[0] = vec[0] - max;
	   if (vec[0] < min)
		  vec[0] = max + vec[0];

	   if (vec[1] > max)
		  vec[1] = vec[1] - max;
	   if (vec[1] < min)
		  vec[1] = max + vec[1];

	   if (vec[2] > max)
		  vec[2] = vec[2] - max;
	   if (vec[2] < min)
		  vec[2] = max + vec[2];
	}
	//Normalise vector
	__inline void Normalise() {
		float len = Length();
		if (len == 0.0f)
			return;
		len = 1.0f / len;
		x *= len;
		y *= len;
		z *= len;
	}

	__inline float Dot(const Vector& v) { return x*v.x + y*v.y + z*v.z; }

	// Float array operations
	__inline operator float *(void) { return vec; }
	__inline float& operator [] (const int i) { return vec[i]; }

	// Comparison operators
	__inline bool operator == (const Vector& v) {
		return ((x==v.x) && (y==v.y) && (z==v.z));
	}
	__inline bool operator != (const Vector& v) {
		return ((x!=v.x) || (y!=v.y) || (z!=v.z));
	}


	union {
		struct {
			float vec[3];
		};
		struct {
			float x, y, z;
		};
	};
};

class Bounds{
public: 
	Vector min, max;
};
//---------------------------------------------------------------------------
#endif	//__VECTOR_H__