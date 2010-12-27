#include "StateMachine.h"

#include "App.h"
#include "State.h"
#include <cassert>

#include <stdio.h> // Dont know why - for null

StateMachine::StateMachine(void) : currentState_(NULL), nextState_(NULL)
{
	App::instance().add(this);
}

StateMachine::~StateMachine(void)
{
	App::instance().remove(this);
	delete currentState_;
	delete nextState_;
}

void StateMachine::update()
{
	if (nextState_)
	{
		if (currentState_)
		{
			currentState_->end();
			delete currentState_;
		}
		currentState_ = nextState_;
		currentState_->start();
		nextState_ = NULL;
	}
	if (currentState_)
	{
		currentState_->update();
	}
}

void StateMachine::change(State* s)
{
	assert(!nextState_);
	nextState_ = s;
	nextState_->parent_ = this;
}