#include "Joystick.h"
#include <SDL_joystick.h>
#include <assert.h>
#include <stdlib.h>


Joystick::Joystick(int port) : port_(port), joy_(NULL)
{
	int num = SDL_NumJoysticks();
	assert(num > 0 && num >= port);
	joy_=SDL_JoystickOpen(port);

	SDL_JoystickUpdate();

	int hats =SDL_JoystickNumHats(joy_);
	int axes =SDL_JoystickNumAxes (joy_);
	int balls =SDL_JoystickNumBalls(joy_);
	int buttons =SDL_JoystickNumButtons(joy_);
	// TODO ensure that theres enough axes
}

Joystick::~Joystick()
{
	if(SDL_JoystickOpened(port_))
		SDL_JoystickClose(joy_);
}

void Joystick::Update()
{
	// TODO only update once for all joysticks
	SDL_JoystickUpdate();
}

float Joystick::Axis(int i) const
{
	float f = SDL_JoystickGetAxis(joy_, i) / 32768.f;
	const float deadZone = 0.25;
	const bool centre = ( f < 0 && f > -deadZone) || ( f > 0 && f < deadZone);
	if (centre)
		f = 0;
	else
	{
		f += (f > 0) ? -deadZone : deadZone;
		f *= 1/(1-deadZone);
	}

	return f;
}
float Joystick::AxisLeftX() const
{
	return Axis(0);
}

float Joystick::AxisLeftY() const
{
	return -1*Axis(1);
}

float Joystick::AxisRightX() const
{
	return Axis(2);
}
float Joystick::AxisRightY() const
{
	return -1*Axis(3);
}

bool Joystick::Button(int button) const
{
	return SDL_JoystickGetButton(joy_, button) != 0;
}