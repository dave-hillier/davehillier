
#include <windows.h>
#include <cstdio>
#include <algorithm>
#include <gl/gl.h>
#include <gl/glu.h>
#include "new_src/TextureManager.h"
#include "timer.h"
#include "ParticleManager.h"

ParticleManager pman;
float time, delta; // saves a func call for each particle

Particle::Particle () : texid(0)
{
	time_stamp=1;
	life=1;

	size_start=1;
	size_end=1;

	colour_start[0]=0;
	colour_start[1]=0;
	colour_start[2]=0;
	colour_start[3]=0;
	colour_end[0]=0;
	colour_end[1]=0;
	colour_end[2]=0;
	colour_end[3]=0;



	size=0;
	colour[0]=0;
	colour[1]=0;
	colour[2]=0;
	colour[3]=0;

	org.Clear();
	vel.Clear();
	acc.Clear();
}
Particle::~Particle ()
{

}

Vector3f x, y;
void Particle::Render()
{
	// TODO textures
	Vector3f pts[4];
	pts[0] = org + ((-x - y) * size);
	pts[1] = org + ((x - y) * size);
	pts[2] = org + ((x + y) * size);
	pts[3] = org + ((-x + y) * size);
	
	glColor4fv(colour);

	TextureManager::Instance().Apply(texid);
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(pts[0].vec);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(pts[1].vec);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(pts[2].vec);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(pts[3].vec);
	glEnd();

	//glPointSize(15);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_POINT_SMOOTH);
	//glBegin(GL_POINTS);
	//glColor4fv(colour);
	//glVertex3f(org.x, org.y, org.z);
	//glEnd();
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

void ParticleManager::Update()
{
	time = timer.GetTime();
	delta = timer.Delta();

	// for each particle
	for (std::vector<Particle>::iterator p = parts.begin(); p != parts.end(); )
	{
		p->Update();
		if ((p->life > 0) && (p->life + p->time_stamp < time))
		{
			p = parts.erase(p);
		}
		else
		{
			++p;
		}
	}
}

ParticleManager::ParticleManager ()
{
	parts.reserve(1024);
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
	//glDisable(GL_TEXTURE_2D);
	for (std::vector<Particle>::iterator p = parts.begin(); p != parts.end(); ++p)
	{
		p->Render();
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}


void ParticleManager::TestSys(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = 1;
	p.size_start = 1;

	p.colour_start[0] = 1;
	p.colour_start[1] = 1;
	p.colour_start[2] = 1;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 1;
	p.colour_end[2] = 1;
	p.colour_end[3] = 1;

	p.time_stamp = timer.GetTime();
	p.life = 1000;
//	p.texid = texture.LoadTexture("./pics/star.tga");
//	TextureManager::Instance().Load("./pics/star.tga", p.texid);

}

void ParticleManager::Dirt(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = 0.1f;
	p.size_start = 0.05f;

	p.colour_start[0] = 1;
	p.colour_start[1] = 1;
	p.colour_start[2] = 1;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 1;
	p.colour_end[2] = 1;
	p.colour_end[3] = 0;

	p.time_stamp = timer.GetTime();
	p.life = 0.1f;
//	p.texid = texture.LoadTexture("./pics/smoke.tga");
//	TextureManager::Instance().Load("./pics/smoke.tga", p.texid);
	parts.push_back(p);

}


void ParticleManager::Smoke(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = 0.1f;
	p.size_start = 0.05f;

	p.colour_start[0] = 0;
	p.colour_start[1] = 1;
	p.colour_start[2] = 0;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 0;
	p.colour_end[2] = 0;
	p.colour_end[3] = 1;

	p.time_stamp = timer.GetTime();
	p.life = 1;
//	p.texid = texture.LoadTexture("./pics/smoke.tga");
//	TextureManager::Instance().Load("./pics/smoke.tga", p.texid);

/*	p.acc = Vector3f(0, -10, 0);
	p.vel = Vector3f(
		(rand() % 5 - 2.5),
		(rand() % 5 - 2.5),
		(rand() % 5 - 2.5));*/
	parts.push_back(p);
}

void ParticleManager::Smoke2(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = 0.2f;
	p.size_start = 0.1f;

	p.colour_start[0] = 1;
	p.colour_start[1] = 1;
	p.colour_start[2] = 1;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 1;
	p.colour_end[2] = 1;
	p.colour_end[3] = 0;

	p.time_stamp = timer.GetTime();
	p.life = 1;
//	p.texid = texture.LoadTexture("./pics/smoke.tga");
//	TextureManager::Instance().Load("./pics/smoke.tga", p.texid);

	p.acc = Vector3f(0, 2, 0);
	p.vel = Vector3f(
		(rand() % 5 - 2.5f)/10.0f,
		(rand() % 5 - 2.5f)/10.0f,
		(rand() % 5 - 2.5f)/10.0f);
}

void ParticleManager::Explosion(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = (rand() % 255) / 1024.0f;
	p.size_start = (rand() % 255) / 512.0f;

	p.colour_start[0] = 1;
	p.colour_start[1] = (rand() % 255) / 255.0f;
	p.colour_start[2] = 0;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 1;
	p.colour_end[2] = (rand() % 255) / 255.0f;
	p.colour_end[3] = (rand() % 255) / 255.0f;

	p.time_stamp = timer.GetTime();
	p.life = (rand() % 255) / 255.0f;
//	p.texid = texture.LoadTexture("./pics/star.tga");
//	TextureManager::Instance().Load("./pics/star.tga", p.texid);

	p.acc = Vector3f(0, -1, 0);
	p.vel = Vector3f(
		(rand() % 100 - 50)/25.0f,
		(rand() % 100 - 50)/25.0f,
		(rand() % 100 - 50)/25.0f);
	parts.push_back(p);
}


void ParticleManager::Trail(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = 0.f;
	p.size_start = 0.025f;

	p.colour_start[0] = 1;
	p.colour_start[1] = 0;
	p.colour_start[2] = 0;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 1;
	p.colour_end[2] = 0;
	p.colour_end[3] = 1;

	p.time_stamp = timer.GetTime();
	p.life = 0.1f;
//	p.texid = texture.LoadTexture("./pics/star.tga");
//	TextureManager::Instance().Load("./pics/star.tga", p.texid);

/*	p.acc = Vector3f(0, -10, 0);
	p.vel = Vector3f(
		(rand() % 255 - 128)/1024.0f,
		(rand() % 255 - 128)/1024.0f,
		(rand() % 255 - 128)/1024.0f);*/
	parts.push_back(p);
}


void ParticleManager::Explosion2(Vector3f pos)
{
	Particle p;

	p.org = pos;
	p.size_end = 0;
	p.size_start = 0.05f;

	p.colour_start[0] = 0;
	p.colour_start[1] = 1;
	p.colour_start[2] = 0;
	p.colour_start[3] = 1;

	p.colour_end[0] = 1;
	p.colour_end[1] = 1;
	p.colour_end[2] = 1;
	p.colour_end[3] = 1;

	p.time_stamp = timer.GetTime();
	p.life = 0.5f;
//	p.texid = texture.LoadTexture("./pics/blob.tga");
//	TextureManager::Instance().Load("./pics/blob.tga", p.texid);

	p.acc = Vector3f(0, -1, 0);
	p.vel = Vector3f(
		(rand() % 100 - 50)/25.0f,
		(rand() % 50)/25.0f,
		(rand() % 100 - 50)/25.0f);

	parts.push_back(p);
}

