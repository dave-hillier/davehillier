#include "JoystickManager.h"
#include "Joystick.h"
#include "DebugLog.h"
#include <Windows.h>
#include <cassert>

JoystickManager *JoystickManager::instance_ = NULL;

JoystickManager::JoystickManager()
{
	resetManager();
}

JoystickManager::~JoystickManager()
{
	clearJoysticks();
}


JoystickManager& JoystickManager::instance() 
{
	// Not thread safe
	if (!instance_)
	{
		instance_ = new	JoystickManager;
	}
	assert(instance_);
	return *instance_;
}

void JoystickManager::resetManager() 
{
	clearJoysticks();
	const unsigned int maxSticks = 15;

	debugLog << "JoystickManager::resetManager" << std::endl;
	for ( unsigned int i =0; i < maxSticks; ++i )
	{
		// Only use the 4 axis joysticks
		JOYCAPS jc;
		MMRESULT result = joyGetDevCaps(i, &jc, sizeof(jc));
		if ((JOYERR_NOERROR == result) && (jc.wNumAxes >= 4))
		{
			joysticks_.push_back(new Joystick(i));
			debugLog << "\tfound joystick " << i << std::endl;
		}
	}

}

void JoystickManager::update(double time)
{
	bool change = false;
	for (unsigned int i = 0; i < joysticks_.size(); ++i)
	{
		if (!joysticks_[i]->update(time))
		{
			change = true;
		}
	}
}

void JoystickManager::clearJoysticks()
{
	for (unsigned int i = 0; i < joysticks_.size(); ++i)
	{
		delete joysticks_[i];
	}
	joysticks_.clear();
	debugLog << "JoystickManager::clearJoysticks" << std::endl;

}

Joystick *JoystickManager::getJoystick(size_t index) const 
{ 
	if (numberOfJoysticks() > index) 
	{
		return joysticks_[index];
	}
	else
	{
		debugLog << "JoystickManager::getJoystick error " << index << " has " << numberOfJoysticks() << std::endl;
		return NULL;
	}
}