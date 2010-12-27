#pragma once

#include "Updatable.h"

class State;

class StateMachine : 
	public Updatable
{
public:
	StateMachine(void);
	~StateMachine(void);

	virtual void update();
	virtual void change(State* s);

private:
	State* currentState_;
	State* nextState_;
};
