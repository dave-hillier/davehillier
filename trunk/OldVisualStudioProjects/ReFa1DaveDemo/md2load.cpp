/*
   OpenGL MD2Load

   File		: md2load.c
   Date		: 26/11/00
   Author	: Mustata Bogdan (LoneRunner)
   Contact	: lonerunner@planetquake.com
   
   Change Log
   ----------
   27/12/2000 - added code for loding PCX file
   28/12/2000 - added code for FPS counter and scale prog. with the CPU power
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <GL/gl.h>


#define NUMVERTEXNORMALS 162

float avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};

typedef float vec_t;
typedef vec_t vec3_t[3];

enum {
    LEFT = 0, 
    RIGHT,
};

int mButton = -1;
int mOldY, mOldX;

vec3_t eye = {0.0f, 0.0f, 50.0f};
vec3_t rot = {0.0f, -45.0f, 0.0f};

int fullscreen=0, pause=0;

DWORD tStart,tCur, tFps;
DWORD tf=0, nf=0, showFps=0;

typedef unsigned char byte;

#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

typedef struct 
{ 
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
} md2_header_t;

typedef struct
{
   byte vertex[3];
   byte lightNormalIndex;
} md2_alias_triangleVertex_t;

typedef struct
{
   float vertex[3];
   float normal[3];
} md2_triangleVertex_t;

typedef struct
{
   short vertexIndices[3];
   short textureIndices[3];
} md2_triangle_t;

typedef struct
{
   short s, t;
} md2_textureCoordinate_t;

typedef struct
{
   float scale[3];
   float translate[3];
   char name[16];
   md2_alias_triangleVertex_t alias_vertices[1];
} md2_alias_frame_t;

typedef struct
{
   char name[16];
   md2_triangleVertex_t *vertices;
} md2_frame_t;

typedef char md2_skin_t[64];

typedef struct
{
   float s, t;
   int vertexIndex;
} md2_glCommandVertex_t;

typedef struct
{
	md2_header_t			header;
	md2_skin_t				*skins;
	md2_textureCoordinate_t	*texCoords;
	md2_triangle_t			*triangles;
	md2_frame_t				*frames;
	int						*glCommandBuffer;
} md2_model_t;

typedef struct animation_s
{
	char	nameAnimation[16];
	int		firstFrame;
	int		lastFrame;
} animation_t;

typedef struct md2_anim_s
{
	int			count_anim;
	animation_t anim[MD2_MAX_FRAMES];
} md2_anim_t;

md2_model_t *q2_loadmodel(char model_name[260]);
void q2_drawframe(md2_model_t *mdl, int curFrame);
void q2_freemodel(md2_model_t *mdl);
void q2_drawframeint(md2_model_t *mdl, int curFrame, int nextFrame, float pol);
md2_anim_t q2_makeanim(md2_model_t *mdl);

md2_model_t *model;
md2_anim_t	anim;
int			canim=0;
int			speed=5; /* speed is in frame/second */

