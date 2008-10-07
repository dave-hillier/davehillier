#pragma once

#include "Service.h"

class Mouse : public Service
{
	Mouse();
public:
	~Mouse();

	static Mouse *instance() 
	{
		static Mouse m;
		return &m;
	}

	bool initialise(){return false;}

	void update(double timeNow);

	int absoluteX() const { return x_; }
	int absoluteY() const { return y_; }

	int relativeX() const { return x_ - lastX_; }
	int relativeY() const { return y_ - lastY_; }


	bool leftButton() const { return left_; }
	bool middleButton() const { return middle_; }
	bool rightButton() const { return right_; }


	// TODO cursor controls
	void visible(bool vis);

private:
	int x_;
	int y_;

	int lastX_;
	int lastY_;

	bool left_;
	bool middle_;
	bool right_;
};
