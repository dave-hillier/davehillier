#include "omega_common.h"
#include "omega_gl.h"

/* omega_particles.c

  My first particle system

  Right ok now what you need to do is make decent behaviour, 
  fading, movement, variable textures etc
*/


/*
   0  4  8  12
   1  5  9  13
   2  6  10 14
   3  7  11 15
*/
double nextmove = 0, nextspawn = 0;
extern float fps;
pmanager pManager;
unsigned int starid;
float frametime = 1;
void pTestSystem ();

// external prototype
float CurrentTime ();
extern int		particle_texture;

// TODO: Add to utils
float RandomRange(float lo, float hi)
{
   int r = rand();
   float	x = (float)(r & 0x7fff) / (float)0x7fff;
   return (x * (hi - lo) + lo);
}

void pRender (particle *part)
{
	float m[16];
	vec3_t pts[4], x, y;

	if (!part)
		return;

	glGetFloatv(GL_MODELVIEW_MATRIX, m);// TODO: Should this be accessed here or else where?

	VectorSet(x, m[0], m[4], m[8]);
	VectorSet(y, m[1], m[5], m[9]);
	
	VectorAdd ( x, y, pts[2] );
	VectorInverse2( pts[2], pts[0] );
	VectorSubtract ( x, y, pts[1]);
	VectorInverse2( pts[1], pts[3] );

	VectorScale(pts[0], part->size, pts[0]);
	VectorScale(pts[1], part->size, pts[1]);
	VectorScale(pts[2], part->size, pts[2]);
	VectorScale(pts[3], part->size, pts[3]);
	
	VectorAdd (part->pos, pts[0], pts[0]);
	VectorAdd (part->pos, pts[1], pts[1]);
	VectorAdd (part->pos, pts[2], pts[2]);
	VectorAdd (part->pos, pts[3], pts[3]);

	glDepthMask(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindTexture(GL_TEXTURE_2D, part->texture);

	glColor4f(1.0f, 1.0f, 1.0f, part->alpha);
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(pts[0]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(pts[1]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(pts[2]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(pts[3]);
	glEnd();

	glDepthMask(1);

}
// Not used anymore, creates a single particle in view, should be called from below
void pTest1 ()
{
	particle part;
	
	part.alpha = 1;
	VectorSet(part.pos, 0, 10, 0);
	part.size = 1;

	pRender(&part);	
}
void pRenderSystems ()
{
	psystem *sysptr;
	particle *partptr;

	glEnable(GL_BLEND);

 	// go through the systems

	for (sysptr = pManager.systems; sysptr != NULL; sysptr=sysptr->next)
	{
		if (sysptr->particles)
		{
			for (partptr = sysptr->particles; partptr != NULL; partptr=partptr->next)
				pRender(partptr);
		}
	}
	glDisable(GL_BLEND);

}

void pInitManager ()
{
	VectorSet (pManager.gravity, 0, -1, 0);
	pManager.systems = NULL;
	// temp test// TODO: remove this
//	pTestSystem();
}

particle *pAddParticle (vec3_t origin, vec3_t velocity, 
					   float life, float size, bool gravity, float alpha,
					   particle *parts, unsigned int texid)
{
	particle *part = (particle *)malloc (sizeof(particle));
	VectorCopy(origin, part->pos);
	VectorCopy(velocity, part->vel);

	part->next = parts;
	part->life = CurrentTime() + life;
	part->size = size;
	part->alpha = alpha;
	part->gravity = gravity;
	part->texture = texid;
	part->start = CurrentTime();
	return part;

}
// returns the new set of particle systems
psystem *pAddSystem (pType type, vec3_t origin, int rate, int num, psystem *sys)
{
	psystem *pSys = (psystem *)malloc (sizeof(psystem));

	pSys->next = sys;
	VectorCopy (origin, pSys->pos);
	pSys->particles = NULL;
	pSys->type = type;
	pSys->rate = rate;
	pSys->num = num;
	
	return pSys;
}
void pSpawnParticles (psystem *sys)
{
	int i;

	for (i = 0; i <= sys->rate; i++)
	{
		if (sys->num > 0)
		{
			vec3_t vel = { RandomRange(-10, 10),
							RandomRange(-10, 10),
							RandomRange(-10, 10) };
			if (sys->type == 1)
			{
				sys->particles = pAddParticle(sys->pos, vel, 
					1, RandomRange(2, 10), false, 1, sys->particles, particle_texture-1);
			}
			else
			{
				sys->particles = pAddParticle(sys->pos, vel, 
					1, RandomRange(2, 10), true, 1, sys->particles, particle_texture);
			}
			sys->num--;
		}
	}
}

void pRemoveSystem (psystem *sys)
{
	psystem *sysptr, *sysprev = NULL;

	// go through the systems
	for (sysptr = pManager.systems;
		sysptr != NULL;
		sysprev = sysptr, sysptr = sysptr->next)
	{
		if (sys == sysptr)
		{
			if (sysprev)
				sysprev->next = sysptr->next;
			else
				pManager.systems = sysptr->next;
			free(sysptr);
			return;
		}
	}
}
void pRemoveParticle (particle *part, psystem *sys)
{
	particle *partptr, *partprev = NULL;

	for (partptr = sys->particles; partptr != NULL; partprev = partptr,partptr = partptr->next)
	{
		if (part == partptr)
		{
			if (partprev)
				partprev->next = partptr->next;
			else
				sys->particles = partptr->next;

			free(partptr);
			return;
		}
	}
}
void pRunFrame ()
{
	psystem *sysptr, *sysprev = NULL;
	particle *partptr, *partprev = NULL;
	
	// go through the systems
	for (sysptr = pManager.systems; sysptr != NULL; sysprev = sysptr, sysptr = sysptr->next)
	{
		// check for death
		if ((sysprev) && (sysprev->num <= 0) && (!sysprev->particles))
			pRemoveSystem(sysprev);
		// spawn new particles
		if (sysptr->nextspawn <= CurrentTime())
		{
			pSpawnParticles(sysptr);
			sysptr->nextspawn = CurrentTime() + 0.1f;
		}
		// do each particles
		for (partptr = sysptr->particles; partptr != NULL; partprev = partptr,partptr = partptr->next)
		{
			// Add vel & gravity to the particles position
			// time limit!!!
			if (partptr->nextmove <= CurrentTime())
			{
				if (partptr->gravity)
				{
					vec3_t grav;
					float fall = (float)(CurrentTime() - partptr->start)*9.81f;
					// lol 9.81 as if its realistic or something
					VectorCopy(pManager.gravity, grav);
					VectorScale (grav, fall, grav);
					VectorAdd(partptr->pos, grav, partptr->pos);
				}

				VectorAdd(partptr->pos, partptr->vel, partptr->pos);

				partptr->nextmove = CurrentTime() + 0.05f;	
			}
			// fade colours
			if ((partprev) && (partprev->life <= CurrentTime()))
				pRemoveParticle(partprev, sysptr);
			// check life
		}
	}		
}
void pTestSystem ()
{
	vec3_t pos;
	VectorSet(pos, 0, 0, 0);

	pManager.systems = pAddSystem(0, pos, 100, 10000, pManager.systems);
}

