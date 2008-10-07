#pragma once


#include "Common.h"
#include "Vec.h"

namespace Maths
{


	class quaternion
	{
	public:

		quaternion()
		{
			*this = identity();
		}

		quaternion( const Real v[4] )
		{
			value( v );
		}


		quaternion( Real q0, Real q1, Real q2, Real q3 )
		{
			value( q0, q1, q2, q3 );
		}


		quaternion( const Matrix4 & m )
		{
			value( m );
		}


		quaternion( const Vec3 &axis, Real radians )
		{
			value( axis, radians );
		}


		quaternion( const Vec3 &rotateFrom, const Vec3 &rotateTo )
		{
			value( rotateFrom, rotateTo );
		}

		quaternion( const Vec3 & from_look, const Vec3 & from_up,
			const Vec3 & to_look, const Vec3& to_up)
		{
			value(from_look, from_up, to_look, to_up);
		}

		const Real * value() const
		{
			return  &q[0];
		}

		void value( Real &q0, Real &q1, Real &q2, Real &q3 ) const
		{
			q0 = q[0];
			q1 = q[1];
			q2 = q[2];
			q3 = q[3];
		}

		quaternion & value( Real q0, Real q1, Real q2, Real q3 )
		{
			q[0] = q0;
			q[1] = q1;
			q[2] = q2;
			q[3] = q3;
			counter = 0;
			return *this;
		}

		void value( Vec3 &axis, Real &radians ) const
		{
			radians = Real(acos( q[3] ) * Constant::Two);
			if ( radians == Constant::Zero )
				axis = Vec3( 0.0, 0.0, 1.0 );
			else
			{
				axis.v[0] = q[0];
				axis.v[1] = q[1];
				axis.v[2] = q[2];
				axis.normalize();
			}
		}

		void value( Matrix4 & m ) const
		{
			Real s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

			Real norm = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];

			s = (equivalent(norm,Constant::Zero)) ? Constant::Zero : ( Constant::Two / norm );

			xs = q[0] * s;
			ys = q[1] * s;
			zs = q[2] * s;

			wx = q[3] * xs;
			wy = q[3] * ys;
			wz = q[3] * zs;

			xx = q[0] * xs;
			xy = q[0] * ys;
			xz = q[0] * zs;

			yy = q[1] * ys;
			yz = q[1] * zs;
			zz = q[2] * zs;

			m(0,0) = Real( Constant::One - ( yy + zz ));
			m(1,0) = Real ( xy + wz );
			m(2,0) = Real ( xz - wy );

			m(0,1) = Real ( xy - wz );
			m(1,1) = Real ( Constant::One - ( xx + zz ));
			m(2,1) = Real ( yz + wx );

			m(0,2) = Real ( xz + wy );
			m(1,2) = Real ( yz - wx );
			m(2,2) = Real ( Constant::One - ( xx + yy ));

			m(3,0) = m(3,1) = m(3,2) = m(0,3) = m(1,3) = m(2,3) = Constant::Zero;
			m(3,3) = Constant::One;
		}

		quaternion & value( const Real * qp )
		{
			memcpy(q,qp,sizeof(Real) * 4);

			counter = 0;
			return *this;
		}

		quaternion & value( const Matrix4 & m )
		{
			Real tr, s;
			int i, j, k;
			const int nxt[3] = { 1, 2, 0 };

			tr = m(0,0) + m(1,1) + m(2,2);

			if ( tr > Constant::Zero )
			{
				s = Real(sqrt( tr + m(3,3) ));
				q[3] = Real ( s * 0.5 );
				s = Real(0.5) / s;

				q[0] = Real ( ( m(1,2) - m(2,1) ) * s );
				q[1] = Real ( ( m(2,0) - m(0,2) ) * s );
				q[2] = Real ( ( m(0,1) - m(1,0) ) * s );
			}
			else
			{
				i = 0;
				if ( m(1,1) > m(0,0) )
					i = 1;

				if ( m(2,2) > m(i,i) )
					i = 2;

				j = nxt[i];
				k = nxt[j];

				s = Real(sqrt( ( m(i,j) - ( m(j,j) + m(k,k) )) + Constant::One ));

				q[i] = Real ( s * 0.5 );
				s = Real(0.5 / s);

				q[3] = Real ( ( m(j,k) - m(k,j) ) * s );
				q[j] = Real ( ( m(i,j) + m(j,i) ) * s );
				q[k] = Real ( ( m(i,k) + m(k,i) ) * s );
			}

			counter = 0;
			return *this;
		}

