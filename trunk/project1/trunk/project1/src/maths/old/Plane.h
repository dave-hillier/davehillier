#pragma once


#include "Common.h"
#include "Vec.h"


namespace Maths
{

	class Plane
	{
	public:

		Plane()
		{
			planedistance = 0.0;
			planenormal.value( 0.0, 0.0, 1.0 );
		}


		Plane( const Vec3 &p0, const Vec3 &p1, const Vec3 &p2 )
		{
			Vec3 v0 = p1 - p0;
			Vec3 v1 = p2 - p0;
			planenormal = v0.cross(v1);  
			planenormal.normalize();
			planedistance = p0.dot(planenormal);
		}

		Plane( const Vec3 &normal, Real distance )
		{
			planedistance = distance;
			planenormal = normal;
			planenormal.normalize();
		}

		Plane( const Vec3 &normal, const Vec3 &point )
		{
			planenormal = normal;
			planenormal.normalize();
			planedistance = point.dot(planenormal);
		}

		void offset( Real d )
		{
			planedistance += d;
		}

		bool intersect( const Line &l, Vec3 &intersection ) const
		{
			Vec3 pos, dir;
			Vec3 pn = planenormal;
			Real pd = planedistance;

			pos = l.position();
			dir = l.direction();

			if(dir.dot(pn) == 0.0) return 0;
			pos -= pn*pd;
			// now we're talking about a plane passing through the origin
			if(pos.dot(pn) < 0.0) pn.negate();
			if(dir.dot(pn) > 0.0) dir.negate();
			Vec3 ppos = pn * pos.dot(pn);
			pos = (ppos.length()/dir.dot(-pn))*dir;
			intersection = l.position();
			intersection += pos;
			return 1;
		}
		void transform( const Matrix4 &Matrix )
		{
			Matrix4 invtr = matrix.inverse();
			invtr = invtr.transpose();

			Vec3 pntOnplane = planenormal * planedistance;
			Vec3 newPntOnplane;
			Vec3 newnormal;

			invtr.multiplyDirMatrix(planenormal, newnormal);
			matrix.multiplyVecMatrix(pntOnplane, newPntOnplane);

			newnormal.normalize();
			planenormal = newnormal;
			planedistance = newPntOnplane.dot(planenormal);
		}

		bool isInHalfSpace( const Vec3 &point ) const
		{

			if(( point.dot(planenormal) - planedistance) < 0.0)
				return 0;
			return 1;
		}


		Real distance( const Vec3 & point ) const 
		{
			return planenormal.dot(point - planenormal*planedistance);
		}

		const Vec3 &normal() const
		{
			return planenormal;
		}


		Real distanceFromOrigin() const
		{
			return planedistance;
		}


		friend bool operator == ( const Plane & p1, const Plane & p2 );


		friend bool operator != ( const Plane & p1, const Plane & p2 );

		//protected:
		Vec3 planenormal;
		Real planedistance;
	};

	inline
		bool operator == (const Plane & p1, const Plane & p2 )
	{
		return (  p1.planedistance == p2.planedistance && p1.planenormal == p2.planenormal);
	}

	inline
		bool operator != ( const Plane & p1, const Plane & p2 )
	{ return  ! (p1 == p2); }


}