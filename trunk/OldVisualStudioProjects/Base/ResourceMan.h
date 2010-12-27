#pragma once
#include <deque>
//#include <boost/thread/mutex.hpp>

class ResourceMan
{
public:
	ResourceMan(void);
	~ResourceMan(void);

	void queue(const char *resourceName);
	bool ready(const char *resourceName);
	
	void update();
	// TODO a special resource pointer, which points to the resource when its 
	// ready or otherwise to the default of its type

	struct Res
	{
		const char *name;
		bool ready;

	};

	std::deque<Res*> res;
};
