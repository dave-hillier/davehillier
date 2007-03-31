#pragma once

#include "SimulationManager.h"	// TODO only DynamicObjectBase required for this header
#include "RenderManager.h"		// TODO only Renderable required for this header



class Entity :  public DynamicObjectBase
{
public:
	Entity();
	virtual ~Entity();

	virtual void collide(CollisionInterface *);
	virtual void collide();

	virtual void update(double time);

private:

	Renderable *internal_;

	bool onFloor_;
};