		quaternion & value( const Vec3 &axis, Real theta )
		{
			Real sqnorm = axis.square_norm();

			if (sqnorm <= Constant::Epsilon)
			{
				// axis too small.
				x = y = z = 0.0;
				w = 1.0;
			} 
			else 
			{
				theta *= Real(0.5);
				Real sin_theta = Real(sin(theta));

				if (!equivalent(sqnorm,Constant::One)) 
					sin_theta /= Real(sqrt(sqnorm));
				x = sin_theta * axis.v[0];
				y = sin_theta * axis.v[1];
				z = sin_theta * axis.v[2];
				w = Real(cos(theta));
			}
			return *this;
		}

		quaternion & value( const Vec3 & rotateFrom, const Vec3 & rotateTo )
		{
			Vec3 p1, p2;
			Real alpha;

			p1 = rotateFrom; 
			p1.normalize();
			p2 = rotateTo;  
			p2.normalize();

			alpha = p1.dot(p2);

			if(equivalent(alpha,Constant::One))
			{ 
				*this = identity(); 
				return *this; 
			}

			// ensures that the anti-parallel case leads to a positive dot
			if(equivalent(alpha,-Constant::One))
			{
				Vec3 v;

				if(p1.v[0] != p1.v[1] || p1.v[0] != p1.v[2])
					v = Vec3(p1.v[1], p1.v[2], p1.v[0]);
				else
					v = Vec3(-p1.v[0], p1.v[1], p1.v[2]);

				v -= p1 * p1.dot(v);
				v.normalize();

				value(v, Constant::Pi);
				return *this;
			}

			p1 = p1.cross(p2);  
			p1.normalize();
			value(p1,Real(acos(alpha)));

			counter = 0;
			return *this;
		}

		quaternion & value( const Vec3 & from_look, const Vec3 & from_up,
			const Vec3 & to_look, const Vec3 & to_up)
		{
			quaternion r_look = quaternion(from_look, to_look);

			Vec3 rotated_from_up(from_up);
			r_look.multiplyVec(rotated_from_up);

			quaternion r_twist = quaternion(rotated_from_up, to_up);

			*this = r_twist;
			*this *= r_look;
			return *this;
		}

		quaternion & operator *= ( const quaternion & qr )
		{
			quaternion ql(*this);

			w = ql.w * qr.w - ql.x * qr.x - ql.y * qr.y - ql.z * qr.z;
			x = ql.w * qr.x + ql.x * qr.w + ql.y * qr.z - ql.z * qr.y;
			y = ql.w * qr.y + ql.y * qr.w + ql.z * qr.x - ql.x * qr.z;
			z = ql.w * qr.z + ql.z * qr.w + ql.x * qr.y - ql.y * qr.x;

			counter += qr.counter;
			counter++;
			counter_normalize();
			return *this;
		}

		void normalize()
		{
			Real rnorm = Constant::One / Real(sqrt(w * w + x * x + y * y + z * z));
			if (equivalent(rnorm, Constant::Zero))
				return;
			x *= rnorm;
			y *= rnorm;
			z *= rnorm;
			w *= rnorm;
			counter = 0;
		}

		friend bool operator == ( const quaternion & q1, const quaternion & q2 );      

		friend bool operator != ( const quaternion & q1, const quaternion & q2 );

		friend quaternion operator * ( const quaternion & q1, const quaternion & q2 );

		bool equals( const quaternion & r, Real tolerance ) const
		{
			Real t;

			t = (
				(q[0]-r.q[0])*(q[0]-r.q[0]) +
				(q[1]-r.q[1])*(q[1]-r.q[1]) +
				(q[2]-r.q[2])*(q[2]-r.q[2]) +
				(q[3]-r.q[3])*(q[3]-r.q[3]) );
			if(t > Constant::Epsilon) 
				return false;
			return 1;
		}

