#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "p6_common.h"
typedef float vec3_t[3];

#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}
#define VectorSet(vec, a,b,c) {vec[0]=a;vec[1]=b;vec[2]=c;}
#define VectorInverse(vec) {vec[0]*=-1;vec[1]*=-1;vec[2]*=-1;}
#define VectorInverse2(a, b) {b[0]=a[0]*-1;b[1]=a[1]*-1;b[2]=a[2]*-1;}
#define VectorScale(a,b,c) {c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;}

int particle_texture;
void test1 ()
{
	float m[16];// TODO: a global maybe
	float size = 30;
	vec3_t pts[4], x, y, pos;
	pos[0] = -100;
	pos[1] = -100;
	pos[2] = -500;

	glGetFloatv(GL_MODELVIEW_MATRIX, m);// TODO: Should this be accessed here or else where?
	
	VectorSet(x, m[0], m[4], m[8]);
	VectorSet(y, m[1], m[5], m[9]);
	
	VectorAdd ( x, y, pts[2] );
	VectorInverse2( pts[2], pts[0] );
	VectorSubtract ( x, y, pts[1]);
	VectorInverse2( pts[1], pts[3] );

	VectorScale(pts[0], size, pts[0]);
	VectorScale(pts[1], size, pts[1]);
	VectorScale(pts[2], size, pts[2]);
	VectorScale(pts[3], size, pts[3]);
	
	VectorAdd (pos, pts[0], pts[0]);
	VectorAdd (pos, pts[1], pts[1]);
	VectorAdd (pos, pts[2], pts[2]);
	VectorAdd (pos, pts[3], pts[3]);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, particle_texture);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glColor4f(0.1f, 0.0f, 0.9f, 1.0f);
	glBegin(GL_TRIANGLE_FAN);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(pts[0]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(pts[1]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(pts[2]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(pts[3]);
	}
	glEnd();
}
void testinit()
{
	particle_texture = GetTexture("star.tga");
}