// Terrain.h: interface for the Terrain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TERRAIN_H__
#define __TERRAIN_H__
typedef unsigned char byte;

#include "Vector.h"
#define PATCH_SIZE 17 // 33

typedef struct {
	
	byte max, min;		// bounds
	int x, z;			// origin corner
	int lod;			// Level of detail
	int omit_code;		// neighboring lod
	float blend;		// blend factor of level morphing 

	Bounds bounds;
} patch_t;
typedef enum { 
	TOP		=	1<<0,
		BOTTOM	=	1<<1,
		LEFT	=	1<<2,
		RIGHT	=	1<<3,
};

class Terrain  
{
public:
	Terrain();
	virtual ~Terrain();
	bool Init(char *name);
	
	float GetHeight(float x, float z);
	inline void SetHeight(Vector &v);
	void GetNormal(float x, float z, Vector *n);
	
	void Render(Vector pos);
	
	int width;
	int height;
	float y_scale;

	bool PickPoint(Vector *res, Vector start, Vector direction);
//	bool Line (Vector start, Vector end, Vector &hit);
private:
	int x_patches, z_patches; // Tile size?
	char name[128];
	bool wireframe;
	
	byte *data;	
	Vector *normal;
	patch_t *patch;
	
	int main_texture_id;
	
	void CalcNormals ();
	void CalcLOD (Vector pos);
	void RenderPatch (patch_t *patch);

//	bool TestBounds(Vector start, Vector dir, Bounds bb, float &tnear,float &tfar);

	bool PickExact(Vector *point, Bounds& bounds, int planeCheck, int x, int z);
	bool RayTerrain(Vector v0, Vector v1, Vector v2);

};
extern Terrain *terrain;

#endif //__TERRAIN__
