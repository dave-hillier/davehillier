#ifndef _Vector3_h
#define _Vector3_h

#include "Common.h"
// TODO Stream operators


class Vector3
{
public:
	Vector3() : x_(0), y_(0), z_(0) {}
	Vector3(float x, float y, float z) : x_(x), y_(y), z_(z) {}
	Vector3(const Vector3 &v) : x_(v.x_), y_(v.y_), z_(v.z_) {}

	float dotProduct( const Vector3& v ) const { return x_*v.x_+y_*v.y_+z_*v.z_; }

	bool isPerpendicularTo( const Vector3& v ) const { return fabs(dotProduct(v)) < Epsilon; }

	float squareModulus() const { return x_*x_+y_*y_+z_*z_;	}
	float modulus() const { return sqrtf(squareModulus()); }

	float sqrEuclidianDistance( const Vector3& to ) const
	{
		Vector3 from(*this);
		from -= to;
		return from.squareModulus();
	}

	float euclidianDistance( const Vector3& to ) const { return sqrtf(sqrEuclidianDistance(to)); }

	void interpolate( const Vector3& v, float t ) {
		x_ += t * ( v.x_ - x_ );
		y_ += t * ( v.y_ - y_ );
		z_ += t * ( v.z_ - z_ );
	}

	// 
	void makeUnitVector()
	{
		const float factor = 1/modulus();
		x_ *= factor;
		y_ *= factor;
		z_ *= factor;
		assert( modulus() == 1.0 );
	}

	static const Vector3& zeroVector() {
		static const Vector3 z;
		return z;
	}

	bool isZero() const { return fabs(x_) < Epsilon && fabs(y_) < Epsilon && fabs(z_) < Epsilon; }
	bool isUnit() const { return fabs(1 - squareModulus()) < 2.0f * Epsilon; } // Square mod double error

	// Reverse the direction of the vector.
	void negate() { x_ = -x_; y_ = -y_; z_ = -z_; }

	float x() const { return x_; }
	float y() const { return y_; }
	float z() const { return z_; }

	void x(float f) { x_=f; }
	void y(float f) { y_=f; }
	void z(float f) { z_=f; }

	void set(float x, float y, float z) { x_=x; y_=y; z_=z; }

	void operator ()(float x, float y, float z) { set(x,y,z); }

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
	inline static Vector3 crossProduct( const Vector3& v1, const Vector3& v2 ) 
	{
		return Vector3(
			v1.y()*v2.z()-v2.y()*v1.z(),
			v1.z()*v2.x()-v2.z()*v1.x(),
			v1.x()*v2.y()-v2.x()*v1.y());
	}

	Vector3 operator -() const
	{
		Vector3 v(0,0,0); 
		v -= *this;
		return v;
	}

private:
	float x_, y_, z_;
};

// definitions

// Simple operators, Binary operators


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
inline bool operator ==( const Vector3& a, const Vector3& b )
{
	return a.sqrEuclidianDistance(b) < 2.0f * Epsilon; // Double Epsilon as its a squared result;
}

#endif // _Vector3_h
