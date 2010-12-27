#pragma once

#include <iostream>

class EventListener
{
public:
	virtual ~EventListener() {}

	// TODO THIS ISNT WORKING PROPERLY FIX IT!

	void joystickButton(unsigned index, unsigned button, bool down) 
	{
		std::cout << "EventListener::joystickButton" << std::endl;
	}
	void joystickAxis(unsigned index, unsigned axis, float value)	
	{
		std::cout << "EventListener::joystickAxis" << std::endl;
	}

	void keyPress(unsigned key, unsigned mod, bool down)
	{
		std::cout << "EventListener::keyPress" << std::endl;
	}


	// Only supporting one at the moment
	void mouseButton(unsigned button, bool down)
	{
		//std::cout << "EventListener::mouseButton" << std::endl;
	}

	void mouseMove(unsigned x, unsigned y,unsigned xrel, unsigned yrel)
	{
		//std::cout << "EventListener::mouseMove" << std::endl;
	}

};
