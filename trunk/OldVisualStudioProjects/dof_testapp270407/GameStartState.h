#pragma once
#include "state.h"
#include "EventListener.h"
#include <vector>
class Renderable;
class Joystick;
class DynamicParticle;

class GameStartState :
	public State
{
public:
	GameStartState(Joystick *joy);
	~GameStartState(void);

	void start();
	void end();
	void update();

private:
	std::vector<Renderable*> owned_;

	std::vector<DynamicParticle*> parts_;
	Renderable *test_;
	unsigned started_;
	Joystick* joy_;
};
