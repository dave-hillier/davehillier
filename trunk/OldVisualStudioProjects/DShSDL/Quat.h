
#ifndef _Quat_h_
#define _Quat_h_

typedef float Radians;
#include <assert.h>
#include "Vector3.h"

class Matrix;

namespace std {
	class ostream;
}

class Quat
{
public:
	// Default constructor initialises to null rotation
	// assert( isNormal() )
	Quat();

	// set by matrix
	Quat( const Matrix &m );

	// Set the scalar-vector pair directly
	Quat( float x, float y, float z, float s );

	// Set from a non-zero direction vector and angle in radians
	// assert( d.modulus() > 0.0 )
	// assert( isNormal() )
	Quat( const Vector3& d, const Radians& angle );

	// Set from the rotation that links a pair of unit vectors
	// assert( from.isUnitVector() )
	// assert( to.isUnitVector() )
	// assert( -1.0 != from.dotProduct(to) )
	// assert( isNormal() )
	Quat( const Vector3& from, const Vector3& to );

	// TODO Matrix ctor

	// Export the vector and scalar values
	const Vector3& vector( void ) const;
	const float&  scalar( void ) const;
	Matrix matrix() const;

	// TODO get matrix

	// Return the direction of the rotation axis in pUnitVector, and the angle of
	// rotation in radians as result
	// assert( isNormal() )
	// assert( fabs( pUnitVector.modulus() - 1.0 ) < Epsilon::instance() )
	float rotation( Vector3* pUnitVector ) const;

	// Quat multiplication means concatenation of rotations
	void operator *=( const Quat& q );

	// Set the scalar-vector pair directly
	void set( float x, float y, float z, float s );

	// Set from a non-zero direction vector and angle in radians
	// assert( d.modulus() > 0.0 )
	// assert( isNormal() )
	void set( const Vector3& d, const Radians& angle );

	// set matrix
	void set(const Matrix &mat);

	// Misc. operations
	void invert();

	void conjugate(void) {
		v_ *= -1;
	};

	// assert( isNormal() )
	Vector3 rotate( const Vector3& v ) const;
	bool isNormal() const;
	float modulus() const;

	// Sphereical linear interpolation. With degree being in the range [0..1], determines the
	// amount of interpolation with 0 being this and 1 being dest.
	// assert(degree >= 0.0 and degree <= 1.0);
	// assert(pResult != NULL);
	// assert(iff(degree == 0, *pResult == *this));
	// assert(iff(degree == 1, *pResult == dest));
	void slerp(const Quat& dest, float degree, Quat* pResult) const;

	void normalise();

	static void normalise( float* pX, float* pY, float* pZ, float* pS );
	// assert( fabs( (*pX)*(*pX) + (*pY)*(*pY) + (*pZ)*(*pZ) + (*pS)*(*pS) - 1.0 )  < Epsilon::instance() );

private:

	//Data members
	Vector3         v_; //Unit direction vector * sin(half rotation angle)
	float   s_; // cos(half rotation angle)

};

// global binary operators

std::ostream& operator <<( std::ostream& o, const Quat& t );


bool operator==( const Quat&, const Quat& );


const Quat operator*( const Quat& q1, const Quat& q2 );


inline
Quat::Quat()
: s_( 1.0 )
{
	assert( isNormal() );	
}


inline
const Vector3& Quat::vector( void ) const
{
	return v_;
}


inline
const float&  Quat::scalar( void ) const
{
	return s_;
}


inline
void Quat::invert()
{
	if( isNormal() )
		v_.negate();
	else
	{
		float invModulus = 1.0f/modulus();
		v_.negate();
		v_ *= invModulus;
		s_ *= invModulus;
	}

	
}


inline
float Quat::modulus() const
{
	return v_.x() * v_.x() + v_.y() * v_.y() + v_.z() * v_.z() + s_ * s_;
}


inline
bool Quat::isNormal() const
{	
	return fabsf( 1.0f - modulus() ) < Epsilon;
}


inline
void Quat::normalise()
{
	float xx = vector().x();
	float yy = vector().y();
	float zz = vector().z();
	float ww = scalar();

	Quat::normalise(&xx, &yy, &zz, &ww);

	v_.x(xx);
	v_.y(yy);
	v_.z(zz);
	s_ = ww;
}


// global binary operators

inline
const Quat operator*( const Quat& q1, const Quat& q2 )
{
	Quat result = q1;
	result *= q2;
	return result;
}

#endif

