#pragma once
#include "Common.h"
#include "plane.h"

class Frustum
{
public:
	Frustum(){}
	~Frustum(){}

	void update(const matrix44 &v) 
	{
		// TODO
		// Create the planes from the matrix
		plane l(  v[1][1]+v[4][1],  v[1][2]+v[4][2],  v[1][3]+v[4][3],  v[1][4]+v[4][4] );
		plane r( -v[1][1]+v[4][1], -v[1][2]+v[4][2], -v[1][3]+v[4][3], -v[1][4]+v[4][4] );
		plane b(  v[2][1]+v[4][1],  v[2][2]+v[4][2],  v[2][3]+v[4][3],  v[2][4]+v[4][4] );
		plane t( -v[2][1]+v[4][1], -v[2][2]+v[4][2], -v[2][3]+v[4][3], -v[2][4]+v[4][4] );
		plane n(  v[3][1]+v[4][1],  v[3][2]+v[4][2],  v[3][3]+v[4][3],  v[3][4]+v[4][4] );
		plane f( -v[3][1]+v[4][1], -v[3][2]+v[4][2], -v[3][3]+v[4][3], -v[3][4]+v[4][4] );
	}



}