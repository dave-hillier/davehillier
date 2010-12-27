#ifndef _Vector3_h
#define _Vector3_h

#include "math.h"
// The implementation of this probably doesnt have to be moved 
// To the cpp as its more effecient here where it can be inlined more readily
class Vector3
{
public:
	Vector3() : x_(0), y_(0), z_(0) {}
	Vector3(float x, float y, float z) : x_(x), y_(y), z_(z) {}
	Vector3(const Vector3 &v) : x_(v.x_), y_(v.y_), z_(v.z_) {}

	float x() const { return x_; }
	float y() const { return y_; }
	float z() const { return z_; }

	void operator =(const Vector3& v)
	{
		x(v.x_);
		y(v.y_);
		z(v.z_);
	}
	void operator +=(const Vector3& v)
	{
		x(x_ + v.x_);
		y(y_ + v.y_);
		z(z_ + v.z_);
	}
	void operator -=(const Vector3& v)
	{
		x(x_ - v.x_);
		y(y_ - v.y_);
		z(z_ - v.z_);
	}

	void operator *=(float f)
	{
		x(x_ * f);
		y(y_ * f);
		z(z_ * f);
	}

	void operator /=(float f)
	{
		x(x_ / f);
		y(y_ / f);
		z(z_ / f);
	}

	float LengthSqr() const {
		return x_ * x_ + y_ * y_ + z_ * z_;
	}
	float Length() const {
		return sqrtf(LengthSqr());
	}

	void Normalise()
	{
		(*this) /= Length();
	}

	// this . v0
	float DotProduct(const Vector3& v0) const
	{
		return ( x_ * v0.x_ + y_ * v0.y_ + z_ * v0.z_ );
	}

	void x(float f) { x_=f; }
	void y(float f) { y_=f; }
	void z(float f) { z_=f; }

private:
	float x_, y_, z_;
};
// Simple operators, TODO Binary operators

// v1 x v2 = 
inline Vector3 CrossProduct( const Vector3& v1, const Vector3& v2 ) 
{
	return Vector3(
		v1.y()*v2.z()-v2.y()*v1.z(),
		v1.z()*v2.x()-v2.z()*v1.x(),
		v1.x()*v2.y()-v2.x()*v1.y());
}

inline Vector3 operator-( const Vector3& v1, const Vector3& v2 )
{
	Vector3 v(v1);
	v -= v2;
	return v;
}

inline Vector3 operator+( const Vector3& v1, const Vector3& v2 )
{
	Vector3 v(v1);
	v += v2;
	return v;
}

inline Vector3 operator/( const Vector3& v1, float f )
{
	Vector3 v(v1);
	v /= f;
	return v;
}

inline Vector3 operator*( const Vector3& v1, float f )
{
	Vector3 v(v1);
	v *= f;
	return v;
}

#endif // _Vector3_h
