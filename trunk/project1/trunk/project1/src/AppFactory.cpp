#include "AppFactory.h"

#include "TestApp.h"


#include <cassert>


TestApp* AppFactory::createApplication()
{
	return new TestApp();
}
