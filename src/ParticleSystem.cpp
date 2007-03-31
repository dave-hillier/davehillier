#include "ParticleSystem.h"
#include "Emitter.h"

#include <iostream>


#include "TestApp.h"



ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::initialise()
{
	emitter_.push_back(new Emitter());
	return true;
}


void ParticleSystem::update(double timeNow)
{
	for (std::vector<Emitter*>::iterator itt = emitter_.begin(); itt != emitter_.end(); ++itt)
	{
		(**itt).update(timeNow);
	}
}

