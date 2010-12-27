#include "InitialState.h"
#include "Timer.h"
#include "GameStartState.h"
#include "Joystick.h"
#include "DebugLog.h"
#include "App.h"
#include <iostream>

InitialState::InitialState(void)
{
	App::instance().add((EventListener*)this);
}

InitialState::~InitialState(void)
{
	App::instance().remove((EventListener*)this);
}

void InitialState::start() 
{
	Timer t;
	startTime_ = t.getTime();
}

void InitialState::end() 
{

}

void InitialState::joystickButton(unsigned index, unsigned button, bool down)
{
	Timer t;
	if (t.getTime() > startTime_ + 2*1000)
	{
		Joystick *joystick = new Joystick(index);
		if (joystick->numberOfAxes() >= 4 && joystick->numberOfButtons() >= 12)
		{
			changeToNewState(new GameStartState(joystick));
		}
	}
}


void InitialState::update() 
{
	Timer t;
	if (t.getTime() > startTime_ + 2*1000)
	{
		unsigned n = Joystick::numberOfJoysticks();
		Joystick *joystick = NULL;
		for (unsigned i =0; i < n; ++i)
		{
			joystick = new Joystick(i);
			if (joystick->numberOfAxes() >= 4 && 
				joystick->numberOfButtons() >= 12)
			{
				break;
			}
			delete joystick;
			joystick = NULL;
		}

		if (!joystick)
		{
			debugLog << "Error, no joysticks detected" << std::endl;
		}
		else
		{
			changeToNewState(new GameStartState(joystick));
		}
	}


}