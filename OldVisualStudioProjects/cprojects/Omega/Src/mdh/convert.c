#include "ase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
==========================================================================
My custom model format.

  Things to add: texture name, texture tiling

==========================================================================
*/
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

FILE			*ase_file;
FILE			*out_file;

typedef struct {
	char title[3];
	unsigned char version;
	unsigned int numfaces;
	unsigned int numverts;
	unsigned int numtfaces;
	unsigned int numtverts;
	char texture[64];
} mdh_header;


void main ()
{
	char filename[256];
	model_t p;

	// open the file
	printf ("Enter ase file name :");
	scanf("%s", filename);


	aseLoad(filename, &p);

	return;


	// dump them out into my file format
}