// TODO: Remove tags
// texture management/linking
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VERSION 0x04
#define MAX_MESHES 64
typedef unsigned char byte;
typedef enum { false, true } bool;

typedef struct {

	char		tag[3];
	byte		version;
	int			meshes;

} mdh_header;

typedef struct {

	char		meshname[64];
	char		texture[64];

	int			numfaces;
	int			numverts;
	int			numtverts;

} mdh_subheader;
typedef struct {

	char	texture[64], name[64];
	int		*faces, num_faces, num_verts;
	float	*verts, *norms, *tverts;

} mdh_tempholder;


// Find next mesh etc 
bool FindNextToken (FILE *f, const char *token, long from, long *start, long *end)
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

void GetInfo (FILE *f, long start, long end, 
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

void GetVerts (FILE *f, float *verts, long start, long end)
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
void GetVNorms (FILE *f, float *vnorms, long start, long end)
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
void GetTVerts (FILE *f, float *tverts, long start, long end)
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
void GetFaces (FILE *f, int *faces, long start, long end)
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

void GetTFaces (FILE *f, int *tfaces, long start, long end)
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

void GetName (FILE *f, char *name, long start, long end)
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
void debug1 (FILE *f, long start, long end)
{
	FILE *fout = fopen("debug.log", "w");
	char data[255];

	fseek (f, start, SEEK_SET);

	while (!feof(f) && (ftell(f) < end))
	{
		fscanf(f, "%s", data);
		fprintf(fout, "%s\n", data);
	}
	fclose(fout);
}

void WriteDH4 (FILE *f, int num_meshes, mdh_tempholder temp[])
{
	int i;
	mdh_header header;
	
	header.meshes = num_meshes;
	header.version = VERSION;

	strcpy (header.tag, "MDH");
	fwrite(&header, sizeof (mdh_header), 1, f);

	for (i = 0; i < num_meshes; i++)
	{
		mdh_subheader subheader;

		strcpy (subheader.meshname, temp[i].name);
		strcpy (subheader.texture, temp[i].texture);

		subheader.numfaces = temp[i].num_faces;
		subheader.numverts = temp[i].num_verts;
		if (temp[i].tverts)
			subheader.numtverts = temp[i].num_verts;
		else
			subheader.numtverts = 0;
					
		fwrite(&subheader, sizeof (mdh_subheader), 1, f);
		fwrite(temp[i].faces, sizeof(int), temp[i].num_faces * 3, f);
		fwrite(temp[i].verts, sizeof(float), temp[i].num_verts * 3, f);
		fwrite(temp[i].norms, sizeof(float), temp[i].num_verts * 3, f);
		if (temp[i].tverts)
			fwrite(temp[i].tverts, sizeof(float), temp[i].num_verts * 2, f);
	}
}

void main ()
{
	char filename[64];
	FILE *f;
	int meshes=0;
	long start=0, end=0, last=0;
	mdh_tempholder temp[MAX_MESHES];

	printf("WARNING TEXTURE NAMES LACK EXTENSTION!\nASE -> DH Convert v%d\n\n", VERSION);

	printf ("Enter File name : ");
	scanf("%s", filename);

	if (f = fopen(filename, "r"))
	{
		while (FindNextToken(f, "*GEOMOBJECT", last, &start, &end))
		{
			char	str[64], meshname[64];
			char	texture[64];
			int		*faces, *tfaces;
			float	*verts, *vnorms, *tverts = NULL;
			int		numverts=0, numfaces=0, numtverts=0, numtfaces=0;
			int		mesh_no, s, e;
			// maybe get texture
			mesh_no = meshes++;

			
			GetName(f, str, start, end);
			str[strlen(str)-1] = 0;
			sscanf(str, "\"%s", meshname);
			// TODO : if tag remove! and add tag vectors at end of file
			//sprintf(texture, "%s.nfo", meshname);
			strcpy(texture, meshname);
			printf("Reading node %s.\n", meshname);

			GetInfo(f, start, end, 
				&numverts, &numfaces, &numtverts, &numtfaces);
			// Allocate
			faces = (int *)malloc (numfaces * 3 * sizeof(int));
			verts =	(float *)malloc (numverts * 3 * sizeof(float));
			vnorms = (float *)malloc (numverts * 3 * sizeof(float));
			if (numtverts)
				tverts = (float *)malloc (numtverts * 3 * sizeof(float));
			if (numtfaces)
				tfaces = (int *)malloc (numtfaces * 3 * sizeof(int));

			// get the data
			printf("MESH_VERTEX_LIST ");
			FindNextToken(f, "*MESH_VERTEX_LIST", start, &s, &e);
			GetVerts(f, verts, s, e);

			printf("MESH_FACE_LIST ");
			FindNextToken(f, "*MESH_FACE_LIST", start, &s, &e);
			GetFaces(f, faces, s, e);

			printf("MESH_NORMALS ");
			FindNextToken(f, "*MESH_NORMALS", start, &s, &e);
			GetVNorms(f, vnorms, s, e);

			// Only these are optional
			if (numtverts) {
				printf("MESH_TVERTLIST ");
				FindNextToken(f, "*MESH_TVERTLIST", start, &s, &e);
				GetTVerts(f, tverts, s, e);
			}
			if (numtfaces) {
				printf("MESH_TFACELIST ");
				FindNextToken(f, "*MESH_TFACELIST", start, &s, &e);
				GetTFaces(f, tfaces, s, e);	
			}	
			printf("\n");
			rewind(f);
			last = end;
			
			// store them for a mo
			strcpy (temp[mesh_no].name, meshname);
			strcpy (temp[mesh_no].texture, texture);
			
			temp[mesh_no].num_faces = numfaces;
			temp[mesh_no].num_verts = numverts;

			temp[mesh_no].faces = faces;
			temp[mesh_no].verts = verts;
			temp[mesh_no].norms = vnorms;
			if (numtverts)
				temp[mesh_no].tverts = tverts;
			else
				temp[mesh_no].tverts = NULL;
			// Dunno if i need t verts
		}

		fclose(f);
		
		filename[strlen(filename)-3] = 0;
		strcat(filename, "dh4");
		if (f = fopen(filename, "wb"))
		{
			int i;
			WriteDH4(f, meshes, temp);
			fclose(f);

			for (i = 0; i < meshes; i++)
			{
				free (temp[i].faces);
				free (temp[i].verts);
				free (temp[i].norms);
				if (temp[i].tverts)
					free (temp[i].tverts);
			}

		}
	}
}
