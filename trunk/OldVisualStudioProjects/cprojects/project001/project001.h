#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA

#define WIN32_LEAN_AND_MEAN // is that the name??

#include <windows.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 //double pi hmm
#endif

#define WIDTH		640
#define HEIGHT		480


#define NAME "Project001"

/*typedef double vector2d[2];
typedef double vector3d[3];
typedef double vector4d[4];*/

typedef float vector2f[2];
typedef float vector3f[3];
typedef float vector4f[4];

#if !defined BYTE_DEFINED
typedef unsigned char 		byte;
#define BYTE_DEFINED 1
#endif

#undef true
#undef false

typedef enum {false, true}	bool;

enum { 
	RED, 
	GREEN, 
	BLUE, 
	ALPHA
};
typedef struct _particle {
	vector3f	position;
	vector3f	velocity;
	vector4f	colour;
	int			flags;
	float		size;
	double timestamp;
	struct _particle *next;
} particle;

extern double		curtime;
extern particle		*particles;

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)		((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])
