#pragma once

#include "Updatable.h"

class StateMachine;

class State : public Updatable
{
public:
	virtual ~State() {};
	virtual void start() = 0;
	virtual void end() = 0;
	virtual void update() = 0;

	void changeToNewState(State* s);
private:
	friend StateMachine;
	StateMachine *parent_;
};