md2_model_t *q2_loadmodel(char model_name[260])
{
	FILE			*md2file;
	char			ver[5];
	int				i, j;
	md2_model_t		*mdl;
	byte			buffer[MD2_MAX_FRAMESIZE];
	
	mdl = (md2_model_t *)malloc(sizeof(md2_model_t));
	md2file = fopen(model_name, "rb");

	if (md2file == NULL) return NULL;
	else
	{
		printf("load model %s...\n", model_name);
		fread(&mdl->header, 1, sizeof(md2_header_t), md2file);

		sprintf(ver, "%c%c%c%c", mdl->header.magic, mdl->header.magic>>8, mdl->header.magic>>16, mdl->header.magic>>24);

		if ( strcmp(ver, "IDP2") || mdl->header.version != 8) return NULL;

		fseek(md2file, mdl->header.offsetSkins, SEEK_SET);
		
		mdl->skins = (md2_skin_t *) malloc(sizeof(md2_skin_t) * mdl->header.numSkins);
		for (i=0; i<mdl->header.numSkins; i++)
			fread(&mdl->skins[i], sizeof(md2_skin_t), 1, md2file);
		
		fseek(md2file, mdl->header.offsetTexCoords, SEEK_SET);

		mdl->texCoords = (md2_textureCoordinate_t *) malloc(sizeof(md2_textureCoordinate_t) * mdl->header.numTexCoords);
		
		for (i=0; i<mdl->header.numTexCoords; i++)
			fread(&mdl->texCoords[i], sizeof(md2_textureCoordinate_t), 1, md2file);
		
		fseek(md2file, mdl->header.offsetTriangles, SEEK_SET);
		
		mdl->triangles = (md2_triangle_t *) malloc(sizeof(md2_triangle_t) * mdl->header.numTriangles);
		
		for (i=0; i<mdl->header.numTriangles; i++)
			fread(&mdl->triangles[i], sizeof(md2_triangle_t), 1, md2file);
		
		fseek(md2file, mdl->header.offsetFrames, SEEK_SET);
		mdl->frames = (md2_frame_t *) malloc(sizeof(md2_frame_t) * mdl->header.numFrames);

		for (i=0; i<mdl->header.numFrames; i++)
		{
			md2_alias_frame_t *frame = (md2_alias_frame_t *) buffer;

			mdl->frames[i].vertices = (md2_triangleVertex_t *) malloc(sizeof(md2_triangleVertex_t) * mdl->header.numVertices);

			fread(frame, 1, mdl->header.frameSize, md2file);
			strcpy(mdl->frames[i].name, frame->name);
			
			for (j=0; j<mdl->header.numVertices; j++)
			{
				mdl->frames[i].vertices[j].vertex[0] = (float) ((int) frame->alias_vertices[j].vertex[0]) * frame->scale[0] + frame->translate[0];
				mdl->frames[i].vertices[j].vertex[2] = -1* ((float) ((int) frame->alias_vertices[j].vertex[1]) * frame->scale[1] + frame->translate[1]);
				mdl->frames[i].vertices[j].vertex[1] = (float) ((int) frame->alias_vertices[j].vertex[2]) * frame->scale[2] + frame->translate[2];

				mdl->frames[i].vertices[j].normal[0] = avertexnormals[frame->alias_vertices[j].lightNormalIndex][0];
				mdl->frames[i].vertices[j].normal[1] = avertexnormals[frame->alias_vertices[j].lightNormalIndex][1];
				mdl->frames[i].vertices[j].normal[2] = avertexnormals[frame->alias_vertices[j].lightNormalIndex][2];
			}
		}

		fseek(md2file, mdl->header.offsetGlCommands, SEEK_SET);

		mdl->glCommandBuffer = (int *) malloc(sizeof(int) * mdl->header.numGlCommands);
		fread(mdl->glCommandBuffer, sizeof(int), mdl->header.numGlCommands, md2file);
		
		fclose(md2file);
	}
	
	return mdl;
}

void q2_drawframe(md2_model_t *mdl, int curFrame)
{
	int i = 0;
	int val = mdl->glCommandBuffer[i++];

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
			float s =  * (float *) &mdl->glCommandBuffer[i++];
			float t =  * (float *) &mdl->glCommandBuffer[i++];
			int index = mdl->glCommandBuffer[i++];

			glTexCoord2f(s, t);

			glNormal3f(mdl->frames[curFrame].vertices[index].normal[0],
				mdl->frames[curFrame].vertices[index].normal[2],
				-mdl->frames[curFrame].vertices[index].normal[1]);

			glVertex3f(mdl->frames[curFrame].vertices[index].vertex[0],
				mdl->frames[curFrame].vertices[index].vertex[1],
				mdl->frames[curFrame].vertices[index].vertex[2]);			
		}

		glEnd ();

		val = mdl->glCommandBuffer[i++];
	}
}

void q2_drawframeint(md2_model_t *mdl, int curFrame, int nextFrame, float pol)
{
	int i = 0;
	int val = mdl->glCommandBuffer[i++];
	float x1, y1, z1, x2, y2, z2;
	float nx1, ny1, nz1, nx2, ny2, nz2;
	
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
			float s =  * (float *) &mdl->glCommandBuffer[i++];
			float t =  * (float *) &mdl->glCommandBuffer[i++];
			int index = mdl->glCommandBuffer[i++];

			glTexCoord2f(s, t);
			
			x1 = mdl->frames[curFrame].vertices[index].vertex[0];
			y1 = mdl->frames[curFrame].vertices[index].vertex[1];
			z1 = mdl->frames[curFrame].vertices[index].vertex[2];

			x2 = mdl->frames[nextFrame].vertices[index].vertex[0];
			y2 = mdl->frames[nextFrame].vertices[index].vertex[1];
			z2 = mdl->frames[nextFrame].vertices[index].vertex[2];

			nx1 = mdl->frames[curFrame].vertices[index].normal[0];
			ny1 = mdl->frames[curFrame].vertices[index].normal[1];
			nz1 = mdl->frames[curFrame].vertices[index].normal[2];

			nx2 = mdl->frames[nextFrame].vertices[index].normal[0];
			ny2 = mdl->frames[nextFrame].vertices[index].normal[1];
			nz2 = mdl->frames[nextFrame].vertices[index].normal[2];
			
			glNormal3f( nx1 + pol * (nx2 - nx1), ny1 + pol * (ny2 - ny1), nz1 + pol * (nz2 - nz1) );
			glVertex3f( x1 + pol * (x2 - x1), y1 + pol * (y2 - y1), z1 + pol * (z2 - z1) );
		}

		glEnd();

		val = mdl->glCommandBuffer[i++];
	}
}

