#pragma once

#include "Service.h"

#include <vector>

class Joystick;

class JoystickManager : public Service
{
	JoystickManager();
public:
	~JoystickManager();

	void resetManager();

	bool initialise(){return false;}
	void update(double time);

	size_t numberOfJoysticks() const { return joysticks_.size(); }
	Joystick *getJoystick(size_t index) const;

	static JoystickManager& instance();

private:
	void clearJoysticks();
	static JoystickManager *instance_;

	std::vector<Joystick *> joysticks_;
};
