
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "TextureManager.h"
#include "timer.h"
#include "ParticleManager.h"

ParticleManager pman;
static Particle *head;
float time, delta; // saves a func call for each particle

Particle::Particle ()
{
	next = head;
	if (next)
		next->prev = this;

	prev = NULL;
	head = this;

	vel.Clear();
	acc.Clear();
}
Particle::~Particle ()
{
	if (this == head)
		head = next;	
	
	if (next)
		next->prev = prev;

	if (prev)
		prev->next = next;
}

Vector3 x, y;
void Particle::Render()
{
	Vector3 pts[4];
	pts[0] = org + ((-x - y) * size);
	pts[1] = org + ((x - y) * size);
	pts[2] = org + ((x + y) * size);
	pts[3] = org + ((-x + y) * size);
	
	glColor4fv(colour);
	texture.Bind(texid);
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
}

void Particle::Update()
{
	float blend = (time - time_stamp)/life;

	if (blend > 1)
		blend = 1;

	size = (1-blend) * size_start + (blend) * size_end;
	colour[0] = (1-blend) * colour_start[0] + (blend) * colour_end[0];
	colour[1] = (1-blend) * colour_start[1] + (blend) * colour_end[1];
	colour[2] = (1-blend) * colour_start[2] + (blend) * colour_end[2];
	colour[3] = (1-blend) * colour_start[3] + (blend) * colour_end[3];

	org += vel * delta;
	vel += acc * delta;
}

void Delete(Particle *p)
{ 
	if (p == head)
	{
		head = p->next;
		head->prev = NULL;
		delete p;
	}
	else if (p->next == NULL)
	{
		if (p->prev)
			p->prev->next = NULL;

		delete p;
	}
	else
	{
		p->prev->next = p->next;
		p->next->prev = p->prev;
		delete p;
	}
}


void ParticleManager::Update()
{
	Particle *p, *temp = NULL;
	time = timer.GetTime();
	delta = timer.Delta();

	// for each particle
	for (p = head; p; p = p->next)
	{
	// update and check collision
		p->Update();
	}

	for (p = head, temp = NULL; p; p = p->next)
	{

		if (p->life + p->time_stamp < time)
		{
			if (p->prev)
				temp = p->prev;
			else
				temp = p->next;
			
			Delete(p);

			p = temp;
		}
	}
}

ParticleManager::ParticleManager ()
{
}
ParticleManager::~ParticleManager ()
{
}

void ParticleManager::Render()
{
	float mdlview[16]; // static?

	glGetFloatv(GL_MODELVIEW_MATRIX, mdlview);
	
	x.Set(mdlview[0], mdlview[4], mdlview[8]);
	y.Set(mdlview[1], mdlview[5], mdlview[9]);

	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);
	Particle *p;
	for (p = head; p; p = p->next)
		p->Render();
	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}


void ParticleManager::TestSys(Vector3 pos)
{
	Particle *p = new Particle();

	p->org = pos;
	p->size_end = 60;
	p->size_start = 10;

	p->colour_start[0] = 1;
	p->colour_start[1] = 1;
	p->colour_start[2] = 0.5*(rand() % 255)/255.0;
	p->colour_start[3] = 1;

	p->colour_end[0] = 1;
	p->colour_end[1] = 1;
	p->colour_end[2] = 1;
	p->colour_end[3] = 1;

	p->time_stamp = timer.GetTime();
	p->life = 60;
	p->texid = texture.LoadTexture("star.tga");

//	p->acc = Vector3(0, -10, 0);
//	p->vel = Vector3((rand() % 50 - 25),(rand() % 50 - 50),(rand() % 50 - 25));
}


void ParticleManager::Smoke(Vector3 pos)
{
	Particle *p = new Particle();

	p->org = pos;
	p->size_end = 30;
	p->size_start = 12;

	p->colour_start[0] = 1;
	p->colour_start[1] = 1;
	p->colour_start[2] = 1;
	p->colour_start[3] = 1;

	p->colour_end[0] = 1;
	p->colour_end[1] = 1;
	p->colour_end[2] = 1;
	p->colour_end[3] = 0;

	p->time_stamp = timer.GetTime();
	p->life = 1;
	p->texid = texture.LoadTexture("smoke.tga");

/*	p->acc = Vector3(0, -10, 0);
	p->vel = Vector3(
		(rand() % 5 - 2.5),
		(rand() % 5 - 2.5),
		(rand() % 5 - 2.5));*/
}


