#pragma once

#include "Service.h"

#include <vector>
#include <list>
#include <iostream>


class Emitter;

class ParticleSystem : public Service
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	virtual void update(double timeNow);
	virtual bool initialise();

	std::vector<Emitter*> emitter_;

};
