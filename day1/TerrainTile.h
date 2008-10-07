/*
 *  TerrainTile.h
 *  Day1
 *
 *  Created by Dave Hillier on 26/08/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef TerrainTileHeader
#define TerrainTileHeader

#include "bbox.h"
#include <vector>

class Frustum;

class TerrainTile 
{
public:
	TerrainTile(const vector3 &br, const vector3 &tl, 
				const std::vector<float> &heights, 
				const std::vector<vector3>& normals);
	
	void render(const Frustum &f);
	void setLod(float distanceFromCamera);

	
private:
	bbox3 bounds_;
	
	struct LodData
	{
		std::vector<vector3> verts_;
		std::vector<vector3> norms_;
	};
	std::vector<LodData> lods_;
	size_t lod_;
};

#endif