#include "Joystick.h"
#include <cassert>
#include <SDL.h>
#include <iostream>
#include <SDL_joystick.h>
#include "DebugLog.h"
#include "App.h"
#include <math.h>

Joystick::Joystick(unsigned index) : index_(index)
{
	joy_ = SDL_JoystickOpen(index);
	App::instance().add(this);
	assert(joy_);
	update();
}

Joystick::~Joystick(void)
{
	if(SDL_JoystickOpened(0))
		SDL_JoystickClose(joy_);
	App::instance().remove(this);
}

unsigned Joystick::numberOfJoysticks()
{
	static bool init = false;
	if (!init)
	{
		init = true;
		int res =  SDL_JoystickEventState(SDL_ENABLE);
	}
	return SDL_NumJoysticks();
}

unsigned Joystick::numberOfAxes() const
{
	return SDL_JoystickNumAxes(joy_);
}

unsigned Joystick::numberOfButtons() const
{
	return SDL_JoystickNumButtons(joy_);
}

void Joystick::update()
{
// TODO this doesnt need to be as high res as the other things...
	for (unsigned i = 0; i < numberOfAxes(); ++i)
	{
		axis_[i] = SDL_JoystickGetAxis(joy_, i)/-32768.f;
		axis_[i] = fabsf(axis_[i]) > 0.05f ? axis_[i] : 0.0f;
	}

	for (unsigned i = 0; i < numberOfButtons(); ++i)
	{
		buttons_[i] = SDL_JoystickGetButton(joy_, i) != 0;
	}
}
