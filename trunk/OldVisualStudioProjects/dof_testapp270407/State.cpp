#include "State.h"
#include "StateMachine.h"


void State::changeToNewState(State* s)
{
	parent_->change(s);
}
