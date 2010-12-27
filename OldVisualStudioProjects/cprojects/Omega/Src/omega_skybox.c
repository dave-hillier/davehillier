#include "omega_common.h"
#include "omega_gl.h"

mdh *ModelPointer (char *name);
mdh *skydome;


void RenderSun ()
{
	particle part;
	
	part.alpha = 1;
	VectorSet(part.pos, -50, 60, -90);
	part.size = 20;
	part.alpha = 1.0f;
	part.texture = particle_texture;

	glPushMatrix();
	glTranslatef (viewer.pos[0],  viewer.pos[1],  viewer.pos[2]);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	pRender(&part);	// draw the sun
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glPopMatrix();


}
void SkyRender()
{
	GLfloat mat_emission[] = {1.0, 1.0, 1.0, 0.0};
	
	glEnable(GL_CULL_FACE);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glTranslatef (512, -100, 512);
	RenderModel(skydome);
	glPopMatrix();
	RenderSun();
	glDisable(GL_CULL_FACE);
}

void SkyInit ()
{
	skydome = ModelPointer("skydome.dh4");
}