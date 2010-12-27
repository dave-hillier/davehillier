// ms3d_model.h: interface for the ms3d_model class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __MS3D_MODEL__
#define __MS3D_MODEL__

typedef struct 
{
   	char id[10];
	int  version;
} ms3d_header;

/*id - Should always be MS3D000000 
version - Should always be 3 or 4 */

typedef struct 
{
	unsigned char	flags;
	unsigned char	refCount;
	char			boneID;
	float			vertex[3];
} ms3d_vertex;

typedef struct 
{
	unsigned short	flags;
	unsigned short	vertexIndices[3];
	float			vertexNormals[3][3];
	float			u[3];
	float			v[3];
	unsigned char	smoothingGroup;
	unsigned char	groupIndex;
	bool			dupe; ///this triangle is a duplicate. generally, ignore it.
} ms3d_triangle;

typedef struct 
{
   	unsigned char	flags;
   	char			name[32];
   	unsigned short	numTriangles;
	unsigned short*	triangleIndices;
	char			materialIndex;

} ms3d_group;

class ms3d_model  
{
public:
	ms3d_model();
	virtual ~ms3d_model();
	void Load (char *filename);
	void Render ();

	// do deform funcs too

private:
	unsigned short	numVertices;
	unsigned short	numTriangles;
	unsigned short	numGroups;
	unsigned short	numLines;

	ms3d_vertex		*vertices;
	ms3d_triangle	*triangles;
	ms3d_group		*groups;

	char			ident[128];
	int				texid;
	int				display_list_id;
};
#endif // __MS3D_MODEL__