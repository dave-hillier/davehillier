// ase_model.cpp: implementation of the ase_model class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include "ase_model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ase_model::ase_model()
{

}

ase_model::~ase_model()
{

}


// Find next mesh etc 
bool ase_model::FindNextToken (FILE *f, const char *token, long from, long *start, long *end)
{
	char data[255];
	int open=0, close=0;
	bool found = false;
	int i;

	fseek(f, from, SEEK_SET);
	while (!feof(f))
	{  
		fscanf(f, "%s", &data);

		if (strcmp (token, data) == 0)
		{
			*start = ftell(f) - strlen(token);
			found = true;
			break;
		}
	}

	if (!found)
		return false;

	fscanf(f, "%s", &data);
	if (strcmp ("{", data) == 0)
		open++;

	

	while ((open != close) && (i = fscanf(f, "%s", &data)))
	{
		if (strcmp ("{", data) == 0)
			open++;
		else if (strcmp ("}", data) == 0)
			close++;
	} 
	*end = ftell(f);
	fseek (f, (*start), SEEK_SET);
	return found;
}

void ase_model::GetInfo (FILE *f, long start, long end, 
				  int *num_verts, 
				  int *num_faces, 
				  int *num_tverts,
				  int *num_tfaces)
{
	char data[255];

	fseek (f, start, SEEK_SET);

	while (ftell(f) < end) 
	{
		fscanf(f, "%s", data);
		if (strcmp ("*MESH_NUMVERTEX", data) == 0)
			fscanf(f, "%d", num_verts);
		else if (strcmp ("*MESH_NUMFACES", data) == 0)
			fscanf(f, "%d", num_faces);
		else if (strcmp ("*MESH_NUMTVERTEX", data) == 0)
			fscanf(f, "%d", num_tverts);
		else if (strcmp ("*MESH_NUMTVFACES", data) == 0)
			fscanf(f, "%d", num_tfaces);
	}
}

void ase_model::GetVerts (FILE *f, float *verts, long start, long end)
{
	int index, i=0;
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", &data);
		
		if (strcmp ("*MESH_VERTEX", data) == 0)
		{
			fscanf(f, "%d", &index);
			fscanf(f, "%f %f %f", 
				&verts[index * 3 + 0], 
				&verts[index * 3 + 2], 
				&verts[index * 3 + 1]);

			verts[index * 3 + 2] *= -1;
		}
	}
}
void ase_model::GetVNorms (FILE *f, float *vnorms, long start, long end)
{
	int index, i=0;
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", &data);
		if (strcmp ("*MESH_VERTEXNORMAL", data) == 0)
		{
			fscanf(f, "%d", &index);
			fscanf(f, "%f %f %f", 
				&vnorms[index * 3 + 0], 
				&vnorms[index * 3 + 1],
				&vnorms[index * 3 + 2]);
		}
	}
}
void ase_model::GetTVerts (FILE *f, float *tverts, long start, long end)
{
	int index , i=0;
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", &data);
		if (strcmp ("*MESH_TVERT", data) == 0)
		{
			fscanf(f, "%d", &index);
			fscanf(f, "%f %f", 
				&tverts[index * 2 + 0], 
				&tverts[index * 2 + 1]);
			tverts[index * 2 + 1] = 1 - tverts[index * 2 + 1];
		}
	}
}
void ase_model::GetFaces (FILE *f, int *faces, long start, long end)
{
	int index, i=0;
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", &data);
		if (strcmp ("*MESH_FACE", data) == 0)
		{
			fscanf(f, "%d:", &index);
			fscanf(f, " A: %d B: %d C: %d", 
				&faces[3 * index + 0],
				&faces[3 * index + 1], 
				&faces[3 * index + 2]);
		}
	}
}

void ase_model::GetTFaces (FILE *f, int *tfaces, long start, long end)
{
	int index, i=0;
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", &data);
		if (strcmp ("*MESH_TFACE", data) == 0)
		{
			fscanf(f, "%d", &index);
			fscanf(f, "%d %d %d", 
				&tfaces[index * 3 + 0], 
				&tfaces[index * 3 + 1],
				&tfaces[index * 3 + 2]);
		}
	}
}

