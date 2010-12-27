// Terrain.h: interface for the Terrain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TERRAIN__
#define __TERRAIN__

#include "types.h"
#include "mathlib.h"
#include "Vector3.h"

enum {
	PATCH_SIZE = 33
};  //17



enum { 
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
	inline void SetHeight(Vector3f &v);
	void GetNormal(float x, float z, Vector3f *n);
	
	void Render(Vector3f pos);
	
	int width;
	int height;
	float y_scale;

	bool PickPoint(Vector3f *res, Vector3f start, Vector3f direction);
//	bool Line (Vector3f start, Vector3f end, Vector3f &hit);

private:
	class Patch {
	public:
		byte max, min;		// bounds
		int x, z;			// origin corner
		int lod;			// Level of detail
		int omit_code;		// neighboring lod
		float blend;		// blend factor of level morphing 

		Bounds bounds;
	};
	int x_patches, z_patches; // Tile size?
	char name[128];
	bool wireframe, reflection;
	
	byte *data;	
	Vector3f *normal;
	Patch *patch;
	
	unsigned int main_texture_id, detail_texture_id;
	
	void CalcNormals ();
	void CalcLOD (Vector3f pos);
	void RenderPatch (Patch *patch);

//	bool TestBounds(Vector3f start, Vector3f dir, Bounds bb, float &tnear,float &tfar);

	bool PickExact(Vector3f *point, Bounds& bounds, int planeCheck, int x, int z);
	bool RayTerrain(Vector3f v0, Vector3f v1, Vector3f v2);

};
extern Terrain terrain;

#endif //__TERRAIN__
