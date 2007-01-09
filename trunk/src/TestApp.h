#pragma once

#include <vector>
class RenderManager;
class Timer;
class Service;

class TestApp
{
public:
	TestApp();
	virtual ~TestApp();

	const Timer &mainTimer() const { return *timer_; }
	virtual bool initialise(int argc, char **argv);

//protected:
	virtual void update();
	virtual void screenResize(unsigned int w, unsigned int h);

//private:
	bool createWindow();
	bool mainLoop();
	void render();

protected:

private:

	Timer *timer_;
	RenderManager *renderManager_;


	unsigned int width_;
	unsigned int height_;
	bool fullscreen_;

	std::vector<Service*> services_;

};
