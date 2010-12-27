// ms3d_model.cpp: implementation of the ms3d_model class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <GL/gl.h>
#include "ms3d_model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ms3d_model::ms3d_model()
{

}

ms3d_model::~ms3d_model()
{
	for(int i = 0; i < numGroups; i++)
		free(groups[i].triangleIndices);

	free(vertices);
	free(triangles);
	free(groups);
}


void ms3d_model::Load(char *filename)
{
	FILE* fp;
	ms3d_header header;
	int i;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
	//	console_printf("Unable to open %s\n", filename);
		return;
	}



	fread(&header.id, sizeof(char), 10, fp);
	fread(&header.version, 1, sizeof(int), fp);

	if(strncmp(header.id, "MS3D000000", 10)!=0)
	{
	//	console_printf("%s incorrect fp format\n", filename);
		return;
	}

	if(header.version != 3 && header.version != 4)
	{
	//	console_printf("%s incorrect version\n", filename);
		return;
	}

	
	fread(&numVertices, sizeof(unsigned short), 1, fp);
	vertices = (ms3d_vertex *)malloc(sizeof(ms3d_vertex)*numVertices);//new MS3D_VERTEX [numVertices];
	for(i=0; i<numVertices; i++)
	{
		fread(&vertices[i].flags,	sizeof(byte),  1, fp);
		fread( vertices[i].vertex,	sizeof(float), 3, fp);
		fread(&vertices[i].boneID,	sizeof(char),  1, fp);
		fread(&vertices[i].refCount, sizeof(byte),  1, fp);
	}
	
	fread(&numTriangles, sizeof(unsigned short), 1, fp);
	triangles = (ms3d_triangle *)malloc(sizeof(ms3d_triangle)*numTriangles); 
		//new MS3D_TRIANGLE [numTriangles];
	for(i=0; i<numTriangles; i++)
	{
		fread(&triangles[i].flags,		sizeof(unsigned short), 1, fp);
		fread( triangles[i].vertexIndices,	sizeof(unsigned short), 3, fp);
		fread( triangles[i].vertexNormals[0],sizeof(float),		3, fp);
		fread( triangles[i].vertexNormals[1],sizeof(float),	 	3, fp);
		fread( triangles[i].vertexNormals[2],sizeof(float),		3, fp);
		fread( triangles[i].u,		sizeof(float),		3, fp);
		fread( triangles[i].v,		sizeof(float),		3, fp);
		fread(&triangles[i].smoothingGroup,	sizeof(unsigned char),	1, fp);
		fread(&triangles[i].groupIndex,	sizeof(unsigned char),	1, fp);
	}
	
	fread(&numGroups, sizeof(unsigned short), 1, fp);
	groups = (ms3d_group *)malloc(sizeof(ms3d_triangle)*(numTriangles)); 
		// new MS3D_GROUP [numGroups];
	for(i=0; i<numGroups; i++)
	{
		fread(&groups[i].flags,	 sizeof(unsigned char),	1,  fp);
		fread( groups[i].name,	 sizeof(char),		32, fp);
		fread(&groups[i].numTriangles,sizeof(unsigned short),1,  fp);
		
		groups[i].triangleIndices = 
			(unsigned short *)malloc(sizeof(unsigned short)*groups[i].numTriangles);
			// new unsigned short [groups[i].numTriangles];
		
		fread( groups[i].triangleIndices, sizeof(unsigned short), groups[i].numTriangles,fp);
		fread(&groups[i].materialIndex,   sizeof(char), 1, fp);
	}
	fclose(fp);	
	// load associated texture
	// fill array
}

void ms3d_model::Render()
{
	int i;
	int j;
	int k;
	
	glEnable(GL_CULL_FACE);
	// bind the texture or should it be per group
	for(i=0; i<numGroups; i++ )
	{
		glBegin(GL_TRIANGLES);
		for(j=0; j<groups[i].numTriangles; j++)
		{
			int triangleIndex			=  groups[i].triangleIndices[j];
			const ms3d_triangle *tri	= &triangles[triangleIndex];
			
			for(k=0; k<3; k++)
			{
				const int index= tri->vertexIndices[k];
				
				glNormal3fv( tri->vertexNormals[k]);
				glTexCoord2f(tri->u[k], tri->v[k]);
				glVertex3fv(vertices[index].vertex);
			}
		}
		glEnd();
	}
	glDisable(GL_CULL_FACE);

}