		quaternion & conjugate()
		{
			q[0] *= -Constant::One;
			q[1] *= -Constant::One;
			q[2] *= -Constant::One;
			return *this;
		}

		quaternion & invert()
		{
			return conjugate();
		}

		quaternion inverse() const
		{
			quaternion r = *this;
			return r.invert();
		}

		//
		// Quaternion multiplication with cartesian Vector
		// v' = q*v*q(star)
		//
		void multiplyVec( const Vec3 &src, Vec3 &dst ) const
		{
			Real v_coef = w * w - x * x - y * y - z * z;                     
			Real u_coef = Constant::Two * (src.v[0] * x + src.v[1] * y + src.v[2] * z);  
			Real c_coef = Constant::Two * w;                                       

			dst.v[0] = v_coef * src.v[0] + u_coef * x + c_coef * (y * src.v[2] - z * src.v[1]);
			dst.v[1] = v_coef * src.v[1] + u_coef * y + c_coef * (z * src.v[0] - x * src.v[2]);
			dst.v[2] = v_coef * src.v[2] + u_coef * z + c_coef * (x * src.v[1] - y * src.v[0]);
		}

		void multiplyVec( Vec3 & src_and_dst) const
		{
			multiplyVec(Vec3(src_and_dst), src_and_dst);
		}

		void scale_angle( Real scaleFactor )
		{
			Vec3 axis;
			Real radians;

			value(axis, radians);
			radians *= scaleFactor;
			value(axis, radians);
		}

		static quaternion slerp( const quaternion & p, const quaternion & q, Real alpha )
		{
			quaternion r;

			Real cos_omega = p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;
			// if B is on opposite hemisphere from A, use -B instead

			int bflip;
			if ( ( bflip = (cos_omega < Constant::Zero)) )
				cos_omega = -cos_omega;

			// complementary interpolation parameter
			Real beta = Constant::One - alpha;     

			if(cos_omega >= Constant::One - Constant::Epsilon)
				return p;

			Real omega = Real(acos(cos_omega));
			Real one_over_sin_omega = Constant::One / Real(sin(omega));

			beta    = Real(sin(omega*beta)  * one_over_sin_omega);
			alpha   = Real(sin(omega*alpha) * one_over_sin_omega);

			if (bflip)
				alpha = -alpha;

			r.x = beta * p.q[0]+ alpha * q.q[0];
			r.y = beta * p.q[1]+ alpha * q.q[1];
			r.z = beta * p.q[2]+ alpha * q.q[2];
			r.w = beta * p.q[3]+ alpha * q.q[3];
			return r;
		}

		static quaternion identity()
		{
			static quaternion ident( Vec3( 0.0, 0.0, 0.0 ), Constant::One );
			return ident;
		}

		Real & operator []( int i )
		{
			assert(i < 4);
			return q[i];
		}

		const Real & operator []( int i ) const
		{
			assert(i < 4);
			return q[i];
		}

	protected:

		void counter_normalize()
		{
			if (counter > 64) // TODO constant?
				normalize();
		}

		union 
		{
			struct 
			{
				Real q[4];
			};
			struct 
			{
				Real x;
				Real y;
				Real z;
				Real w;
			};
		};

		// renormalization counter
		unsigned char counter;
	};

	inline
	bool operator == ( const quaternion & q1, const quaternion & q2 )
	{
		return (equivalent(q1.x, q2.x) &&
			equivalent(q1.y, q2.y) &&
			equivalent(q1.z, q2.z) &&
			equivalent(q1.w, q2.w) );
	}

	inline
	bool operator != ( const quaternion & q1, const quaternion & q2 )
	{ 
		return ! ( q1 == q2 ); 
	}

	inline
	quaternion operator * ( const quaternion & q1, const quaternion & q2 )
	{	
		quaternion r(q1); 
		r *= q2; 
		return r; 
	}

}