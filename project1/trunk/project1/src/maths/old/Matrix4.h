#pragma once


#include "Common.h"
#include "Vec.h"
#include <cassert>
// TODO Unit Tests
 
namespace Maths 
{

	class Matrix4
	{

	public:

		Matrix4() { identity(); }

		Matrix4( Real r ) 
		{ value(r); }

		Matrix4( Real * m )
		{ value(m); }

		Matrix4( Real a00, Real a01, Real a02, Real a03,
			Real a10, Real a11, Real a12, Real a13,
			Real a20, Real a21, Real a22, Real a23,
			Real a30, Real a31, Real a32, Real a33 )
		{
			element(0,0) = a00;
			element(0,1) = a01;
			element(0,2) = a02;
			element(0,3) = a03;

			element(1,0) = a10;
			element(1,1) = a11;
			element(1,2) = a12;
			element(1,3) = a13;

			element(2,0) = a20;
			element(2,1) = a21;
			element(2,2) = a22;
			element(2,3) = a23;

			element(3,0) = a30;
			element(3,1) = a31;
			element(3,2) = a32;
			element(3,3) = a33;
		}


		void value( Real * mp ) const
		{
			int c = 0;
			for(int j=0; j < 4; j++)
				for(int i=0; i < 4; i++)
					mp[c++] = element(i,j);
		}


		const Real * value() const
		{ return m; }

		void value( Real * mp)
		{
			int c = 0;
			for(int j=0; j < 4; j++)
				for(int i=0; i < 4; i++)
					element(i,j) = mp[c++];
		}

		void value( Real r ) 
		{
			for(int i=0; i < 4; i++)
				for(int j=0; j < 4; j++)
					element(i,j) = r;
		}

		void identity()
		{
			element(0,0) = 1.0;
			element(0,1) = 0.0;
			element(0,2) = 0.0; 
			element(0,3) = 0.0;

			element(1,0) = 0.0;
			element(1,1) = 1.0; 
			element(1,2) = 0.0;
			element(1,3) = 0.0;

			element(2,0) = 0.0;
			element(2,1) = 0.0;
			element(2,2) = 1.0;
			element(2,3) = 0.0;

			element(3,0) = 0.0; 
			element(3,1) = 0.0; 
			element(3,2) = 0.0;
			element(3,3) = 1.0;
		}


		static Matrix4 identity()
		{
			static Matrix4 mident (
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0  );
			return mident;
		}


		void scale( Real s )
		{
			element(0,0) = s;
			element(1,1) = s;
			element(2,2) = s;
		}

		void scale( const Vec3 & s )
		{
			element(0,0) = s.v[0];
			element(1,1) = s.v[1];
			element(2,2) = s.v[2];
		}


		void translate( const Vec3 & t )
		{
			element(0,3) = t.v[0];
			element(1,3) = t.v[1];
			element(2,3) = t.v[2];
		}

		void row(int r, const Vec4 & t)
		{
			element(r,0) = t.v[0];
			element(r,1) = t.v[1];
			element(r,2) = t.v[2];
			element(r,3) = t.v[3];
		}

		void column(int c, const Vec4 & t)
		{
			element(0,c) = t.v[0];
			element(1,c) = t.v[1];
			element(2,c) = t.v[2];
			element(3,c) = t.v[3];
		}


		void row(int r, Vec4 & t) const
		{
			t.v[0] = element(r,0);
			t.v[1] = element(r,1);
			t.v[2] = element(r,2);
			t.v[3] = element(r,3);
		}

		Vec4 row(int r) const
		{
			Vec4 v; row(r, v);
			return v;
		}

		void column(int c, Vec4 & t) const
		{
			t.v[0] = element(0,c);
			t.v[1] = element(1,c);
			t.v[2] = element(2,c);
			t.v[3] = element(3,c);
		}

		Vec4 column(int c) const
		{
			Vec4 v; column(c, v);
			return v;
		}

