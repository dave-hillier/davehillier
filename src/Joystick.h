#pragma once

class JoystickManager;

class Joystick
{
public:
	Joystick(unsigned int port);
	~Joystick();

	unsigned int numberAxis() const { return nAxis; }
	float axis(unsigned int i) const { return axis_[i]; } 
	unsigned int numberButtons() const { return nButtons; }
	bool button(unsigned int i) const { return buttons_[i]; }

protected:
	friend JoystickManager;
	bool update(double timeNow);

private:
	enum {
		nButtons = 16,
		nAxis = 4
	};
	float axis_[nAxis];
	bool buttons_[nButtons];
	int pad_;
};
