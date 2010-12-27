//---------------------------------------------------------------------------
#ifndef __PLANE_H__
#define __PLANE_H__
//---------------------------------------------------------------------------
#include "vector.h"
//---------------------------------------------------------------------------
class Plane
{
public:
	Vector n;
	double	d;

	Plane(float a=1, float b=0, float c=0, float d=0) : n(a, b, c), d(d) { }
	Plane(Vector& normal, double d=0) : n(normal), d(d) { }
	Plane(Plane& plane) : n(plane.n), d(plane.d) { }

	Plane(Vector &va,Vector &vb,Vector &vc)
	{
		Vector a = (vc - va); a.Normalise();
		Vector b = (vc - vb); b.Normalise();
		n = a % b;
		n.Normalise();
		d = - va.Dot(n);
	}	

	Plane& operator = (Plane& plane)
	{	n = plane.n;
		d = plane.d;
		return *this;
	}

	//	Boolean Operators
	bool operator == (Plane& plane) 
	{	return n == plane.n && d == plane.d;
	}
	bool operator != (Plane& plane) 
	{	return !(*this == plane);
	}

	//	Move plane
	Plane operator + (Vector& vector) 
	{	return Plane(n, d + vector.Dot(n));
	}
	Plane& operator += (Vector& vector)
	{	return *this = *this + vector;
	}
	Plane operator - (Vector& vector) 
	{	return Plane(n, d - vector.Dot(n));
	}
	Plane& operator -= (Vector& vector)
	{	return *this = *this - vector;
	}

	//	Just your average plane functions
	bool inline PointOnPlane(Vector& point) 
	{
		return DistanceToPlane(point) == 0;
	}
	float inline DistanceToPlane(Vector& point) 
	{
		return (float)(point.Dot(n) + d);
	}
	Vector inline RayIntersection(Vector& RayPos, Vector& RayDir) 
	{
		float a = RayDir.Dot(n);
		if (a == 0) 
			return RayPos;	//error line parallel to plane
		
		return RayPos - RayDir * (DistanceToPlane(RayPos) / a);
	}
};
//---------------------------------------------------------------------------
#endif	//__PLANE_H__