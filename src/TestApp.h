#pragma once

#include <vector>

class RenderManager;
class SimulationManager;
class Timer;
class Service;
// TODO create a service manager, make things services


class TestApp
{
public:
	TestApp();
	virtual ~TestApp();

	const Timer &mainTimer() const { return *timer_; }
	virtual bool initialise(int argc, char **argv);

	bool add(Service *s);

//protected:
	virtual void update();
	virtual void screenResize(unsigned int w, unsigned int h);

//private:
	bool createWindow();
	bool mainLoop();
	void render();


	RenderManager &renderManager() const { return *renderManager_; }
	SimulationManager &simulationManager() const { return *simulationManager_; }

protected:

private:

	Timer *timer_;
	RenderManager *renderManager_;
	SimulationManager *simulationManager_;

	unsigned int width_;
	unsigned int height_;
	bool fullscreen_;


	double nextUpdate_;
	double lastUpdate_;


	std::vector<Service*> services_;
};

TestApp& globalApp();
