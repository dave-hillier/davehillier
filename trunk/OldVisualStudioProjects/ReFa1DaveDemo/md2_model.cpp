// md2_model.cpp: implementation of the md2_model class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <GL/gl.h>
#include "md2_model.h"

#define NUMVERTEXNORMALS 162

float avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

md2_model::md2_model()
{

}

md2_model::~md2_model()
{
	if (skins) 
		free(skins);
	if (texCoords) 
		free(texCoords);
	if (triangles) 
		free(triangles);

	if (frames)
	{
		int i;

		for (i=0; i<header.numFrames; i++)
			if (frames[i].vertices) free(frames[i].vertices);

		free(frames);
	}

	if (glCommandBuffer) 
		free(glCommandBuffer);
	
}

void md2_model::Load(char model_name[256])
{
	FILE			*md2file;

	md2file = fopen(model_name, "rb");

	if (md2file)
	{
		char	ver[5];
		byte	buffer[MD2_MAX_FRAMESIZE];

		fread(&header, 1, sizeof(md2_header_t), md2file);

		sprintf(ver, "%c%c%c%c", header.magic, header.magic>>8, header.magic>>16, header.magic>>24);

		if ( strcmp(ver, "IDP2") || header.version != 8) 
			return;

		fseek(md2file, header.offsetSkins, SEEK_SET);
		skins = (md2_skin_t *) malloc(sizeof(md2_skin_t) * header.numSkins);

		for (int i=0; i<header.numSkins; i++)
			fread(&skins[i], sizeof(md2_skin_t), 1, md2file);
		
		fseek(md2file, header.offsetTexCoords, SEEK_SET);
		
		texCoords = (md2_textureCoordinate_t *) malloc(sizeof(md2_textureCoordinate_t) * header.numTexCoords);
		
		for (i=0; i<header.numTexCoords; i++)
			fread(&texCoords[i], sizeof(md2_textureCoordinate_t), 1, md2file);
		
		fseek(md2file, header.offsetTriangles, SEEK_SET);
		
		triangles = (md2_triangle_t *) malloc(sizeof(md2_triangle_t) * header.numTriangles);
		
		for (i=0; i<header.numTriangles; i++)
			fread(&triangles[i], sizeof(md2_triangle_t), 1, md2file);
		
		fseek(md2file, header.offsetFrames, SEEK_SET);
		frames = (md2_frame_t *) malloc(sizeof(md2_frame_t) * header.numFrames);
		
		for (i=0; i<header.numFrames; i++)
		{
			md2_alias_frame_t *frame = (md2_alias_frame_t *) buffer;
			
			frames[i].vertices = (md2_triangleVertex_t *) malloc(sizeof(md2_triangleVertex_t) * header.numVertices);
			
			fread(frame, 1, header.frameSize, md2file);
			strcpy(frames[i].name, frame->name);
			
			for (int j=0; j<header.numVertices; j++)
			{
				frames[i].vertices[j].vertex[0] = (float) ((int) frame->alias_vertices[j].vertex[0]) * frame->scale[0] + frame->translate[0];
				frames[i].vertices[j].vertex[2] = -1* ((float) ((int) frame->alias_vertices[j].vertex[1]) * frame->scale[1] + frame->translate[1]);
				frames[i].vertices[j].vertex[1] = (float) ((int) frame->alias_vertices[j].vertex[2]) * frame->scale[2] + frame->translate[2];
				
				frames[i].vertices[j].normal[0] = avertexnormals[frame->alias_vertices[j].lightNormalIndex][0];
				frames[i].vertices[j].normal[1] = avertexnormals[frame->alias_vertices[j].lightNormalIndex][1];
				frames[i].vertices[j].normal[2] = avertexnormals[frame->alias_vertices[j].lightNormalIndex][2];
			}
		}
		
		fseek(md2file, header.offsetGlCommands, SEEK_SET);
		
		glCommandBuffer = (int *) malloc(sizeof(int) * header.numGlCommands);
		fread(glCommandBuffer, sizeof(int), header.numGlCommands, md2file);
		
		fclose(md2file);
	}
	MakeAnim();
}


