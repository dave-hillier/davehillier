#include "Resourceman.h"

ResourceMan::ResourceMan(void)
{
}

ResourceMan::~ResourceMan(void)
{
}

void ResourceMan::queue(const char *resourceName)
{
	// TODO attempt to get the lock of the resources
	// if locked then load
}

bool ResourceMan::ready(const char *resourceName)
{
	// TODO does this require a lock? probably
	// Check the status
	return false;

}

void ResourceMan::update()
{

}