#pragma once

#include "maths/matrix.h"
#include "maths/plane.h"

class Frustum
{
public:
	Frustum(const matrix44 &projection, const matrix44 &modelView) 
	{
		update(projection, modelView);
	}
	~Frustum() {}

	void update(const matrix44 &projection, const matrix44 &modelView)
	{
		extractPlanes(planes_, modelView * projection);
	}

	bool outside(const vector3& point) const
	{
		bool result = false;
		for (unsigned int i = 0; i < 6; ++i)
		{
			float distance = planes_[i].distance(point);
			if (distance < 0)
			{
				result = true;
				break;
			}
		}
		return result;
	}

	float distanceToNearPlane(const vector3& point) { return planes_[4].distance(point); }

private:


	static void normalizePlane(plane &p)
	{
		float mag;
		mag = p.normal().len();
		p.a = p.a / mag;
		p.b = p.b / mag;
		p.c = p.c / mag;
		p.d = p.d / mag;
	}

	static void extractPlanes(plane *planes, const matrix44 & combinedView)
	{
		// Left clipping plane
		planes[0].a = combinedView.m[0][3] + combinedView.m[0][0];
		planes[0].b = combinedView.m[1][3] + combinedView.m[1][0];
		planes[0].c = combinedView.m[2][3] + combinedView.m[2][0];
		planes[0].d = combinedView.m[3][3] + combinedView.m[3][0];
		// Right clipping plane
		planes[1].a = combinedView.m[0][3] - combinedView.m[0][0];
		planes[1].b = combinedView.m[1][3] - combinedView.m[1][0];
		planes[1].c = combinedView.m[2][3] - combinedView.m[2][0];
		planes[1].d = combinedView.m[3][3] - combinedView.m[3][0];
		// Top clipping plane
		planes[2].a = combinedView.m[0][3] - combinedView.m[0][1];
		planes[2].b = combinedView.m[1][3] - combinedView.m[1][1];
		planes[2].c = combinedView.m[2][3] - combinedView.m[2][1];
		planes[2].d = combinedView.m[3][3] - combinedView.m[3][1];
		// Bottom clipping plane
		planes[3].a = combinedView.m[0][3] + combinedView.m[0][1];
		planes[3].b = combinedView.m[1][3] + combinedView.m[1][1];
		planes[3].c = combinedView.m[2][3] + combinedView.m[2][1];
		planes[3].d = combinedView.m[3][3] + combinedView.m[3][1];
		// Near clipping plane
		planes[4].a = combinedView.m[0][3] + combinedView.m[0][2];
		planes[4].b = combinedView.m[1][3] + combinedView.m[1][2];
		planes[4].c = combinedView.m[2][3] + combinedView.m[2][2];
		planes[4].d = combinedView.m[3][3] + combinedView.m[3][2];
		// Far clipping plane
		planes[5].a = combinedView.m[0][3] - combinedView.m[0][2];
		planes[5].b = combinedView.m[1][3] - combinedView.m[1][2];
		planes[5].c = combinedView.m[2][3] - combinedView.m[2][2];
		planes[5].d = combinedView.m[3][3] - combinedView.m[3][2];

		// Normalize the plane equations
		normalizePlane(planes[0]);
		normalizePlane(planes[1]);
		normalizePlane(planes[2]);
		normalizePlane(planes[3]);
		normalizePlane(planes[4]);
		normalizePlane(planes[5]);

	}

	plane planes_[6];
};