
#ifndef __VECTOR3H__
#define __VECTOR3H__

namespace platformMath {

class Vector3f {
public:
	// Constructors
	__inline Vector3f() { x=0; y=0; z=0; }
	__inline Vector3f(const Vector3f& v) { x=v.x; y=v.y; z=v.z; }
	__inline Vector3f(const float x, const float y, const float z) { this->x=x; this->y=y; this->z=z; }
	__inline Vector3f(const float *v) { x=vec[0]; y=vec[1]; z=vec[2]; }
	__inline Vector3f(const float a) { x = y = z = a; }
	// Destructor
	__inline ~Vector3f() { }

	// Simple arithmetic
	__inline Vector3f& operator = (const Vector3f& v) { x=v.x; y=v.y; z=v.z; return *this;    }
	__inline Vector3f  operator + (const Vector3f& v) { return Vector3f(x+v.x, y+v.y, z+v.z); }
	__inline Vector3f  operator - (const Vector3f& v) { return Vector3f(x-v.x, y-v.y, z-v.z); }
	__inline Vector3f  operator * (const Vector3f& v) { return Vector3f(x*v.x, y*v.y, z*v.z); }
	__inline Vector3f  operator / (const Vector3f& v) { return Vector3f(x/v.x, y/v.y, z/v.z); }
	__inline Vector3f  operator + (const float a)     { return Vector3f(x+a,   y+a,   z+a);   }
	__inline Vector3f  operator - (const float a)     { return Vector3f(x-a,   y-a,   z-a);   }
	__inline Vector3f  operator * (const float a)     { return Vector3f(x*a,   y*a,   z*a);   }
	__inline Vector3f  operator / (const float a)     { return Vector3f(x/a,   y/a,   z/a);   }
	__inline Vector3f  operator - (void)              { return Vector3f(-x,    -y,    -z);    }
	__inline friend Vector3f  operator * (float a, const Vector3f& v) { return Vector3f(a*v.x, a*v.y, a*v.z); }

	// More arithmetic
	__inline Vector3f& operator += (const Vector3f& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	__inline Vector3f& operator -= (const Vector3f& v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	__inline Vector3f& operator *= (const Vector3f& v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
	__inline Vector3f& operator /= (const Vector3f& v) { x/=v.x; y/=v.y; z/=v.z; return *this; }
	__inline Vector3f& operator += (const float a)     { x+=a;   y+=a;   z+=a;   return *this; }
	__inline Vector3f& operator -= (const float a)     { x-=a;   y-=a;   z-=a;   return *this; }
	__inline Vector3f& operator *= (const float a)     { x*=a;   y*=a;   z*=a;   return *this; }
	__inline Vector3f& operator /= (const float a)     { x/=a;   y/=a;   z/=a;   return *this; }

	// Faster arithmetic that doesn't create a new Vector3f each time
	__inline Set(const float x, const float y, const float z) { this->x=x; this->y=y; this->z=z; }
	__inline Set(const float a) { x=y=z=a; }
	__inline Set(const Vector3f& v) { x=v.x; y=v.y; z=v.z; }
	__inline Set(const Vector3f *v) { x=v->x; y=v->y; z=v->z; }
	__inline Add(const float x, const float y, const float z) { this->x+=x; this->y+=y; this->z+=z; }
	__inline Add(const Vector3f& v) { x+=v.x; y+=v.y; z+=v.z; }
	__inline Sub(const float x, const float y, const float z) { this->x-=x; this->y-=y; this->z-=z; }
	__inline Sub(const Vector3f& v) { x-=v.x; y-=v.y; z-=v.z; }
	__inline Mul(const float x, const float y, const float z) { this->x*=x; this->y*=y; this->z*=z; }
	__inline Mul(const Vector3f& v) { x*=v.x; y*=v.y; z*=v.z; }
	__inline Mul(const float a) { x*=a; y*=a; z*=a; }
	__inline Div(const float x, const float y, const float z) { this->x/=x; this->y/=y; this->z/=z; }
	__inline Div(const Vector3f& v) { x/=v.x; y/=v.y; z/=v.z; }
	__inline Div(const float a) { x/=a; y/=a; z/=a; }
	__inline Abs(const Vector3f& v) { x=(float )fabs(v.x); y=(float )fabs(v.y); z=(float )fabs(v.z); }
	__inline Abs() { x=(float )fabs(x); y=(float )fabs(y); z=(float )fabs(z); }
	// Reverse subtract
	__inline RevSub(const float x, const float y, const float z) { this->x=x-this->x; this->y=y-this->y; this->z=z-this->z; }
	__inline RevSub(const Vector3f& v) { x=v.x-x; y=v.y-y; z=v.z-z; }
	__inline Clear() { x=y=z=0; }

	// Rotate the vector by an angle in degrees
	__inline RotateX(float angle) {
		float s = (float )sin(MathLib::DegToRad(angle));
		float c = (float )cos(MathLib::DegToRad(angle));
		float temp = y*c - z*s;
		z = z*c + y*s;
		y = temp;
	}
	__inline RotateY(float angle) {
		float s = (float )sin(MathLib::DegToRad(angle));
		float c = (float )cos(MathLib::DegToRad(angle));
		float temp = x*c + z*s;
		z = z*c - x*s;
		x = temp;
	}
	__inline RotateZ(float angle) {
		float s = (float )sin(MathLib::DegToRad(angle));
		float c = (float )cos(MathLib::DegToRad(angle));
		float temp = x*c - y*s;
		y = y*c + x*s;
		x = temp;
	}

	// Cross product
	__inline Vector3f operator % (const Vector3f& v) { return Vector3f(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }
	// Vector length
	__inline float Length()  { return sqrtf(x*x + y*y + z*z); }
	// Squared vector length
	__inline float Length2() { return x*x + y*y + z*z; }


	__inline Vector3f Reflection(Vector3f n) { 
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

	__inline float Dot(const Vector3f& v) { return x*v.x + y*v.y + z*v.z; }

	// Float array operations
	__inline operator float *(void) { return vec; }
	__inline float& operator [] (const int i) { return vec[i]; }

	// Comparison operators
	__inline bool operator == (const Vector3f& v) {
		if ((x==v.x) && (y==v.y) && (z==v.z))
			return true;
		return false;
	}
	__inline bool operator != (const Vector3f& v) {
		if ((x!=v.x) || (y!=v.y) || (z!=v.z))
			return true;
		return false;
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

typedef struct {
	int x, y;
} Vector2i;

typedef struct {
	Vector3f min, max;
} Bounds;

typedef struct {
	Vector3f start;
	Vector3f dir;
} Ray3f;

typedef struct {
	Vector3f n;
	float d;
} Plane;


}

#endif