void md2_model::DrawFrameInt(int curFrame, int nextFrame, float pol)
{
	int i = 0;
	int val = glCommandBuffer[i++];
	float x1, y1, z1, x2, y2, z2;
	float nx1, ny1, nz1, nx2, ny2, nz2;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	while (val != 0)
	{
		int count;

		if (val > 0)
		{
			glBegin(GL_TRIANGLE_STRIP);
			count = val;
		}
		else
		{
			glBegin(GL_TRIANGLE_FAN);
			count = -val;
		}

		while (count--)
		{
			float s =  * (float *) &glCommandBuffer[i++];
			float t =  * (float *) &glCommandBuffer[i++];
			int index = glCommandBuffer[i++];

			glTexCoord2f(s, t);
			
			x1 = frames[curFrame].vertices[index].vertex[0];
			y1 = frames[curFrame].vertices[index].vertex[1];
			z1 = frames[curFrame].vertices[index].vertex[2];

			x2 = frames[nextFrame].vertices[index].vertex[0];
			y2 = frames[nextFrame].vertices[index].vertex[1];
			z2 = frames[nextFrame].vertices[index].vertex[2];

			nx1 = frames[curFrame].vertices[index].normal[0];
			ny1 = frames[curFrame].vertices[index].normal[1];
			nz1 = frames[curFrame].vertices[index].normal[2];

			nx2 = frames[nextFrame].vertices[index].normal[0];
			ny2 = frames[nextFrame].vertices[index].normal[1];
			nz2 = frames[nextFrame].vertices[index].normal[2];
			
			glNormal3f( nx1 + pol * (nx2 - nx1), ny1 + pol * (ny2 - ny1), nz1 + pol * (nz2 - nz1) );
			glVertex3f( x1 + pol * (x2 - x1), y1 + pol * (y2 - y1), z1 + pol * (z2 - z1) );
		}

		glEnd();

		val = glCommandBuffer[i++];
	}
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
}

void md2_model::MakeAnim()
{
	char	t_st[16], t_name[16], t_last[16];
	int		anim_vector[MD2_MAX_FRAMES];
	int		count_anim, anim_index, t_ln, i;
	md2_anim_t	animation;

	anim_index = 0;
	count_anim = 0;

	for(i=0; i<MD2_MAX_FRAMES; i++) anim_vector[i] = 0;

	strcpy(animation.anim[count_anim].nameAnimation, "all frames");
	animation.anim[count_anim].firstFrame = 0;
	animation.anim[count_anim].lastFrame = header.numFrames;
	count_anim++;

	for (i=0; i<header.numFrames; i++)
	{
		strcpy(t_st, frames[i].name);
		t_ln = strlen(t_st);

		t_ln--;
		while (t_st[t_ln] >= '0' && t_st[t_ln] <= '9' || t_st[t_ln] == '_') t_ln--;
		t_ln++;

		strncpy(t_name, t_st, t_ln);
		t_name[t_ln] = '\0';

		if ( strcmp(t_name, t_last) )
		{
			anim_vector[anim_index] = i;
			anim_index++;
				
			strcpy(animation.anim[count_anim].nameAnimation, t_name);
			animation.anim[count_anim].firstFrame = -1;
			animation.anim[count_anim].lastFrame = -1;
			count_anim++;
		}
		
		strcpy(t_last, t_name);
	}

	anim_vector[anim_index] = header.numFrames;
	anim_index++;

	for(i=0; i<anim_index; i++)
	{
		animation.anim[i+1].firstFrame = anim_vector[i];
		animation.anim[i+1].lastFrame = anim_vector[i+1];
	}
	
	animation.count_anim = count_anim;

/*	printf("----------------------------\n");
	printf("anim: %d\n", animation.count_anim);
	for (i=0; i<animation.count_anim; i++) printf("%d %s %d %d\n", i, animation.anim[i].nameAnimation, animation.anim[i].firstFrame, animation.anim[i].lastFrame);

	return animation;*/
}