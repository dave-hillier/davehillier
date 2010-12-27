#pragma once

class Joystick
{
public:
	Joystick(void);
	~Joystick(void);
	void update(double timeNow);

	unsigned int numberAxis() const { return nAxis; }
	float axis(unsigned int i) const { return axis_[i]; } 
	unsigned int numberButtons() const { return nButtons; }
	bool button(unsigned int i) const { return buttons_[i]; }

private:
	enum {
		nButtons = 16,
		nAxis = 4
	};
	float axis_[nAxis];
	bool buttons_[nButtons];
	int pad_;
};
