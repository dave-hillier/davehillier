#include "omega_common.h"
#include "omega_gl.h"

void drawfog ()
{
	int i;
	return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	for (i=0; i < 255; i+=30)
	{
		glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
		glVertex3i(0, i, 0);
		glVertex3i(1024, i, 0);
		glVertex3i(1024, i, 1024);
		glVertex3i(0, i, 1024);

	}
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}