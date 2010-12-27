#pragma once

class WindowManager;
class State;
class Updatable;
class EventListener;

class App
{
public:
	virtual ~App() {}

	virtual void update() = 0;
	virtual void setState(State* s) = 0;

	virtual WindowManager &getWindowManager() const = 0;

	virtual void add(Updatable* r) = 0;
	virtual void remove(Updatable* r) = 0;

	virtual void add(EventListener* r) = 0;
	virtual void remove(EventListener* r) = 0;


	static App &instance() {
		return *globalApp;
	};

protected:
	virtual WindowManager *createWindowManager() = 0;
	virtual State *createStartState() = 0;

	static App *globalApp;

};