		Matrix4 inverse() const
		{
			Matrix4 minv;

			Real r1[8], r2[8], r3[8], r4[8];
			Real *s[4], *tmprow;

			s[0] = &r1[0];
			s[1] = &r2[0];
			s[2] = &r3[0];
			s[3] = &r4[0];

			int i,j,p,jj;
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{
					s[i][j] = element(i,j);
					if(i==j) s[i][j+4] = 1.0;
					else     s[i][j+4] = 0.0;
				}
			}
			Real scp[4];
			for(i=0;i<4;i++)
			{
				scp[i] = Real(fabs(s[i][0]));
				for(j=1;j<4;j++)
					if(Real(fabs(s[i][j])) > scp[i]) scp[i] = Real(fabs(s[i][j]));
				if(scp[i] == 0.0) return minv; // singular Matrix!
			}

			int pivot_to;
			Real scp_max;
			for(i=0;i<4;i++)
			{
				// select pivot row
				pivot_to = i;
				scp_max = Real(fabs(s[i][i]/scp[i]));
				// find out which row should be on top
				for(p=i+1;p<4;p++)
					if(Real(fabs(s[p][i]/scp[p])) > scp_max)
					{ scp_max = Real(fabs(s[p][i]/scp[p])); pivot_to = p; }
					// Pivot if necessary
					if(pivot_to != i)
					{
						tmprow = s[i];
						s[i] = s[pivot_to];
						s[pivot_to] = tmprow;
						Real tmpscp;
						tmpscp = scp[i];
						scp[i] = scp[pivot_to];
						scp[pivot_to] = tmpscp;
					}

					Real mji;
					// perform gaussian elimination
					for(j=i+1;j<4;j++)
					{
						mji = s[j][i]/s[i][i];
						s[j][i] = 0.0;
						for(jj=i+1;jj<8;jj++)
							s[j][jj] -= mji*s[i][jj];
					}
			}
			if(s[3][3] == 0.0) return minv; // singular Matrix!

			//
			// Now we have an upper triangular matrix.
			//
			//  x x x x | y y y y
			//  0 x x x | y y y y 
			//  0 0 x x | y y y y
			//  0 0 0 x | y y y y
			//
			//  we'll back substitute to get the inverse
			//
			//  1 0 0 0 | z z z z
			//  0 1 0 0 | z z z z
			//  0 0 1 0 | z z z z
			//  0 0 0 1 | z z z z 
			//

			Real mij;
			for(i=3;i>0;i--)
			{
				for(j=i-1;j > -1; j--)
				{
					mij = s[j][i]/s[i][i];
					for(jj=j+1;jj<8;jj++)
						s[j][jj] -= mij*s[i][jj];
				}
			}

			for(i=0;i<4;i++)
				for(j=0;j<4;j++)
					minv(i,j) = s[i][j+4] / s[i][i];

			return minv;
		}


		Matrix4 transpose() const
		{
			Matrix4 mtrans;

			for(int i=0;i<4;i++)
				for(int j=0;j<4;j++)
					mtrans(i,j) = element(j,i);		
			return mtrans;
		}

		Matrix4 & multiplyright( const Matrix4 & b )
		{
			Matrix4 mt(*this);
			value(Real(0));

			for(int i=0; i < 4; i++)
				for(int j=0; j < 4; j++)
					for(int c=0; c < 4; c++)
						element(i,j) += mt(i,c) * b(c,j);
			return *this;
		}    

		Matrix4 & multiplyleft( const Matrix4 & b )
		{
			Matrix4 mt(*this);
			value(Real(0));

			for(int i=0; i < 4; i++)
				for(int j=0; j < 4; j++)
					for(int c=0; c < 4; c++)
						element(i,j) += b(i,c) * mt(c,j);
			return *this;
		}

