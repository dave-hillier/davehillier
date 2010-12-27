/*
=========================================================================
MDH module
----------


=========================================================================
*/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <stdio.h>
#include <malloc.h>
#include "common.h"


mdh_list *model_list = NULL;

mdh *ReadMDH (char *filename)
{
	FILE			*fileptr;
	mdh_header		header;
	mdh				*model;
	unsigned int	*faces;
	float			*verts;
	float			*vnorms;
	float			*tverts;
	unsigned int	*tfaces;

	if (fileptr = fopen(filename, "rb"))
	{
		// Get the file details
		fread(&header, sizeof (mdh_header), 1, fileptr);
		// Allocate memory
		faces = (unsigned int *)calloc (header.numfaces * 3, sizeof(unsigned int));
		verts = (float *)calloc (header.numverts * 3, sizeof(float));
		vnorms = (float *)calloc (header.numverts * 3, sizeof(float));
		tverts = (float *)calloc (header.numtverts * 2, sizeof(float));
		tfaces = (unsigned int *)calloc (header.numtfaces * 3, sizeof(unsigned int));

		// Read the actual data
		fread(verts, sizeof(float), header.numverts * 3, fileptr);
		fread(faces, sizeof(unsigned int), header.numfaces * 3, fileptr);
		fread(vnorms, sizeof(float), header.numverts * 3, fileptr);
		fread(tverts, sizeof(float), header.numtverts * 2, fileptr);
		fread(tfaces, sizeof(unsigned int), header.numtfaces * 3, fileptr);

		fclose (fileptr);

		model = malloc(sizeof(mdh));
		model->numfaces = header.numfaces;
		model->numtfaces = header.numtfaces;
		model->numtverts = header.numtverts;
		model->numverts = header.numverts;

		model->faces = faces;
		model->tfaces = tfaces;
		model->tverts = tverts;
		model->verts = verts;
		model->vnorms = vnorms;

		return model;
	}
	else
		return NULL;
}

mdh *getmdhptr (char *filename)
{
	mdh_list *mptr;
	// check through the model list	
	if (model_list != NULL)
	{
		for (mptr = model_list; mptr != NULL; mptr = mptr->next)
		{
			// if its already in the memory use that address
			if (stricmp(filename, mptr->filename) == 0)
				return mptr->model;
		}
	}
	else
	{
		model_list = (mdh_list *)malloc(sizeof(mdh_list));
		model_list->next = NULL;
		strcpy (model_list->filename, filename);
		model_list->model = ReadMDH(filename);
	}
	// otherwise readthe model in and add it to the list

	// ptr should be NULL
	mptr = (mdh_list *)malloc(sizeof(mdh_list));
	mptr->next = NULL;
	strcpy (mptr->filename, filename);
	mptr->model = ReadMDH(filename);

	return mptr->model;
}

void renderModels()
{
	int i;
	mdh *model1;
	// Do for entities list

/*	
	glVertexPointer(3, GL_FLOAT, 0, model_list->model->verts);
	glDrawElements(GL_TRIANGLES, 3 * model_list->model->numfaces, GL_UNSIGNED_INT, model_list->model->faces);
*/

	model1 = model_list->model;
	glBegin(GL_TRIANGLES);
	//glBegin(GL_LINES);
	{	
		for (i=0; i < (int)model1->numfaces; i++)
		{
			vec3f a, b, c, n0, n1, n2;

			glColor3f  (1.0, 0.0, 0.0);

			n0[0] = model1->vnorms[model1->faces[i * 3 + 0] * 3 + 0];
			n0[1] = model1->vnorms[model1->faces[i * 3 + 0] * 3 + 1];
			n0[2] = model1->vnorms[model1->faces[i * 3 + 0] * 3 + 2];

			a[0] = model1->verts[model1->faces[i * 3 + 0] * 3 + 0];
			a[1] = model1->verts[model1->faces[i * 3 + 0] * 3 + 1];
			a[2] = model1->verts[model1->faces[i * 3 + 0] * 3 + 2];

			n1[0] = model1->vnorms[model1->faces[i * 3 + 1] * 3 + 0];
			n1[1] = model1->vnorms[model1->faces[i * 3 + 1] * 3 + 1];
			n1[2] = model1->vnorms[model1->faces[i * 3 + 1] * 3 + 2];

			b[0] = model1->verts[model1->faces[i * 3 + 1] * 3 + 0];
			b[1] = model1->verts[model1->faces[i * 3 + 1] * 3 + 1];
			b[2] = model1->verts[model1->faces[i * 3 + 1] * 3 + 2];

			n2[0] = model1->vnorms[model1->faces[i * 3 + 2] * 3 + 0];
			n2[1] = model1->vnorms[model1->faces[i * 3 + 2] * 3 + 1];
			n2[2] = model1->vnorms[model1->faces[i * 3 + 2] * 3 + 2];

			c[0] = model1->verts[model1->faces[i * 3 + 2] * 3 + 0];
			c[1] = model1->verts[model1->faces[i * 3 + 2] * 3 + 1];
			c[2] = model1->verts[model1->faces[i * 3 + 2] * 3 + 2];

			glNormal3fv(n0);
			glColor3f  (1.0, 0.0, 0.0);
			glVertex3fv(a);


			glNormal3fv(n1);
			glColor3f  (1.0, 1.0, 0.0);
			glVertex3fv(b);
			
			glNormal3fv(n2);
			glColor3f  (1.0, 1.0, 1.0);
			glVertex3fv(c);
		}
		
	}
	glEnd();
}