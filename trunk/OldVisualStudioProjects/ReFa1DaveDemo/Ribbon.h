// Ribbon.h: interface for the Ribbon class.
//
// These should be trail like things for lasers etc when particles 
// arent appropriate 
//
//////////////////////////////////////////////////////////////////////
#ifndef __RIBBON__
#define __RIBBON__

#include "mathlib.h"
#include "Vector3.h"
#include <deque>


// ribbons should be attached to particular objects and
// probably last the entire life time
class Ribbon
{
public:

	Ribbon();
	virtual ~Ribbon();

	class Point 
	{
	public:
		Point(const Vector3f &p1, const Vector3f &p2);
		virtual ~Point() {};

		bool operator <(Point &rhs) const { return time_stamp < rhs.time_stamp; }

		float time_stamp;
		Vector3f pt1, pt2;
	};
	std::deque<Point> points;

	float colour_start[4];
	float colour_end[4];
	float life;

	void Update ();
	void Render ();
	void AddPoint (Vector3f pt1, Vector3f pt2);
};

#endif