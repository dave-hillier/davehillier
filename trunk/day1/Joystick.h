#ifndef JOYSTICK_H
#define JOYSTICK_H

//#pragma once
//#include "updatable.h"

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;


// TODO change to event listener rather than a poll
class Joystick // : public Updatable
{
public:
	static unsigned numberOfJoysticks();

	Joystick(unsigned index);
	virtual ~Joystick(void);

	unsigned numberOfAxes() const;
	unsigned numberOfButtons() const;
	// SDL_JoystickNumHats 
	float axis(unsigned int i) const { return axis_[i]; } 
	bool button(unsigned int i) const { return buttons_[i]; }


	void update();

private:
	SDL_Joystick *joy_;
	unsigned index_;

	enum {
		nButtons = 12,
		nAxis = 4
	};
	float axis_[nAxis];
	bool buttons_[nButtons];
};
#endif



