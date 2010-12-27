// ase_model.h: interface for the ase_model class.
//
//////////////////////////////////////////////////////////////////////

// try to give all model class a simple interface
#ifndef __ASEMODEL__
#define __ASEMODEL__

#include <stdio.h>

typedef struct {
	char	name[128];
	int		*faces, *tfaces;
	int		numverts, numfaces, numtverts, numtfaces;
	float	*verts, *vnorms, *tverts;	
} ase_mesh_t;

class ase_model  
{
public:
	ase_model();
	virtual ~ase_model();

	void Load (char *file_name);
	void Render();

private:
	bool FindNextToken (FILE *f, const char *token, long from, long *start, long *end);
	void GetInfo (FILE *f, long start, long end,  int *num_verts, int *num_faces, int *num_tverts, int *num_tfaces);
	void GetVNorms (FILE *f, float *vnorms, long start, long end);
	void GetVerts (FILE *f, float *verts, long start, long end);
	void GetFaces (FILE *f, int *faces, long start, long end);
	void GetTVerts (FILE *f, float *tverts, long start, long end);
	void GetTFaces (FILE *f, int *tfaces, long start, long end);
	void GetName (FILE *f, char *name, long start, long end);
	
	ase_mesh_t *mesh;
	int no_meshes;
};
#endif // __ASEMODEL__