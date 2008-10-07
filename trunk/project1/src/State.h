#pragma once

#include "Service.h"

class State : public Service
{
public:
	virtual ~State() = 0;
	
	// ?
};


class MainMenu : public State
{
public:
	// TODO
	// Create the world,
	// Set the camera (pos/path)
    // wait for key presses?	
	// Can go to: exit, in game, ???
};

class InGame : public State
{
public:
	// ?
	// ?


};

