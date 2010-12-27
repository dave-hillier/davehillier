#pragma once

#include <vector>

class Resource;

class ResourceFactory
{
public:
	ResourceFactory();
	~ResourceFactory();

	Resource *create(const char *name);

private:
	std::vector<Resource*> cache_;
};