		// dst = M * src
		void multiplyMatrixVec( const Vec3 &src, Vec3 &dst ) const
		{
			Real w = (
				src.v[0] * element(3,0) +
				src.v[1] * element(3,1) + 
				src.v[2] * element(3,2) +
				element(3,3)          );

			assert(w != Constant::Zero);

			dst.v[0]  = (
				src.v[0] * element(0,0) +
				src.v[1] * element(0,1) +
				src.v[2] * element(0,2) +
				element(0,3)          ) / w;
			dst.v[1]  = (
				src.v[0] * element(1,0) +
				src.v[1] * element(1,1) +
				src.v[2] * element(1,2) +
				element(1,3)          ) / w;
			dst.v[2]  = (
				src.v[0] * element(2,0) +
				src.v[1] * element(2,1) + 
				src.v[2] * element(2,2) +
				element(2,3)          ) / w;
		}

		void multiplyMatrixVec( Vec3 & src_and_dst) const
		{ multiplyMatrixVec(Vec3(src_and_dst), src_and_dst); }


		// dst = src * M
		void multiplyVecMatrix( const Vec3 &src, Vec3 &dst ) const
		{
			Real w = (
				src.v[0] * element(0,3) +
				src.v[1] * element(1,3) +
				src.v[2] * element(2,3) +
				element(3,3)          );

			assert(w != Constant::Zero);

			dst.v[0]  = (
				src.v[0] * element(0,0) +
				src.v[1] * element(1,0) + 
				src.v[2] * element(2,0) + 
				element(3,0)          ) / w;
			dst.v[1]  = (
				src.v[0] * element(0,1) +
				src.v[1] * element(1,1) +
				src.v[2] * element(2,1) +
				element(3,1)          ) / w;
			dst.v[2]  = (
				src.v[0] * element(0,2) +
				src.v[1] * element(1,2) +
				src.v[2] * element(2,2) +
				element(3,2)          ) / w;
		}


		void multiplyVecMatrix( Vec3 & src_and_dst) const
		{ multiplyVecMatrix(Vec3(src_and_dst), src_and_dst); }

		// dst = M * src
		void multiplyMatrixVec( const Vec4 &src, Vec4 &dst ) const
		{
			dst.v[0]  = (
				src.v[0] * element(0,0) +
				src.v[1] * element(0,1) +
				src.v[2] * element(0,2) +
				src.v[3] * element(0,3));
			dst.v[1]  = (
				src.v[0] * element(1,0) +
				src.v[1] * element(1,1) +
				src.v[2] * element(1,2) +
				src.v[3] * element(1,3));
			dst.v[2]  = (
				src.v[0] * element(2,0) +
				src.v[1] * element(2,1) + 
				src.v[2] * element(2,2) +
				src.v[3] * element(2,3));
			dst.v[3] = (
				src.v[0] * element(3,0) +
				src.v[1] * element(3,1) + 
				src.v[2] * element(3,2) +
				src.v[3] * element(3,3));
		}

		void multiplyMatrixVec( Vec4 & src_and_dst) const
		{ multiplyMatrixVec(Vec4(src_and_dst), src_and_dst); }


		// dst = src * M
		void multiplyVecMatrix( const Vec4 &src, Vec4 &dst ) const
		{
			dst.v[0]  = (
				src.v[0] * element(0,0) +
				src.v[1] * element(1,0) + 
				src.v[2] * element(2,0) + 
				src.v[3] * element(3,0));
			dst.v[1]  = (
				src.v[0] * element(0,1) +
				src.v[1] * element(1,1) +
				src.v[2] * element(2,1) +
				src.v[3] * element(3,1));
			dst.v[2]  = (
				src.v[0] * element(0,2) +
				src.v[1] * element(1,2) +
				src.v[2] * element(2,2) +
				src.v[3] * element(3,2));
			dst.v[3] = (
				src.v[0] * element(0,3) +
				src.v[1] * element(1,3) +
				src.v[2] * element(2,3) +
				src.v[3] * element(3,3));
		}


		void multiplyVecMatrix( Vec4 & src_and_dst) const
		{ multiplyVecMatrix(Vec4(src_and_dst), src_and_dst); }


