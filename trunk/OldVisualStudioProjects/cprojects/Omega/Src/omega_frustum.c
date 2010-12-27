#include "omega_common.h"
#include "omega_gl.h"

bool pointinfrustum (float *pt)
{
	float pos[3];

	VectorSubtract(pt, viewer.pos, pos);

	if (DotProduct(viewer.bottom, pos) < 0)
		return false;
	else if (DotProduct(viewer.top, pos) < 0)
		return false;
	else if (DotProduct(viewer.right, pos) < 0)
		return false;
	else if (DotProduct(viewer.left, pos) < 0)
		return false;
	else
		return true;
}

void calcfrustumnormals ()
{
	vec3_t f, r, u;
	float m[16];
	
	glGetFloatv(GL_MODELVIEW_MATRIX, m);

	VectorSet(r, m[0], m[4], m[8]);
	VectorSet(u, m[1], m[5], m[9]);
	VectorSet(f, -m[2], -m[6], -m[10]);

	VectorAdd (f, r, viewer.right);
	VectorSubtract (f, r, viewer.left);

	VectorAdd (f, u, viewer.top);
	VectorSubtract (f, u, viewer.bottom);

}
