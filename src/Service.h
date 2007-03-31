#pragma once

class Service
{
public:

	Service();
	virtual ~Service();

	virtual bool initialise() = 0;
	virtual void update(double timeNow) = 0;

};
