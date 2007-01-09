#include "AppFactory.h"

#include "TestApp.h"
#include "RenderManager.h"

#include <cassert>

RenderManager* AppFactory::createRenderManager()
{
	RenderManager* result = new RenderManager;
	bool init = result->initialise();
	assert(init);
	return result;
}
TestApp* AppFactory::createApplication()
{
	return new TestApp;
}