void q2_freemodel(md2_model_t *mdl)
{
	if (mdl->skins) free(mdl->skins);
	if (mdl->texCoords) free(mdl->texCoords);
	if (mdl->triangles) free(mdl->triangles);

	if (mdl->frames)
	{
		int i;

		for (i=0; i<mdl->header.numFrames; i++)
			if (mdl->frames[i].vertices) free(mdl->frames[i].vertices);

		free(mdl->frames);
	}

	if (mdl->glCommandBuffer) free(mdl->glCommandBuffer);
	
	free(mdl);
	
	printf("free model...\n");
}

md2_anim_t q2_makeanim(md2_model_t *mdl)
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
	animation.anim[count_anim].lastFrame = mdl->header.numFrames;
	count_anim++;

	for (i=0; i<model->header.numFrames; i++)
	{
		strcpy(t_st, mdl->frames[i].name);
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

	anim_vector[anim_index] = mdl->header.numFrames;
	anim_index++;

	for(i=0; i<anim_index; i++)
	{
		animation.anim[i+1].firstFrame = anim_vector[i];
		animation.anim[i+1].lastFrame = anim_vector[i+1];
	}
	
	animation.count_anim = count_anim;

	printf("----------------------------\n");
	printf("anim: %d\n", animation.count_anim);
	for (i=0; i<animation.count_anim; i++) printf("%d %s %d %d\n", i, animation.anim[i].nameAnimation, animation.anim[i].firstFrame, animation.anim[i].lastFrame);

	return animation;
}

void clamp(vec3_t v)
{
    int i;
    
    for (i = 0; i < 3; i ++)
        if (v[i] > 360 || v[i] < -360)
            v[i] = 0;
}

/*
void glutDisplay(void)
{   
	static int f=0, s=0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	glTranslatef (-eye[0], -eye[1], -eye[2]);
	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rot[2], 0.0f, 0.0f, 1.0f);

	// draw q2 model
	glBindTexture(GL_TEXTURE_2D, 2);
	q2_drawframeint(model, f, f+1, (float)(tCur-tFps)/(float)(1000/speed));
	
	// set curent animation
	if (!pause)
	{ 
		if (tCur-tFps > (DWORD)(1000/speed))
		{
			if (f > anim.anim[canim].lastFrame-3) f = anim.anim[canim].firstFrame;
			else f = f++;
			tFps = tCur;
		}
	}
	
	// draw fps
	glprint(glutGet(GLUT_WINDOW_WIDTH)-64, 25, 18.f, "%3d fps", showFps);

	glPopMatrix();
	glFlush();
	glutSwapBuffers();
    
    // get curent time and modify fps
	tCur = timeGetTime();

    if (tCur-tf>=1000)
	{
		showFps = nf;
		tf = tCur;
		nf = 0;
	}
	else nf++;
}*/
/*
int main(int argc, char *argv[])
{   

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("md2load");

	glutReshapeFunc(glutResize);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutDisplay);
    glutKeyboardFunc(glutKeyboard);
	glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
 
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glutCreateMenu(glutMenu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
	glutAddMenuEntry("Full Screen", 1);
	glutAddMenuEntry("Exit", 2);
		
	// load font from font.tga
	loadTGA("textures\\font.tga", 1);

	// load skin drfreak.pcx for q2model
	loadPCX("model\\drfreak.pcx", 2);
	
	// load q2model drfreak
	model = q2_loadmodel("model\\tris.md2");
	
	// get animation from q2model
	anim = q2_makeanim(model);

	tStart = timeGetTime();

	glutMainLoop();
	
	// free q2model drfreak
	q2_freemodel(model);

	return 0;
}*/