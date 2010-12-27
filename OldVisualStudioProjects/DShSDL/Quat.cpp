
#include "Quat.h"
// Set the scalar-vector pair directly
#include "Matrix.h"

Quat::Quat( float x, float y, float z, float s )
{
	v_( x, y, z );
	s_ = s;

	
}

// Set from a non-zero direction vector and angle in radians

Quat::Quat( const Vector3& d, const Radians& angle)
{
	assert( d.modulus() > 0.0f );

	//Store the vector and normalise it
	v_ = d;
	v_.makeUnitVector();

	//Get the cos and sine of the half-angle
	s_ = cos( angle * 0.5f );
	float f = sin( angle * 0.5f );
	v_ *= f;

	assert( isNormal() );
}

Quat::Quat(const Matrix &m)
{
	set(m);
}

void Quat::set( float x, float y, float z, float s )
{
	v_.set( x, y, z );
	s_ = s;
}
//////////////////////////////////////////////////////////////////////////////////////////


void Quat::set( const Vector3& d, const Radians& angle )
{
	assert( d.modulus() > 0.0 );

	//Store the vector and normalise it
	v_ = d;
	v_.makeUnitVector();

	//Get the cos and sine of the half-angle
	s_ = cos( angle * 0.5f );
	float f = sin( angle * 0.5f );
	v_ *= f;

	assert( isNormal() );
}
//////////////////////////////////////////////////////////////////////////////////////////
void Quat::set(const Matrix &mat)
{
	float tr, s, q[4];
	int i, j, k;
	int nxt[3] = {1, 2, 0};

	tr = mat[0][0] + mat[1][1] + mat[2][2];

	if (tr > 0.0)
	{
		s = (float) sqrt(tr + 1.0f);
		s_ = s / 2.0f;
		s = 0.5f / s;
		v_.x((mat[2][1] - mat[1][2]) * s);
		v_.y((mat[0][2] - mat[2][0]) * s);
		v_.z((mat[1][0] - mat[0][1]) * s);
	}
	else
	{		
		i = 0;

		if (mat[1][1] > mat[0][0])
			i = 1;

		if (mat[2][2] > mat[i][i])
			i = 2;

		j = nxt[i];
		k = nxt[j];

		s = (float) sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0f);

		q[i] = s * 0.5f;

		if (s != 0.0f)
			s = 0.5f / s;

		q[3] = (mat[k][j] - mat[j][k]) * s;
		q[j] = (mat[j][i] + mat[i][j]) * s;
		q[k] = (mat[k][i] + mat[i][k]) * s;

		v_.x(q[0]);
		v_.y(q[1]);
		v_.z(q[2]);
		s_ = q[3];
	}
	assert( isNormal() );
}
//////////////////////////////////////////////////////////////////////////////////////////

float Quat::rotation( Vector3* pUnitVector ) const
{
	
	assert( isNormal() );

	//Compute the sine of the half - angle
	float halfSine = v_.modulus();

	//Hence compute the sine and cosine of the angle
	float sine = 2.0f * halfSine * s_;
	float cosine = 1.0f - 2.0f * halfSine * halfSine;
	float angle = atan2( sine, cosine );

	//Compute the unit direction vector. If the angle is very small,
	//use a dummy x vector
	if( halfSine < Epsilon )
	{
		pUnitVector->set( 1.0, 0.0, 0.0 );
	}
	else
	{
		pUnitVector->set( v_.x() / halfSine,
			v_.y() / halfSine,
			v_.z() / halfSine );
	}

	assert( fabs( pUnitVector->modulus() - 1.0 ) < Epsilon );
	
	return angle;
}
//////////////////////////////////////////////////////////////////////////////////////////
// static
void Quat::normalise( 
							   float* pX, float* pY, 
							   float* pZ, float* pS )
{
	float k = (*pX)*(*pX) + (*pY)*(*pY) + (*pZ)*(*pZ) + (*pS)*(*pS);

	if( k != 1.0 )
	{
		k = sqrt( k );
		*pX /= k;
		*pY /= k;
		*pZ /= k;
		*pS /= k;
	}

	assert( fabs( (*pX)*(*pX) + (*pY)*(*pY) + (*pZ)*(*pZ) + (*pS)*(*pS) - 1.0 )  < Epsilon );
}

//////////////////////////////////////////////////////////////////////////////////////////


void Quat::operator *=( const Quat& q )
{
	

	Vector3 vec1 = v_;
	vec1 *= q.s_;
	Vector3 vec2 = q.v_;
	vec2 *= s_;

	s_ = (s_ * q.s_) - v_.dotProduct( q.v_ );

	v_ = Vector3::crossProduct( v_, q.v_ );
	v_ += vec1;
	v_ += vec2;

	
}

