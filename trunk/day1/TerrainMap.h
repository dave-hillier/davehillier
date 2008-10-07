/*
 *  TerrainMap.h
 *  Day1
 *
 *  Created by Dave Hillier on 27/08/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TerrainMapHeader
#define TerrainMapHeader

//#include <map>
#include <tr1/unordered_map>

#include <vector>
#include <set>

class Frustum;
class TerrainTile;
class _vector3;
typedef _vector3 vector3;

class TerrainMap
{
public:
	 class MapGenerator
	 {
	 public:
		MapGenerator() {}
		virtual ~MapGenerator() {}
		virtual float operator()(float x, float z) const = 0;
	 };

	 TerrainMap(float range, MapGenerator* generator);
	 
	 void update();
	 void render(const Frustum &f);
	 
	 
private:
	void updateExistingTiles();
	void searchForNewTiles();

	struct Position
	{
		Position(int x, int z) : x(x), z(z) {}
		int x;
		int z;
		bool operator==(const Position& a) const
		{
			return a.x == x && a.z == z;
		}
		bool operator<(const Position& a) const
		{
			if (x < a.x)
				return true;
			else if (x == a.x && z < a.z)
				return true;
			else
				return false;
		}
	};
	
	struct PositionHash
	{
		int operator()(const Position& a) const
		{
			return (int)a.x + ((int)a.z << 16);
		}
	};

	

	MapGenerator* generator_;
	//typedef std::map<Position, TerrainTile*> map;
	typedef std::tr1::unordered_map<Position, TerrainTile*, PositionHash> map;
	map tiles_;
	std::vector<Position> requestedTiles_;
	float range_;
};

#endif