// Ribbon.h: interface for the Ribbon class.
//
// These should be trail like things for lasers etc when particles 
// arent appropriate 
//
//////////////////////////////////////////////////////////////////////
#ifndef __RIBBON__
#define __RIBBON__

#include "mathlib.h"
using namespace platformMath;

class RibbonPoint 
{
public:
	RibbonPoint();
	virtual ~RibbonPoint() {};

	float time_stamp;
	Vector3f pt1, pt2;

	RibbonPoint *next, *prev; // list order is important!

};

// ribbons should be attached to particular objects and
// probably last the entire life time
class Ribbon
{
public:
	Ribbon();
	virtual ~Ribbon();

	RibbonPoint *head;

	float colour_start[4];
	float colour_end[4];
	float life;

	void Update ();
	void Render ();
	void AddPoint (Vector3f pt1, Vector3f pt2);
};

#endif