#include "project001.h"
#include <malloc.h>

particle *particles;

void InitParticle ()
{
	particles = NULL;
}
/*
returns a list with the new particle at the start
*/

particle *AddParticle (particle		*prev,
						vector3f	pos, 
						vector3f	vel, 
						vector4f	colour,
						double		timestamp,
						int			flags, 
						float size)
{
	particle *ptr;

	ptr = malloc (sizeof(particle));
	ptr->next = prev;
	VectorCopy(pos, ptr->position);
	VectorCopy(vel, ptr->velocity);
	Vector4Copy(colour, ptr->colour);
	ptr->flags = flags;
	ptr->timestamp = timestamp;
	ptr->size = size;
	
//	MessageBox(NULL, "h", "h", MB_OK);
	return ptr;
}
void printp (particle *p)
{
	char str[80];

	if (!p)
		return;

	sprintf(str, "pos ( %f, %f, %f )", 
		p->position[0], p->position[1], p->position[2]);

	MessageBox(NULL, str, "DEBUG: AddParticle", MB_OK);

	printp(p->next);
}
void ProcessParticles ()
{
	particle *ptr;
	particle *prev_ptr;


	glBegin(GL_POINTS); //

	for (ptr = particles; ptr != NULL; prev_ptr = ptr++)
	{
/*		if (curtime > ptr->timestamp)
		{
			prev_ptr->next = ptr->next;
			free(ptr);
		}	*/	

	//	glPointSize(ptr->size);
		glColor3f(ptr->colour[0], ptr->colour[1], ptr->colour[2]);
		glVertex3f(ptr->position[0], ptr->position[1], ptr->position[2]);

// Add only on intervals
		VectorAdd(ptr->position, ptr->velocity, ptr->position);

	}

	glEnd(); // 

}


