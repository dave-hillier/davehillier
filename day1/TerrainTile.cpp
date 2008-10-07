/*
 *  TerrainTile.cpp
 *  Day1
 *
 *  Created by Dave Hillier on 26/08/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TerrainTile.h"
#include <cassert>
#include <SDL.h>
#include "Frustum.h"
#include "Camera.h"

#define NO_SDL_GLEXT
#include <SDL_OpenGL.h>
#include <iostream>
#include <algorithm>
#include <limits>

#include "Timer.h"

extern int tileSize;
namespace
{
	void buildMesh(const vector3 &corner, const std::vector<float> &heights,
		const std::vector<vector3> &normals,
		unsigned stride, unsigned steps, 
		std::vector<vector3>& verts, std::vector<vector3>& norms, float scale,
		bbox3 &bounds)
	{		
		float minHeight = std::numeric_limits<float>::max();
		float maxHeight = std::numeric_limits<float>::min();
		// TODO change this to be using indicies and triangle strips 
		float vscale = 0.1;
		for (unsigned z = stride; z < steps-stride*2; z += stride)
		{
			for (unsigned x = stride; x < steps-stride*2; x += stride)
			{
				unsigned index1 = z * steps + x;
				unsigned index2 = (z+stride) * steps + x;
				unsigned index3 = z * steps + x+stride;
				unsigned index4 = (z+stride) * steps + x+stride;
								
				verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index4]+corner.y, corner.z+(z+stride)*scale));
				verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
				verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
				verts.push_back(vector3(corner.x+x*scale, vscale*heights[index1]+corner.y, corner.z+z*scale));
				verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
				verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
				
				// Same order as verts
				norms.push_back(normals[index4]);
				norms.push_back(normals[index2]);
				norms.push_back(normals[index3]);
				norms.push_back(normals[index1]);
				norms.push_back(normals[index3]);
				norms.push_back(normals[index2]);
				
				maxHeight = std::max(maxHeight, vscale*heights[index1]+corner.y);
				minHeight = std::min(minHeight, vscale*heights[index1]+corner.y);

			}
		}
		for (unsigned z = 0; z < steps-stride; z += stride)
		{
			unsigned x = 0;
			unsigned index1 = z * steps + x;
			unsigned index2 = (z+stride) * steps + x;
			unsigned index3 = z * steps + x+stride;
			unsigned index4 = (z+stride) * steps + x+stride;
							
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index4]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index1]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			
			// Same order as verts
			norms.push_back(normals[index4]);
			norms.push_back(normals[index2]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index1]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index2]);	
			
			maxHeight = std::max(maxHeight, vscale*heights[index1]+corner.y);
			minHeight = std::min(minHeight, vscale*heights[index1]+corner.y);

			x = steps-(stride)-1;
			index1 = z * steps + x;
			index2 = (z+stride) * steps + x;
			index3 = z * steps + x+stride;
			index4 = (z+stride) * steps + x+stride;
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index4]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index1]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			
			// Same order as verts
			norms.push_back(normals[index4]);
			norms.push_back(normals[index2]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index1]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index2]);	

			maxHeight = std::max(maxHeight, vscale*heights[index1]+corner.y);
			minHeight = std::min(minHeight, vscale*heights[index1]+corner.y);
		}
		for (unsigned x = 0; x < steps-stride; x += stride)
		{
			unsigned z = 0;
			unsigned index1 = z * steps + x;
			unsigned index2 = (z+stride) * steps + x;
			unsigned index3 = z * steps + x+stride;
			unsigned index4 = (z+stride) * steps + x+stride;
							
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index4]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index1]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			
			// Same order as verts
			norms.push_back(normals[index4]);
			norms.push_back(normals[index2]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index1]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index2]);	
			maxHeight = std::max(maxHeight, vscale*heights[index1]+corner.y);
			minHeight = std::min(minHeight, vscale*heights[index1]+corner.y);
			
			z = steps-(stride)-1;
			index1 = z * steps + x;
			index2 = (z+stride) * steps + x;
			index3 = z * steps + x+stride;
			index4 = (z+stride) * steps + x+stride;
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index4]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index1]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+(x+stride)*scale, vscale*heights[index3]+corner.y, corner.z+z*scale));
			verts.push_back(vector3(corner.x+x*scale, vscale*heights[index2]+corner.y, corner.z+(z+stride)*scale));
			
			// Same order as verts
			norms.push_back(normals[index4]);
			norms.push_back(normals[index2]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index1]);
			norms.push_back(normals[index3]);
			norms.push_back(normals[index2]);		
			maxHeight = std::max(maxHeight, vscale*heights[index1]+corner.y);
			minHeight = std::min(minHeight, vscale*heights[index1]+corner.y);
		}	
		
		bounds.vmax = corner+vector3(0, maxHeight, 0);
		bounds.vmin = corner+vector3(steps, minHeight, steps);
	}
}

TerrainTile::TerrainTile(const vector3 &br, const vector3 &tl, 
			const std::vector<float> &heights, 
			const std::vector<vector3>& normals) : 
			lod_(4)
{
	float scale = 1.0f;
	assert(heights.size() == normals.size());
	for (unsigned i = 0; (1 << i) < tileSize-1;  ++i)
	{
		LodData l;
		bbox3 tmp;
		bbox3& v = i == 0 ? bounds_ : tmp;
		buildMesh(br, heights, normals, 1 << i, tileSize, l.verts_, l.norms_, scale, 
			v);
		lods_.push_back(l);
	}
}

void TerrainTile::setLod(float distanceFromCamera)
{
	float c = distanceFromCamera / tileSize;
	lod_ = std::min((size_t)c, lods_.size()-1);
}

void TerrainTile::render(const Frustum &f)
{
	// TODO factor this bit out
	extern Camera cam;
	vector3 pos = cam.position();
	if (pos.distance(pos, bounds_.center()) > tileSize)
	{
		bool outside = false;
		for (int i = 0; i < 6; ++i)
		{
			vector3 p[] = {
				vector3( bounds_.vmax.x, bounds_.vmax.y, bounds_.vmax.z ),
				vector3( bounds_.vmax.x, bounds_.vmax.y, bounds_.vmin.z ),
				vector3( bounds_.vmax.x, bounds_.vmin.y, bounds_.vmax.z ),
				vector3( bounds_.vmax.x, bounds_.vmin.y, bounds_.vmin.z ),
				vector3( bounds_.vmin.x, bounds_.vmax.y, bounds_.vmax.z ),
				vector3( bounds_.vmin.x, bounds_.vmax.y, bounds_.vmin.z ),
				vector3( bounds_.vmin.x, bounds_.vmin.y, bounds_.vmax.z ),
				vector3( bounds_.vmin.x, bounds_.vmin.y, bounds_.vmin.z ),
			}; 
			outside = f.outside(p[i]);
			if (!outside)
				break;
		}
		if (outside)
		{
			return;
		}
	}
	
	
	// TODO use VBO
	const std::vector<vector3> *v = &lods_[lod_].verts_;
	const std::vector<vector3> *n = &lods_[lod_].norms_;
	/*
	float mcolor[][4] = {
		{ 0, 0, 1, 1.0f },
		{ 0, 1, 0, 1.0f },
		{ 0, 1, 1, 1.0f },
		{ 1, 0, 0, 1.0f },
		{ 1, 0, 1, 1.0f },
		{ 1, 1, 0, 1.0f },
		{ 1, 1, 1, 1.0f },
	};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor[lod]);
	*/

	glVertexPointer(3, GL_FLOAT, 0, (void*)&(*v)[0]);
	glNormalPointer(GL_FLOAT, 0, (void*)&(*n)[0]);
	glDrawArrays(GL_TRIANGLES, 0, v->size());

}


