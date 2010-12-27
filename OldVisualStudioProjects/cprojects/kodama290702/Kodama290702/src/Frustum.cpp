// Fucked up... Im not sure what way around im doing stuff
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "ConsoleVars.h"
#include "Frustum.h"
#include "GLMain.h"
enum
{
    CLIP_RIGHT_BIT   = 1,
    CLIP_LEFT_BIT    = 1 << 1,
    CLIP_TOP_BIT     = 1 << 2,
    CLIP_BOTTOM_BIT  = 1 << 3,
    CLIP_FAR_BIT     = 1 << 4,
    CLIP_NEAR_BIT    = 1 << 5,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]
typedef float vec_t;
typedef vec_t vec2_t[2];  /* st */
typedef vec_t vec3_t[3];  /* xyz */
typedef vec_t vec4_t[4];  /* xyzw */
typedef float mat3_t[9];  /* 3x3 matrix */
typedef float mat4_t[16]; /* 4x4 matrix */
typedef int	bbox_t[6];        /* Integer bounding box (mins, maxs)*/
typedef float bboxf_t[6];     /* Float bounding box */

void mat4_mmult(mat4_t a, mat4_t b, mat4_t product)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
		P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
}

void mat4_vmult(mat4_t a, vec4_t b, vec4_t product)
{
    int i;
    float b0=b[0], b1=b[1], b2=b[2], b3=b[3];
    for (i=0; i < 4; i++)
    {
		product[i] = A(i,0)*b0 + A(i,1)*b1 + A(i,2)*b2 + A(i,3)*b3;
    }
}
static mat4_t clipmat;   
// AABB
Frustum *frustum;

Frustum::Frustum()
{

}

Frustum::~Frustum()
{

}

void Frustum::Update ()
{
	clip.Multiply(glapp->projection, glapp->modelView);
}
int Frustum::ClipTest (Bounds bb)
{
	Vector corners[] = {
		Vector(bb.max.x, bb.max.y, bb.max.z), 
		Vector(bb.min.x, bb.max.y, bb.max.z), 
		Vector(bb.min.x, bb.min.y, bb.max.z), 
		Vector(bb.max.x, bb.min.y, bb.max.z),
		Vector(bb.max.x, bb.max.y, bb.min.z), 
		Vector(bb.min.x, bb.max.y, bb.min.z), 
		Vector(bb.min.x, bb.min.y, bb.min.z), 
		Vector(bb.max.x, bb.min.y, bb.min.z)
    };

	float temp[4];
    int clipcode, clip_or, clip_and, clip_in;
    int i;
   
    clip_in = clip_or = 0; clip_and = 0xff;
	for (i = 0; i < 8; i++)
	{
		temp[0] = corners[i].x;
		temp[1] = corners[i].y;
		temp[2] = corners[i].z;
		temp[3] = 1.0;
		
		clip.Multiply(temp, temp);

		clipcode = ClipTest4f(temp);
		clip_or |= clipcode;
		clip_and &= clipcode;

		if (!clipcode) 
			clip_in = 1;	
	}

    if (clip_and) 
		return 0;
    if (!clip_or) 
		return 2;
    if (clip_in) 
		return 1;
	return 1;
}

int Frustum::ClipTest4f (float p[4])
{
    int mask = 0;
    const float cx = p[0];
    const float cy = p[1];
    const float cz = p[2];
    const float cw = p[3];      
    
    if (cx >  cw) mask |= CLIP_RIGHT_BIT;
    if (cx < -cw) mask |= CLIP_LEFT_BIT;
    if (cy >  cw) mask |= CLIP_TOP_BIT;
    if (cy < -cw) mask |= CLIP_BOTTOM_BIT;
    if (cz >  cw) mask |= CLIP_FAR_BIT;
    if (cz < -cw) mask |= CLIP_NEAR_BIT;

    return mask;
}
