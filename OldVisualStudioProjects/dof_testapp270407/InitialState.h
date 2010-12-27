#pragma once
#include "State.h"
#include "EventListener.h"

class InitialState :
	public State, public EventListener
{
public:
	InitialState(void);
	virtual ~InitialState(void);

	void start();
	void end();
	void update();

	void joystickButton(unsigned index, unsigned button, bool down);

private:
	unsigned startTime_;

	
};
