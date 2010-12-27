// frustum.cpp - works well so i'll keep using the old c
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "mathlib.h"
#include "Camera.h"



typedef float vec_t;
typedef vec_t vec2_t[2];  /* st */
typedef vec_t vec3_t[3];  /* xyz */
typedef vec_t vec4_t[4];  /* xyzw */
typedef float mat3_t[9];  /* 3x3 matrix */
typedef float mat4_t[16]; /* 4x4 matrix */
typedef int	bbox_t[6];        /* Integer bounding box (mins, maxs)*/
typedef float bboxf_t[6];     /* Float bounding box */

enum
{
    CLIP_RIGHT_BIT   = 1,
    CLIP_LEFT_BIT    = 1 << 1,
    CLIP_TOP_BIT     = 1 << 2,
    CLIP_BOTTOM_BIT  = 1 << 3,
    CLIP_FAR_BIT     = 1 << 4,
    CLIP_NEAR_BIT    = 1 << 5,
};


#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]

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
static mat4_t clipmat;        /* Matrix to go from worldspace to clipspace */

void gen_clipmat(void)
{
//	return;
    mat4_t modelview;
    mat4_t proj;

    /* Get the modelview and projection matricies from GL */
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)&modelview);
    glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)&proj);

    /* Multiply to get clip coordinate transformation */
    mat4_mmult(proj, modelview, clipmat);
}

int cliptest_point(vec4_t p)
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
int point_test_bboxf(bboxf_t bv, vec3_t pt)
{
	if (pt[0] >= bv[0] && pt[0] <= bv[3] &&
	//	pt[1] >= bv[1] && pt[1] <= bv[4] &&
		pt[2] >= bv[2] && pt[2] <= bv[5])
		return 1;
	else
		return 0;
}
/* Test bounding box for intersection with view fustrum.
 * Return val:   0 = reject
 *               1 = accept
 *               2 = trivially accept (entirely in fustrum)
 */
int cliptest_bboxf(bboxf_t bv)
{
//	return 2;
    static int corner_index[8][3] =
    {
		{0, 1, 2}, {3, 1, 2}, {3, 4, 2}, {0, 4, 2},
		{0, 1, 5}, {3, 1, 5}, {3, 4, 5}, {0, 4, 5}
    };

    vec4_t corner[8];
    int clipcode, clip_or, clip_and, clip_in;
    int i;

    /* Check if eye point is contained */
	if (point_test_bboxf(bv, camera.position))
		return 1;
    
    clip_in = clip_or = 0; clip_and = 0xff;
    for (i=0; i < 8; ++i)
    {
		corner[i][0] = bv[corner_index[i][0]];
		corner[i][1] = bv[corner_index[i][1]];
		corner[i][2] = bv[corner_index[i][2]];
		corner[i][3] = 1.0;
		
		mat4_vmult(clipmat, corner[i], corner[i]);
		clipcode = cliptest_point(corner[i]);
		clip_or |= clipcode;
		clip_and &= clipcode;
		if (!clipcode) clip_in = 1;
    }

    /* Check for trival acceptance/rejection */
    if (clip_and) return 0;
    if (!clip_or) return 2;
    if (clip_in) return 1;   /* At least one corner in view fustrum */
	
#if 0
    /* FIXME: need something better for this. */
    /* Maybe find maximum radius to each corner */
    {
		/* Normalize coordinates */
		vec3_t center, rad;
		float cw;
		
		cw = 1.0f/corner[0][3];
		vec_scale(corner[0], cw, corner[0]);
		corner[0][3] = 1.0;
		cw = 1.0f/corner[6][3];
		vec_scale(corner[6], cw, corner[6]);
		corner[6][3] = 1.0;
		
		/* Check for non-trivial acceptance */
		vec_avg(corner[0], corner[6], center);
		vec_sub(corner[0], center, rad);
		if (sqrt(vec_dot(center, center)) -
			sqrt(vec_dot(rad, rad)) <= 1.41421356)
			return 1;
    }
	
    return 0;
#endif
    return 1;
}

int cliptest_bbox(bbox_t bbox)
{
    bboxf_t bv;

    bv[0] = (float)bbox[0];
    bv[1] = (float)bbox[1];
    bv[2] = (float)bbox[2];
    bv[3] = (float)bbox[3];
    bv[4] = (float)bbox[4];
    bv[5] = (float)bbox[5];

    return cliptest_bboxf(bv);
}

int point_test (vec3_t pt)
{
	vec4_t temp = { pt[0], pt[1], pt[2], 1.0 };

		
	mat4_vmult(clipmat, temp, temp);

	if (!cliptest_point(temp))
		return 1;
	else
		return 0;
}



int cliptest(Bounds bb)
{
    bboxf_t bv;

    bv[0] = bb.max.x;
    bv[1] = bb.max.y;
    bv[2] = bb.max.z;

    bv[3] = bb.min.x;
    bv[4] = bb.min.y;
    bv[5] = bb.min.z;

    return cliptest_bboxf(bv);
}
