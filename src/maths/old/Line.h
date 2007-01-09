#pragma once


#include "Common.h"
#include "Vec.h"

// TODO Unit Tests
 
namespace Maths 
{
	class Line
	{
	public:

		Line()
		{ value(Vec3(0,0,0),Vec3(0,0,1)); }

		Line( const Vec3 & p0, const Vec3 &p1)
		{ value(p0,p1); }

		void value( const Vec3 &p0, const Vec3 &p1)
		{
			position = p0;
			direction = p1-p0;
			direction.normalize();
		}

		bool closestPoints(const Line &Line2, 
			Vec3 &pointOnThis,
			Vec3 &pointOnThat)
		{

			// quick check to see if parallel -- if so, quit.
			if(fabs(direction.dot(Line2.direction)) == 1.0)
				return 0;
			Line l2 = Line2;

			// Algorithm: Brian Jean
			// 

			Vec3 Vr = direction;
			Vec3 Vs = l2.direction;
			Real Vr_Dot_Vs = Vr.dot(Vs);
			Real detA = Real(1.0 - (Vr_Dot_Vs * Vr_Dot_Vs));
			Vec3 C = l2.position - position;
			Real C_Dot_Vr =  C.dot(Vr);
			Real C_Dot_Vs =  C.dot(Vs);

			Real u = (C_Dot_Vr - Vr_Dot_Vs * C_Dot_Vs)/detA;
			Real v = (C_Dot_Vr * Vr_Dot_Vs - C_Dot_Vs)/detA;

			pointOnThis = position;
			pointOnThis += direction * u;
			pointOnThat = l2.position;
			pointOnThat += l2.direction * v;

			return 1;
		}

		Vec3 closestPoint(const Vec3 &point)
		{
			Vec3 np = point - position;
			Vec3 rp = direction*direction.dot(np)+position;
			return rp;
		}

		const Vec3 & position() const {return position;}

		const Vec3 & direction() const {return direction;}

		//protected:
		Vec3 position;
		Vec3 direction;
	};

}