		// dst = M * src
		void multiplyMatrixdir( const Vec3 &src, Vec3 &dst ) const
		{
			dst.v[0]  = (
				src.v[0] * element(0,0) +
				src.v[1] * element(0,1) +
				src.v[2] * element(0,2) ) ;
			dst.v[1]  = ( 
				src.v[0] * element(1,0) +
				src.v[1] * element(1,1) +
				src.v[2] * element(1,2) ) ;
			dst.v[2]  = ( 
				src.v[0] * element(2,0) +
				src.v[1] * element(2,1) + 
				src.v[2] * element(2,2) ) ;
		}


		void multiplyMatrixdir( Vec3 & src_and_dst) const
		{ multiplyMatrixdir(Vec3(src_and_dst), src_and_dst); }


		// dst = src * M
		void multiplyDirMatrix( const Vec3 &src, Vec3 &dst ) const
		{
			dst.v[0]  = ( 
				src.v[0] * element(0,0) +
				src.v[1] * element(1,0) +
				src.v[2] * element(2,0) ) ;
			dst.v[1]  = ( 
				src.v[0] * element(0,1) +
				src.v[1] * element(1,1) +
				src.v[2] * element(2,1) ) ;
			dst.v[2]  = (
				src.v[0] * element(0,2) +
				src.v[1] * element(1,2) + 
				src.v[2] * element(2,2) ) ;
		}


		void multiplyDirMatrix( Vec3 & src_and_dst) const
		{ multiplyDirMatrix(Vec3(src_and_dst), src_and_dst); }


		Real & operator () (int row, int col)
		{ return element(row,col); }

		const Real & operator () (int row, int col) const
		{ return element(row,col); }

		Real & element (int row, int col)
		{ return m[row | (col<<2)]; }

		const Real & element (int row, int col) const
		{ return m[row | (col<<2)]; }

		Matrix4 & operator *= ( const Matrix4 & mat )
		{
			multiplyright( mat );
			return *this;
		}

		Matrix4 & operator *= ( const Real & r )
		{
			for (int i = 0; i < 4; ++i)
			{
				element(0,i) *= r;
				element(1,i) *= r;
				element(2,i) *= r;
				element(3,i) *= r;
			}
			return *this;
		}

		Matrix4 & operator += ( const Matrix4 & mat )
		{
			for (int i = 0; i < 4; ++i)
			{
				element(0,i) += mat.element(0,i);
				element(1,i) += mat.element(1,i);
				element(2,i) += mat.element(2,i);
				element(3,i) += mat.element(3,i);
			}
			return *this;
		}

		friend Matrix4 operator * ( const Matrix4 & m1,	const Matrix4 & m2 );
		friend bool operator == ( const Matrix4 & m1, const Matrix4 & m2 );
		friend bool operator != ( const Matrix4 & m1, const Matrix4 & m2 );

		//protected:
		Real m[16];
	};

	inline  
		Matrix4 operator * ( const Matrix4 & m1, const Matrix4 & m2 )
	{
		Matrix4 product;

		product = m1;
		product.multiplyright(m2);

		return product;
	}

	inline
		bool operator ==( const Matrix4 &m1, const Matrix4 &m2 )
	{
		return ( 
			m1(0,0) == m2(0,0) &&
			m1(0,1) == m2(0,1) &&
			m1(0,2) == m2(0,2) &&
			m1(0,3) == m2(0,3) &&
			m1(1,0) == m2(1,0) &&
			m1(1,1) == m2(1,1) &&
			m1(1,2) == m2(1,2) &&
			m1(1,3) == m2(1,3) &&
			m1(2,0) == m2(2,0) &&
			m1(2,1) == m2(2,1) &&
			m1(2,2) == m2(2,2) &&
			m1(2,3) == m2(2,3) &&
			m1(3,0) == m2(3,0) &&
			m1(3,1) == m2(3,1) &&
			m1(3,2) == m2(3,2) &&
			m1(3,3) == m2(3,3) );
	}

	inline
		bool operator != ( const Matrix4 & m1, const Matrix4 & m2 )
	{ return !( m1 == m2 ); }  
}