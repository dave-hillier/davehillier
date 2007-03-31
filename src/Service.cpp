
#include "Service.h"

#include "TestApp.h" 

Service::Service()
{
	globalApp().add(this);
}

Service::~Service() 
{
}
