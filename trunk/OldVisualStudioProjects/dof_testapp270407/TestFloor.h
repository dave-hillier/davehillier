#pragma once
#include "Renderable.h"

class TestFloor :
	public Renderable
{
public:
	TestFloor(void);
	~TestFloor(void);

	virtual void render();

};
