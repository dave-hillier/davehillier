/*
 *  TerrainGenerator.h
 *  Day1
 *
 *  Created by Dave Hillier on 30/09/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef TerrainGenerator_h
#define TerrainGenerator_h

#include "TerrainMap.h"

class TerrainGenerator : public TerrainMap::MapGenerator
{
public:
	virtual float operator()(float x, float z) const;
};
	
#endif