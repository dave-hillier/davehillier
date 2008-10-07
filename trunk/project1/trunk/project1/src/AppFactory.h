#pragma once

class SimulationManager;
class RenderManager;
class TestApp;

class AppFactory
{
private:
	AppFactory() {}
public:
	virtual ~AppFactory() {}

	static AppFactory &instance() {
		static AppFactory instance;
		return instance;
	}

	TestApp* createApplication();

};
