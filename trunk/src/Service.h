#pragma once

class Service
{
public:
	virtual ~Service() {}

	virtual void update(double timeNow) = 0;

};
