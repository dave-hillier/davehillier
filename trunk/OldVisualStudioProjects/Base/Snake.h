#pragma once

#include "Vector3.h"

class Snake
{
public:
	Snake();
	~Snake();

	void Draw();
	void Update();

private:
	Vector3 pos_;
	float speed_;
	float heading_;
	float headingA_;
};


