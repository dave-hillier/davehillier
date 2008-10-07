/*
 *  TerrainMap.cpp
 *  Day1
 *
 *  Created by Dave Hillier on 27/08/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TerrainMap.h"
#include "TerrainTile.h"
#include "Camera.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <SDL.h>

#define NO_SDL_GLEXT
#include <SDL_OpenGL.h>

int tileSize = 65;
TerrainMap::TerrainMap(float range, MapGenerator* generator) : range_(range), 
	generator_(generator)
{

}

void TerrainMap::updateExistingTiles()
{
	extern Camera cam;
	vector3 camPos = cam.position();
	
	for(map::iterator itt = 
		tiles_.begin(); itt != tiles_.end(); ++itt)
	{
		Position p = itt->first;
		vector3 a(p.x+tileSize/2, 0, p.z+tileSize/2);
		vector3 b(camPos);
		float dist = vector3::distance(a, b);

		// look for expired tiles
		float r = range_+camPos.y;
		if (dist > r + tileSize)
		{
			// remove tile
			delete itt->second;
			tiles_.erase(itt);
		}
		else
		{
			itt->second->setLod(dist);
		}
	}	
}

void TerrainMap::searchForNewTiles()
{
	extern Camera cam;
	vector3 pos = cam.position();
		
	int pz = pos.z;
	int px = pos.x;
	float r = range_+pos.y;
	// Search for new tiles
	for (int z = -r+pz; z < r+pz; z += tileSize-1)
	{
		for (int x = -r+px; x < r+px; x += tileSize-1)
		{	
			int mx = x % (tileSize-1);
			int qx = x > 0 ? x - mx : x - mx - (tileSize-1);
			int mz = z % (tileSize-1);
			int qz = z > 0 ? z - mz : z - mz - (tileSize-1);
			vector3 a(qx+tileSize/2, 0, qz+tileSize/2);
			vector3 b(pos);
			float dist = vector3::distance(a, b);
			
			if (dist < r)
			{
				// ensure we have tile, if not request it.				
				Position p(qx, qz);
				map::iterator itt = tiles_.find(p);
				if (itt == tiles_.end())
				{
					if (requestedTiles_.end() == std::find(requestedTiles_.begin(), requestedTiles_.end(), p))
					{
						requestedTiles_.push_back(p);
					}
				}
			}
		}
	}
}

void TerrainMap::update()
{
	// Generate this tile, one per frame
	if (requestedTiles_.size() > 0)
	{
		Position p = requestedTiles_.back();
		requestedTiles_.pop_back();
		map::iterator itt = tiles_.find(p);
		if (itt == tiles_.end())
		{
			
			std::vector<vector3> normals;
			std::vector<float> heights;
			heights.reserve(tileSize*tileSize);
			normals.reserve(tileSize*tileSize);
			for (unsigned iz = 0; iz < tileSize; ++iz)
			{
				for (unsigned ix = 0; ix < tileSize; ++ix)
				{
					int x = ix + p.x;
					int z = iz + p.z;
					// TODO a caching scheme for this?
					float a = (*generator_)(x,z);
					heights.push_back(a);
					
					vector3 b = vector3(x+1, (*generator_)(x+1,z), z);
					vector3 c = vector3(x-1, (*generator_)(x-1,z), z);
					vector3 e = vector3(x, (*generator_)(x,z+1), z+1);
					vector3 f = vector3(x, (*generator_)(x,z-1), z-1);

					vector3 normal = (b - c) * (e - f); 
					normal.norm();
					normals.push_back(-normal);
				}
			}
			TerrainTile* terrain = new TerrainTile(
				vector3(p.x,-20,p.z), 
				vector3(p.x+tileSize-1,-20,p.z+tileSize-1), heights, normals);
					
			tiles_[p] = terrain;
		}
		else
		{
			std::cout << "Error tile already exists: (" << p.x << ", " << p.z << ")\n";
		}
	}
	
	updateExistingTiles();
	searchForNewTiles();
}

void TerrainMap::render(const Frustum &f)
{
	// TODO move the following to global settings
	glShadeModel(GL_SMOOTH);
	// Somewhere in the initialization part of your program…
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	GLfloat ambientLight[] = { 0.75f, 0.75f, 0.75f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat position[] = { 1.f, 2.0f, 1.0f, 0.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	for(map::const_iterator itt = 
		tiles_.begin(); itt != tiles_.end(); ++itt)
	{
		(*itt).second->render(f);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_LIGHTING);
	
}