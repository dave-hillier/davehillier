#include "omega_common.h"
#include "omega_gl.h"


#define MAX_ENTS 1024
// need init
// object array?
entity world_entities[MAX_ENTS];

bool EntVisible (entity *e)
{
	vec3_t a, b;
	VectorAdd(e->position, e->maxs, a);
	VectorAdd(e->position, e->mins, a);

	if (pointinfrustum(a) && pointinfrustum(b))
		return true;
	else
		return false;
}

void RunEntity (entity *e)
{
	if (/*EntVisible(e)*/true)
	{
		glPushMatrix();

		glScalef (e->scale[0], e->scale[1], e->scale[2]);
		glTranslatef (e->position[0], e->position[1], e->position[2]);

		glRotatef (e->angles[1],	0, 0, 1);
		glRotatef (-e->angles[0],	0, 1, 0);
		glRotatef (e->angles[2],	1, 0, 0);

		glDepthMask(GL_TRUE);
		glEnable(GL_LIGHTING);
		
		RenderModel(e->model);

		glPopMatrix();

		glDepthMask(GL_FALSE);
		glDisable(GL_LIGHTING);
	}

	// Collision
	if (WorldCollision(e->position))
		VectorInverse(e->velocity);

	// Movement
	// Needs scaling with frame rate!
	VectorAdd (e->position, e->velocity, e->position);

	// think
	
}
void RunEntities ()
{
	int i;

	for (i = 0; i < MAX_ENTS; i++)
		// Improve, I dont want to run them all, unless i really need to
	{
		if (world_entities[i].in_use)
			RunEntity(&world_entities[i]);
	}
}

entity *GetEntity ()
{
	int i;

	for (i = 0; i < MAX_ENTS; i++)
	{
		if (!world_entities[i].in_use)
		{
			world_entities[i].in_use = true;
			VectorSet(world_entities[i].scale, 1, 1, 1);
			return &world_entities[i];
		}
	}
}
void InitEntities ()
{
	int i;
	for (i = 0; i < MAX_ENTS; i++)
		world_entities[i].in_use = false;
}
void FreeEntity (entity *e)
{
	e->in_use = false;
}
