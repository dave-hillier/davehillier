#include "Emitter.h"
#include "RenderManager.h"
#include "TestApp.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <cassert>
#include "DebugLog.h"
#include "Camera.h"

Emitter::Emitter()
{
	life_ = 3;
	rate_ = 1000;
	lastEmitter_ = 0;
	lastUpdate_ = 0;
	pos_ = vector3(0,50,-20);
	vel_ = vector3(0,0,0);
	acc_ = vector3(0,0,0);
	particles_.reserve(10);
	scale_ = 0.1f;
	length_ = 4.f;
	velocityAlign_ = false;
	wireFrame_ = false;

	colourStart_ = vector4(1,1,1,1);
	colourEnd_ = vector4(1,0,0,0);
	globalApp().renderManager().add(this);
}
Emitter::~Emitter()
{
	globalApp().renderManager().remove(this);
}


void Emitter::render(double timeNow) 
{
	glPushMatrix();

	Camera *camera = globalApp().renderManager().camera();
	const vector3 &eye = camera->position();

	vector3 x = camera->modelView().x_component();
	vector3 y = camera->modelView().y_component();

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (std::vector<Particle>::iterator itt = particles_.begin(); itt != particles_.end(); ++itt)
	{
		Particle &particle = *itt;
		particle.setColour(timeNow, life_, colourStart_, colourEnd_);
		if (velocityAlign_)
			particle.renderVelocityAligned(eye, length_, scale_, wireFrame_);
		else
			particle.renderAxisAligned(x,y,scale_,wireFrame_);

	}
	glPopMatrix();
	glDisable(GL_BLEND);

}

void Emitter::removeDeadParticles(double timeNow)
{
	std::vector<Particle>::iterator dead = particles_.end();
	double delta = timeNow - lastUpdate_;
	for (std::vector<Particle>::iterator itt = particles_.begin(); itt != dead; ++itt)
	{

		Particle &p = (*itt);
		if (p.timeStamp_ + life_ < timeNow)
		{
			std::swap(p, *(--dead));
		}
		if (itt == dead)
		{
			break;
		}
	}
	if (dead != particles_.end())
	{
		particles_.erase(dead, particles_.end());
	}
}

void Emitter::createNewParticles(double timeNow)
{
	double elapsedTime = timeNow - lastEmitter_;
	int n = (int)floor(elapsedTime * rate_);
	if (n > 0)
	{
		particles_.reserve(particles_.size()+n);
		for (int i = 0; i < n; ++i)
		{
			vector3 vel(1.0f*(rand() - 32767/2),
				1.0f*(rand() - 32767/2),
				1.0f*(rand() - 32767/2));

			vel.norm();
			vel *= 20;

			Particle p(pos_, vel, timeNow);
			particles_.push_back(p);
		}
		lastEmitter_= timeNow;
	}
}

void Emitter::update(double timeNow)
{
	double delta = timeNow - lastUpdate_;

	removeDeadParticles(timeNow);
	createNewParticles(timeNow);

	// Update all the particles
	for (std::vector<Particle>::iterator itt = particles_.begin(); itt != particles_.end(); ++itt)
	{
		Particle &p = (*itt);

		vector3 force(0,-9.81f,0);
		p.update(delta, force);
	}

	lastUpdate_= timeNow;
}