//////////////////////////////////////////////////////////////////////////////////////////


Vector3 Quat::rotate( const Vector3& v ) const
{
	
	assert( isNormal() );

	// We want q * [0, v] * q'. Start by computing [0, v] * q'
	Vector3 intermediateVec = v;
	intermediateVec *= s_;
	intermediateVec += Vector3::crossProduct( v_, v );

	float intermediateScalar = v_.dotProduct( v );

	// Now compute the vector component of q * [0, v] * q'
	Vector3 result = v_;
	result *= intermediateScalar;
	result += Vector3::crossProduct( v_, intermediateVec );
	intermediateVec *= s_;
	result += intermediateVec;

	
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// Quat slerp().


void Quat::slerp(const Quat& dest, float degree, Quat* pResult) const
{
	assert(degree >= 0.0 && degree <= 1.0);
	assert(pResult);

	//The maths goes wrong at the limit and produces -dest as the result. Hence special case treatement.
	if( degree == 1.0 )
		*pResult = dest;
	else
	{
		// This is a slight variation on the SLERP algorithm found in the Febuary 1998, Game Developer.

		float quatW = dest.scalar();
		Vector3 quatXYZ = dest.vector();

		float omega;
		float cosom;
		float sinom;
		float scale0;
		float scale1;

		// Dot product of this and dest.
		cosom = vector().x() * dest.vector().x() + 
			vector().y() * dest.vector().y() +
			vector().z() * dest.vector().z() +
			scalar() * dest.scalar();

		if(cosom < 0.0)
		{
			cosom = -cosom;
			quatW = -quatW;
			quatXYZ.negate();
		}

		// This determines whether we can risk the division.
		const float epsilon = Epsilon;
		if(1.0f - cosom < epsilon)
		{
			scale0 = 1.0f - degree;
			scale1 = degree;
		}
		else
		{
			omega = ::acos(cosom);
			sinom = ::sin(omega);
			scale0 = ::sin((1.0f - degree) * omega) / sinom;
			scale1 = ::sin(degree * omega) / sinom;
		}

		pResult->v_.x(scale0 * vector().x() + scale1 * quatXYZ.x());
		pResult->v_.y(scale0 * vector().y() + scale1 * quatXYZ.y());
		pResult->v_.z(scale0 * vector().z() + scale1 * quatXYZ.z());
		pResult->s_ = scale0 * scalar() + scale1 * quatW;
	}

	//assert(implies(degree == 0, *pResult == *this));
	//assert(implies(degree == 1, *pResult == dest));
}


std::ostream& operator <<( std::ostream& o, const Quat& t )
{

//	o << "[" << t.vector() << t.scalar() << "]";

	return o;
}


bool operator ==( const Quat& a, const Quat& b )
{
	return a.vector() == b.vector() && fabs( a.scalar() - b.scalar() ) < Epsilon;
}


Quat::Quat( const Vector3& from, const Vector3& to ) :
v_(Vector3::crossProduct(from,to))
{
	assert( from.isUnit() );
	assert( to.isUnit() );
	assert( -1.0 != from.dotProduct(to) );

	// Look - no transcendentals!
	const float dp = from.dotProduct(to);
	const float cosHalfAngle = sqrt(0.5f * (1.0f + dp));
	assert(cosHalfAngle > 0.0f);	// Thanks to the pre-conditions
	s_ = cosHalfAngle;
	v_ *= (0.5f/cosHalfAngle);

	assert( isNormal() );
}


Matrix Quat::matrix() const
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = v_.x() + v_.x();
	y2 = v_.y() + v_.y(); 
	z2 = v_.z() + v_.z();

	xx = v_.x() * x2;
	xy = v_.x() * y2;
	xz = v_.x() * z2;
	yy = v_.y() * y2;
	yz = v_.y() * z2;
	zz = v_.z() * z2;
	wx = s_ * x2;
	wy = s_ * y2;
	wz = s_ * z2;

	Matrix result;
	result[0][0] = 1.0f - (yy + zz);
	result[0][1] = xy - wz;
	result[0][2] = xz + wy;
	result[0][3] = 0.0f;
	result[1][0] = xy + wz;
	result[1][1] = 1.0f - (xx + zz);
	result[1][2] = yz - wx;
	result[1][3] = 0.0f;
	result[2][0] = xz - wy;
	result[2][1] = yz + wx;
	result[2][2] = 1.0f - (xx + yy);
	result[2][3] = 0.0f;
	result[3][0] = 0;
	result[3][1] = 0;
	result[3][2] = 0;
	result[3][3] = 1;
	return result;
}
