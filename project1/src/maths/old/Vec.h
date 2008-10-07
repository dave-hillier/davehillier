#pragma once

// TODO Unit Tests
 
namespace Maths 
{
	template <int N, class T>	
	class Vec
	{				
	public:
		int size() const { return N; }

		Vec(const T & t = T()) 
		{ for(int i = 0; i < N; i++) v[i] = t; }
		Vec(const T * tp)
		{ for(int i = 0; i < N; i++) v[i] = tp[i]; }

		const T * value() const
		{ return v; }


		T dot( const Vec<N,T> & rhs ) const
		{ 
			T r = 0;
			for(int i = 0; i < N; i++) r += v[i]*rhs.v[i];
			return r;
		}

		T length() const
		{
			T r = 0;
			for(int i = 0; i < N; i++) r += v[i]*v[i]; 
			return T(sqrt(r));
		}	

		T squareNorm() const
		{
			T r = 0;
			for(int i = 0; i < N; i++) r += v[i]*v[i]; 
			return r;
		}	

		void  negate()
		{ for(int i = 0; i < N; i++) v[i] = -v[i]; }


		T normalise() 
		{ 
			T sum(0);
			for(int i = 0; i < N; i++) 
				sum += v[i]*v[i];
			sum = T(sqrt(sum));
			if (sum > Maths::Constant::Epsilon)
				for(int i = 0; i < N; i++) 
					v[i] /= sum;
			return sum;
		}


		Vec<N,T> & value( const T * rhs )
		{ for(int i = 0; i < N; i++) v[i] = rhs[i]; return *this; }

		T & operator [] ( int i )
		{ return v[i]; }

		const T & operator [] ( int i ) const
		{ return v[i]; }

		Vec<N,T> & operator *= ( T d )
		{ for(int i = 0; i < N; i++) v[i] *= d; return *this;}

		Vec<N,T> & operator *= ( const Vec<N,T> & u )
		{ for(int i = 0; i < N; i++) v[i] *= u[i]; return *this;}

		Vec<N,T> & operator /= ( T d )
		{ if(d == 0) return *this; for(int i = 0; i < N; i++) v[i] /= d; return *this;}

		Vec<N,T> & operator += ( const Vec<N,T> & u )
		{ for(int i = 0; i < N; i++) v[i] += u.v[i]; return *this;}

		Vec<N,T> & operator -= ( const Vec<N,T> & u )
		{ for(int i = 0; i < N; i++) v[i] -= u.v[i]; return *this;}


		Vec<N,T> operator - () const
		{ Vec<N,T> rv = v; rv.negate(); return rv; }

		Vec<N,T> operator + ( const Vec<N,T> &v) const
		{ Vec<N,T> rt(*this); return rt += v; }

		Vec<N,T> operator - ( const Vec<N,T> &v) const
		{ Vec<N,T> rt(*this); return rt -= v; }

		Vec<N,T> operator * ( T d) const
		{ Vec<N,T> rt(*this); return rt *= d; }

	//protected:
		T v[N];
	};



	// Vector friend operators

	template <int N, class T> inline
	Vec<N,T> operator * ( const Vec<N,T> & b, T d )
	{
		Vec<N,T> rt(b);
		return rt *= d;
	}

	template <int N, class T> inline
	Vec<N,T> operator * ( T d, const Vec<N,T> & b )
	{ return b*d; }

	template <int N, class T> inline
	Vec<N,T> operator * ( const Vec<N,T> & b, const Vec<N,T> & d )
	{
		Vec<N,T> rt(b);
		return rt *= d;
	}

	template <int N, class T> inline
	Vec<N,T> operator / ( const Vec<N,T> & b, T d )
	{ Vec<N,T> rt(b); return rt /= d; }

	template <int N, class T> inline
	Vec<N,T> operator + ( const Vec<N,T> & v1, const Vec<N,T> & v2 )
	{ Vec<N,T> rt(v1); return rt += v2; }

