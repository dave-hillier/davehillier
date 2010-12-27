#pragma once

class Renderable;

class WindowManager
{
public:
	virtual ~WindowManager() {}

    virtual void resize(unsigned w, unsigned h) = 0;
	virtual void update() = 0;

	virtual void add(Renderable* r) = 0;
	virtual void remove(Renderable* r) = 0;
};

