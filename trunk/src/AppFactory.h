#pragma once

// TODO perhaps extract an interface latter - its quite easy, but I cant be arsed now

class RenderManager;
class TestApp;

class AppFactory
{
public:
	AppFactory() {}
	virtual ~AppFactory() {}

	static AppFactory &instance() {
		static AppFactory instance;
		return instance;
	}

	TestApp* createApplication();
	RenderManager* createRenderManager();	// Perhaps this isnt entirely the correct place

};
