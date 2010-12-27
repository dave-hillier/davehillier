#include "omega_common.h"
#include "omega_gl.h"


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

int nummodels;
// skeletal stuff, such as vectors need doing
#define TABLESIZE 512
float skellbob;

mdh *test, *modeltable[TABLESIZE], *skel;

mdh *read_mdh (char *filename)
{
	FILE			*fileptr;
	mdh_header		header;
	mdh				*model;
	int				i;

	if (fileptr = fopen(filename, "rb"))
	{
		// Get the file details
		fread(&header, sizeof (mdh_header), 1, fileptr);
		
		model = (mdh *)malloc(sizeof(mdh));
		model->nummeshes = header.meshes;
		model->meshes = (mdh_submesh *)malloc(sizeof(mdh_submesh) * model->nummeshes);
		strcpy(model->name, filename);

		for (i = 0; i < header.meshes; i++)
		{
			mdh_subheader subheader;
			mdh_submesh *submesh = &model->meshes[i];
			int		*faces;
			float	*verts;
			float	*vnorms;
			float	*tverts = NULL;

			fread(&subheader, sizeof (mdh_subheader), 1, fileptr);
			// Allocate memory
			faces = (unsigned int *)malloc (subheader.numfaces * 3 * sizeof(unsigned int));
			verts = (float *)malloc (subheader.numverts * 3 * sizeof(float));
			vnorms = (float *)malloc (subheader.numverts * 3 * sizeof(float));

			if (subheader.numtverts)
				tverts = (float *)malloc (subheader.numtverts * 2 * sizeof(float));

			// Read the actual data
			fread(faces, sizeof(int), subheader.numfaces * 3, fileptr);
			fread(verts, sizeof(float), subheader.numverts * 3, fileptr);
			fread(vnorms, sizeof(float), subheader.numverts * 3, fileptr);

			if (subheader.numtverts)
				fread(tverts, sizeof(float), subheader.numverts * 2, fileptr);

			submesh->numfaces = subheader.numfaces;
			submesh->numverts = subheader.numverts;
			submesh->numtverts = subheader.numtverts;

			submesh->faces = faces;
			submesh->verts = verts;
			submesh->vnorms = vnorms;
			submesh->tverts = tverts;

			strcpy(submesh->meshname, subheader.meshname);
			if (subheader.numtverts)
				submesh->texid = LoadTexture(subheader.texture, true);// TODO : texture info
			else
				submesh->texid = 0;

		}
		fclose (fileptr);

		return model;
	}
	else
	{
		ConPrintf("Error: Cant open %s\n", filename);
		return NULL;
	}
}

float CurrentTime();

void RenderModel (mdh *mdlptr)
{
	int i;
	mdh_submesh *meshptr;

	if (!mdlptr)
		return;

	for (i = 0, meshptr = mdlptr->meshes; i < mdlptr->nummeshes; i++, meshptr++)
	{
		glBindTexture(GL_TEXTURE_2D, meshptr->texid);
		glVertexPointer(3, GL_FLOAT, 0, meshptr->verts);		
		glTexCoordPointer(2, GL_FLOAT, 0, meshptr->tverts);
		glNormalPointer(GL_FLOAT, 0, meshptr->vnorms);
		glDrawElements(GL_TRIANGLES, meshptr->numfaces * 3, GL_UNSIGNED_INT, meshptr->faces);
	}
}

void material (void)
{
	GLfloat mat_ambient[] = {0.5, 0.5, 1.0, 0.0};
	GLfloat mat_diffuse[] = {0.5, 0.5, 1.0, 0.0};
	GLfloat mat_emission[] = {1.0, 1.0, 1.0, 0.0};
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat low_shininess[] = { 5.0 };

	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
}


void ModelFrame (void)
{
	GLfloat mat_emission[] = { 0.50, 0.50, 0.50, 0.50 };
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	// BRIGHT THINGS UP A BIT
	glEnable(GL_LIGHTING);
//	glEnable(GL_CULL_FACE);
	glLightfv(GL_LIGHT0, GL_POSITION, viewer.pos);


	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPushMatrix();

	glDepthMask(GL_TRUE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.5);

	skellbob +=2;
	glTranslatef(512, 255, 512);
	glRotatef ((float)skellbob, 0.0f, 1.0f, 0.0f);
	RenderModel (skel);
	glPopMatrix();

	glDisable(GL_ALPHA_TEST);
	glDepthMask(GL_FALSE);

	glDisable(GL_LIGHTING);
//	glDisable(GL_CULL_FACE);
}

mdh *ModelPointer (char *name)
{
	int i;
	
	for (i = 0; i < nummodels; i++)
	{
		if (stricmp(name, modeltable[i]->name) == 0)
			return modeltable[i];
	}

	modeltable[nummodels] = read_mdh(name);
	nummodels++;

	return modeltable[nummodels-1];
}
void ModelInit (void)
{
	nummodels = 0;
	skellbob = 0;

	skel = ModelPointer ("skel.dh4");
	ConPrintf("Precaching: models\n");
}