void ase_model::GetName (FILE *f, char *name, long start, long end)
{
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", data);
		if (strcmp ("*NODE_NAME", data) == 0)
		{
			fscanf(f, "%s", name);
			return;
		}
	}
}

void ase_model::Render ()
{
	for (int i = 0; i < no_meshes; i++)
	{
		//glBindTexture(GL_TEXTURE_2D, mesh[i].texid);
		glNormalPointer(GL_FLOAT, 0, mesh[i].vnorms);
		glVertexPointer(3, GL_FLOAT, 0, mesh[i].verts);		
		glTexCoordPointer(2, GL_FLOAT, 0, mesh[i].tverts);
		glDrawElements(GL_TRIANGLES, mesh[i].numfaces * 3, GL_UNSIGNED_INT, mesh[i].faces);
	}
}


void ase_model::Load(char *file_name)
{

	long start = 0, end = 0, last = 0;
	FILE *f;

	if (f = fopen(file_name, "r"))
	{
		int		mesh_no = 0;
		no_meshes = 0;
		while (FindNextToken(f, "*GEOMOBJECT", last, &start, &end))
		{
			no_meshes++;
			last = end;
		}

		rewind(f);
		mesh = new ase_mesh_t [no_meshes];
		start = 0; 
		end = 0; 
		last = 0;

		while (FindNextToken(f, "*GEOMOBJECT", last, &start, &end))
		{
			char	str[64];
			long	s, e;
			// maybe get texture

			
			GetName(f, str, start, end);
			str[strlen(str)-1] = 0;
	//		sscanf(str, "\"%s", mesh->name);
			strcpy(mesh[mesh_no].name, str+1);

			//printf("Reading node %s.\n", meshname);

			GetInfo(f, start, end, 
				&mesh[mesh_no].numverts, &mesh[mesh_no].numfaces, 
				&mesh[mesh_no].numtverts, &mesh[mesh_no].numtfaces);
			// Allocate
			mesh[mesh_no].faces = new int[mesh[mesh_no].numfaces * 3];
			mesh[mesh_no].verts = new float[mesh[mesh_no].numverts * 3];
			mesh[mesh_no].vnorms = new float[mesh[mesh_no].numverts * 3];
			if (mesh[mesh_no].numtverts)
				mesh[mesh_no].tverts = new float[mesh[mesh_no].numtverts * 3];
			if (mesh[mesh_no].numtfaces)
				mesh[mesh_no].tfaces = new int[mesh[mesh_no].numtfaces * 3];

			// get the data
			//printf("MESH_VERTEX_LIST ");
			FindNextToken(f, "*MESH_VERTEX_LIST", start, &s, &e);
			GetVerts(f, mesh[mesh_no].verts, s, e);

			//printf("MESH_FACE_LIST ");
			FindNextToken(f, "*MESH_FACE_LIST", start, &s, &e);
			GetFaces(f, mesh[mesh_no].faces, s, e);

			//printf("MESH_NORMALS ");
			FindNextToken(f, "*MESH_NORMALS", start, &s, &e);
			GetVNorms(f, mesh[mesh_no].vnorms, s, e);

			// Only these are optional
			if (mesh[mesh_no].numtverts) {
			//	printf("MESH_TVERTLIST ");
				FindNextToken(f, "*MESH_TVERTLIST", start, &s, &e);
				GetTVerts(f, mesh[mesh_no].tverts, s, e);
			}
			if (mesh[mesh_no].numtfaces) {
			//	printf("MESH_TFACELIST ");
				FindNextToken(f, "*MESH_TFACELIST", start, &s, &e);
				GetTFaces(f, mesh[mesh_no].tfaces, s, e);	
			}	
			//printf("\n");
			rewind(f);
			last = end;
			mesh_no++;
			
		}

		fclose(f);
	}

}