#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

	char		tag[3];
	unsigned char version;
	char		texture[64];

	unsigned int numfaces;
	unsigned int numverts;
} mdh_header;


#define VERTEX_LIST			"*MESH_VERTEX_LIST"
#define FACE_LIST			"*MESH_FACE_LIST"
#define UTILE				"*UVW_U_TILING"
#define VTILE				"*UVW_V_TILING"
#define UOFFSET				"*UVW_U_OFFSET"
#define VOFFSET				"*UVW_V_OFFSET"


#define VERSION				0x03

typedef enum { false , true } bool;

unsigned int	*faces;
float			*verts;
float			*vnorms;

float			*tverts;
unsigned int	*tfaces;

unsigned int	num_faces;
unsigned int	num_verts;
unsigned int	num_tfaces;
unsigned int	num_tverts;

char			texture[64];

void GetInfo (FILE *ase_file)
{
	char data[255];

	printf("\nGetting info.\n");
	rewind(ase_file);

	while (!feof(ase_file))
	{  
		fscanf(ase_file, "%s", &data);

		if (strcmp ("*COMMENT", data) == 0)
		{
			fgets(data, 255, ase_file);
			printf("Comments: %s", data);
		}
		else if (strcmp ("*MESH_NUMVERTEX", data) == 0)
			fscanf(ase_file, "%d", &num_verts);
		else if (strcmp ("*MESH_NUMFACES", data) == 0)
			fscanf(ase_file, "%d", &num_faces);
		else if (strcmp ("*MESH_NUMTVERTEX", data) == 0)
			fscanf(ase_file, "%d", &num_tverts);
		else if (strcmp ("*MESH_NUMTVFACES", data) == 0)
			fscanf(ase_file, "%d", &num_tfaces);
		else if (strcmp ("*BITMAP", data) == 0)
			fscanf(ase_file, "%s", &texture);
	}
	
	printf ("Vertexes %d\n", num_verts);
	printf ("Faces %d\n", num_faces);
	printf ("Texture Vertexes %d\n", num_tverts);
	printf ("Texture faces %d\n\n", num_tfaces);
}


void GetData (FILE *ase_file)
{
	char data[255];
	int index;
	int v=0, f=0, tv=0, tf=0, n=0;

	rewind(ase_file);

	while (!feof(ase_file))
	{  
		fscanf(ase_file, "%s", &data);
		if (strcmp ("*MESH_VERTEX", data) == 0)
		{
			fscanf(ase_file, "%d", &index);
			fscanf(ase_file, "%f %f %f", 
				&verts[index * 3 + 0], 
				&verts[index * 3 + 2], 
				&verts[index * 3 + 1]);
			v++;
			verts[index * 3 + 2] *= -1;
		}
		else if (strcmp ("*MESH_FACE", data) == 0)
		{
			fscanf(ase_file, "%d:", &index);
			fscanf(ase_file, " A: %d B: %d C: %d", 
				&faces[3 * index + 0],
				&faces[3 * index + 1], 
				&faces[3 * index + 2]);
			f++;
		}
		else if (strcmp ("*MESH_TVERT", data) == 0)
		{
			fscanf(ase_file, "%d", &index);
			fscanf(ase_file, "%f %f", 
				&tverts[index * 2 + 0], 
				&tverts[index * 2 + 1]);
			tverts[index * 2 + 1] = 1 - tverts[index * 2 + 1];
			tv++;
		}
		else if (strcmp ("*MESH_TFACE", data) == 0)
		{
			fscanf(ase_file, "%d", &index);
			fscanf(ase_file, "%d %d %d", 
				&tfaces[index * 3 + 0], 
				&tfaces[index * 3 + 1],
				&tfaces[index * 3 + 2]);
			tf++;
		}
		else if (strcmp ("*MESH_VERTEXNORMAL", data) == 0)
		{
			fscanf(ase_file, "%d", &index);
			fscanf(ase_file, "%f %f %f", 
				&vnorms[index * 3 + 0], 
				&vnorms[index * 3 + 1],
				&vnorms[index * 3 + 2]);
			n++;
		}
	}
	printf ("Vertexes Read %d\n", v);
	printf ("Faces Read %d\n", f);
	printf ("Texture Vertexes Read %d\n", tv);
	printf ("Texture Faces Read %d\n", tf);
	printf ("Normal Vertexes Read %d\n",n);

}

void Allocate ()
{
	verts = (float *)malloc (num_verts * 3 * sizeof(float));
	vnorms = (float *)malloc (num_verts * 3 * sizeof(float));
	tverts = (float *)malloc (num_tverts * 2 * sizeof(float));
	faces = (unsigned int *)malloc (num_faces * 3 * sizeof(unsigned int));
	tfaces = (unsigned int *)malloc (num_tfaces * 3 * sizeof(unsigned int));
}

void SaveFile (FILE *f)
{
	mdh_header header;

	printf ("Enter texture filename :");
	scanf("%s", header.texture);

	header.version = VERSION;
	header.numfaces = num_faces;
	header.numverts = num_verts;
	strcpy (header.tag, "MDH");

	fwrite(&header, sizeof (mdh_header), 1, f);

	fwrite(faces, sizeof(unsigned int), num_faces * 3, f);
	fwrite(verts, sizeof(float), num_verts * 3, f);
	fwrite(vnorms, sizeof(float), num_verts * 3, f);
	fwrite(tverts, sizeof(float), num_verts * 2, f);
}

int main ()
{
	FILE *fin, *fout;
	char filename[64];

	texture[0] = 0;

	printf("ASE -> DH Convert v%d\n\n", VERSION);

	printf("Enter filename : ");
	scanf ("%s", filename);

	if (fin = fopen(filename, "r"))
	{
		GetInfo(fin);
		Allocate();
		GetData(fin);
		fclose(fin);
	}
	else
	{
		printf("Open %s fail", filename);
		return 1;
	}

	filename[strlen(filename)-3] = 0;
	strcat(filename, "dh");

	if (fout = fopen(filename, "wb"))
	{
		printf ("Saving %s\n", filename);
		SaveFile (fout);
		fclose (fout);
	}
	else
	{
		printf("Open %s fail", filename);
		return 1;
	}
	return 0;
}