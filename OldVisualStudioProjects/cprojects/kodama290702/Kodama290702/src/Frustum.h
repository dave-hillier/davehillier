// Frustum.h: interface for the Frustum class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __FRUSTUM_H__
#include "Vector.h"
#include "plane.h"
#include "matrix.h"

class Frustum  
{

	Matrix clip;
	
public:

	Frustum();
	virtual ~Frustum();
	void Update ();

	int ClipTest4f (float p[4]);
	int ClipTest (Bounds bb);
};

extern Frustum *frustum;

#endif