	template <int N, class T> inline
	Vec<N,T> operator - ( const Vec<N,T> & v1, const Vec<N,T> & v2 )
	{ Vec<N,T> rt(v1); return rt -= v2; }


	template <int N, class T> inline
	bool operator == ( const Vec<N,T> & v1, const Vec<N,T> & v2 )
	{
		for(int i = 0; i < N; i++)
			if(v1.v[i] != v2.v[i])
				return false;
		return true;
	}

	template <int N, class T> inline
	bool operator != ( const Vec<N,T> & v1, const Vec<N,T> & v2 )
	{ return !(v1 == v2); }

	typedef Vec<3,unsigned char> Vec3ub;
	typedef Vec<4,unsigned char> Vec4ub;


	// TODO Operator () and x, y, z get/set
	class Vec2 : public Vec<2,Real>
	{
	public:
		Vec2(const Real & t = Real()) : Vec<2,Real>(t)
		{}
		Vec2(const Vec<2,Real> & t) : Vec<2,Real>(t)
		{}
		Vec2(const Real * tp) : Vec<2,Real>(tp)
		{}

		Vec2(Real x, Real y )
		{ v[0] = x; v[1] = y; }

		void value(Real & x, Real & y) const
		{ x = v[0]; y = v[1]; }

		Vec2 & value( const Real & x, const Real & y)
		{ v[0] = x; v[1] = y; return *this; }

	};


	class Vec3 : public Vec<3,Real>
	{
	public:
		Vec3(const Real & t = Real()) : Vec<3,Real>(t)
		{}
		Vec3(const Vec<3,Real> & t) : Vec<3,Real>(t)
		{}
		Vec3(const Real * tp) : Vec<3,Real>(tp)
		{}

		Vec3(Real x, Real y, Real z)
		{ v[0] = x; v[1] = y; v[2] = z; }

		void value(Real & x, Real & y, Real & z) const
		{ x = v[0]; y = v[1]; z = v[2]; }

		Vec3 cross( const Vec3 &rhs ) const
		{
			Vec3 rt;
			rt.v[0] = v[1]*rhs.v[2]-v[2]*rhs.v[1];
			rt.v[1] = v[2]*rhs.v[0]-v[0]*rhs.v[2];
			rt.v[2] = v[0]*rhs.v[1]-v[1]*rhs.v[0];	
			return rt;
		}

		Vec3 & value( const Real & x, const Real & y, const Real & z)
		{ v[0] = x; v[1] = y; v[2] = z; return *this; }

	};


	class Vec4 : public Vec<4,Real>
	{
	public:
		Vec4(const Real & t = Real()) : Vec<4,Real>(t)
		{}
		Vec4(const Vec<4,Real> & t) : Vec<4,Real>(t)
		{}

		Vec4(const Vec<3,Real> & t, Real fourth)

		{ v[0] = t.v[0]; v[1] = t.v[1]; v[2] = t.v[2]; v[3] = fourth; }
		Vec4(const Real * tp) : Vec<4,Real>(tp)
		{}
		Vec4(Real x, Real y, Real z, Real w)
		{ v[0] = x; v[1] = y; v[2] = z; v[3] = w; }

		void value(Real & x, Real & y, Real & z, Real & w) const
		{ x = v[0]; y = v[1]; z = v[2]; w = v[3]; }

		Vec4 & value( const Real & x, const Real & y, const Real & z, const Real & w)
		{ v[0] = x; v[1] = y; v[2] = z; v[3] = w; return *this; }
	};

	inline Vec3 homogenise(const Vec4 & v)
	{
		Vec3 rt;
		assert(v.v[3] != Constant::Zero);
		rt.v[0] = v.v[0]/v.v[3];
		rt.v[1] = v.v[1]/v.v[3];
		rt.v[2] = v.v[2]/v.v[3];
		return rt;
	}


}