// md2_model.h: interface for the md2_model class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __MD2MODEL_H__
#define __MD2MODEL_H__

typedef float vec_t;
typedef vec_t vec3_t[3];

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
/*
typedef struct
{
	md2_header_t			header;
	md2_skin_t				*skins;
	md2_textureCoordinate_t	*texCoords;
	md2_triangle_t			*triangles;
	md2_frame_t				*frames;
	int						*glCommandBuffer;
} md2_model_t;*/

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


class md2_model  
{
public:
	md2_model();
	virtual ~md2_model();

	void Load (char model_name[256]);
	void DrawFrameInt(int curFrame, int nextFrame, float pol);

private:
	md2_header_t			header;
	md2_skin_t				*skins;
	md2_textureCoordinate_t	*texCoords;
	md2_triangle_t			*triangles;
	md2_frame_t				*frames;
	int						*glCommandBuffer;

	void MakeAnim();
	md2_anim_t	animation;

};

#endif