#pragma once

#include <cassert>
#include <list>

class Actor;

// TODO Change to environment?
// Singleton? for now?

class Environment  
{
public:

	static Environment& instance() 
	{
		static Environment env;
		return env;
	}

	float scale() const { return scale_; }

	void Delete (Actor *p);
	void Render ();
	void Update ();
	bool add(Actor *u);

	// TODO move player out and into a seperate class
	void player (Actor *unit)
	{
		assert(player_unit == NULL);
		player_unit = unit;
	}

	Actor *player ()
	{
		assert(player_unit != NULL);
		return player_unit;
	}

	std::list<Actor*> units; // TODO restrict access to this

private:
	Environment ();

	// why a list? because I want to maintain the iterators across adds
	std::list<Actor*> units_; // TODO restrict access to this

	float scale_;
	Actor *player_unit;

};