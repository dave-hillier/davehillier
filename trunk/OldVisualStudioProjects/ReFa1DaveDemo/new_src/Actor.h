
#pragma once

#include "../Vector3.h"
#include <string>

class Actor  
{
public:
	Actor();
	virtual ~Actor();

	// management
	bool expired;

	// do nothing
	virtual void Update() {};
	virtual void Render(); // draws bounds


	bool TraceActor(Vector3f pick_start, Vector3f pick_dir);
	bool Visible(Actor *target);
	bool InFront(Actor *target);

	// TODO Encapsulate
	Vector3f org, vel, acc, grav, ang;
	float mass;
	Bounds bounds;
	std::string name;
	float time_stamp;
	int health, damage;
};
