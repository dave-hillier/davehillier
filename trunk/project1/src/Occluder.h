#pragma once

// TODO UNIT TEST
// TODO move implementation to Cpp
#include "maths/plane.h"
#include "maths/vector.h"
#include "maths/quaternion.h"

#include <vector>
#include <cassert>

// TODO maybe factor out the common stuff for use in portals?
// TODO rotation, can probably do like I've done the eye pos
class Occluder	
{
public:
	Occluder(const vector3 &position, quaternion rotate, const std::vector<vector3> &points) : 
	  polygon_(points), theOccluder_(points[0], points[1], points[2]), position_(position), orientation_(rotate)
	{
		assert(points.size() >= 3);
	}

	vector3 transformedEyePoint(const vector3 &eye) const
	{
		quaternion q = orientation_;
		//q.invert();
		vector3 p1 = q.rotate(eye - position_);
		return p1;
	}

	// This is a relative value, how much influence the occluder has on the scene
	float influence(const vector3 &eye) const
	{
		vector3 p1 = transformedEyePoint(eye);
		
		const float distance = theOccluder_.distance(p1);					// Distance from the camera
		vector3 forward = ((polygon_[0] + polygon_[1] + polygon_[2]) / 3) - p1;
		forward.norm();
		const float cosAngle = -theOccluder_.normal().dot(forward);				// Are we facing the camera	
		const vector3 cross = (polygon_[0] - polygon_[1]) * (polygon_[2] - polygon_[1]);
		const float areaApproximation = cross.lensquared();
		return cosAngle * areaApproximation / (distance+1);
	}

	void position(const vector3 &p) { position_ = p; }

	void update(const vector3 &eye)
	{
		vector3 p1 = transformedEyePoint(eye);
		sides_.resize(polygon_.size());
		std::vector<plane>::iterator p = sides_.begin();
		for (std::vector<vector3>::const_iterator itt = polygon_.begin(); itt != polygon_.end(); ++itt, ++p)
		{
			std::vector<vector3>::const_iterator next = itt+1;
			if (next == polygon_.end())
				next = polygon_.begin(); 

			*p = plane(p1, *itt, *next);
		}
	}

	// Opposite to the frustum, outside this open frustum is what we want!

	bool outside(const vector3 &point) const 
	{
		return outside(point, 0);
	}
	bool outside(const vector3 &point, float radius) const 
	{
		vector3 pointToTest = transformedEyePoint(point);
		bool result = false;

		if (theOccluder_.distance(point) > -radius)
			return true;

		for (std::vector<plane>::const_iterator itt = sides_.begin(); 
			itt != sides_.end(); ++itt)
		{
			float distance = (*itt).distance(pointToTest);
			if (distance > -radius)
			{
				result = true;
				break;
			}
		}
		return result;
	}
private:

	plane theOccluder_;
	std::vector<plane> sides_;
	std::vector<vector3> polygon_;
	vector3 position_;
	quaternion orientation_;
};