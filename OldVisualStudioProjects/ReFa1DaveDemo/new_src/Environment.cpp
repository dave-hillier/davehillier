
#include "Environment.h"
#include "Actor.h"
#include "../Terrain.h"



bool Environment::add(Actor *u)
{
	units_.push_back(u);
	return true;
}

void Environment::Render ()
{
	// for each unit
	for (std::list<Actor*>::iterator itt = units_.begin(); itt != units_.end(); ++itt)
	{
		Actor *u = *itt;
		u->Render();
	}
}
void Environment::Update ()
{
	for (std::list<Actor*>::iterator itt = units_.begin(); itt != units_.end(); )
	{
		Actor *u = *itt;


		if (u->org.z > terrain.height-1)
			u->org.z = (float)terrain.height-1;
		else if (u->org.z < 0.0f)
			u->org.z = 0.0f;
		if (u->org.x > terrain.width-1)
			u->org.x = (float)terrain.width-1;
		else if (u->org.x < 0.0f)
			u->org.x = 0.0f;

		u->Update();

		if ( u->expired )
		{
			delete *itt;
			itt = units_.erase(itt);
		}
		else
			++itt;
	}
}

Environment::Environment () : scale_(0.0034f)
{
}



