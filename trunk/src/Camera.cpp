
#include "Camera.h"
#include "Mouse.h"
#include "DebugLog.h"
#include "Font.h"
#include "Joystick.h"
Joystick j;

#include <iostream>

void Camera::initialise()
{
	forward_ = vector3(0,0,-1);
}




void Camera::update(double timeNow) 
{
	j.update(timeNow);
	yaw_ += j.axis(0) * 0.0005f;
	pitch_ += -j.axis(1) * 0.0005f;	
	
	Mouse &m = *Mouse::instance();

	if (m.rightButton())
	{
		yaw_ += m.relativeX() * 0.005f;
		pitch_ += m.relativeY() * 0.005f;
	}
	
	orientation_.set_orientation_xyz(pitch_, yaw_, 0);
	orientation_.invert();

	vector3 side = orientation_.rotate(vector3(1,0,0));
	vector3 up = orientation_.rotate(vector3(0,1,0));
	forward_ = orientation_.rotate(vector3(0,0,-1));

	modelView_.ident();
	modelView_ = orientation_;
	modelView_.translate(position_);
	modelView_.invert();		// TODO with proper maths, this can be removed... hopefully!

	float delta = (float)(timeNow - lastUpdate_);
	delta = delta < 1000 ? delta : 1000;
	//if (m.leftButton())
	//{
	//	position_ += forward_ * delta * 5;
	//}
	//if (m.middleButton())
	//{
	//	position_ += forward_ * -delta * 5;
	//}
    
	position_ += forward_ * -delta * j.axis(3) * 10;
	position_ += side * delta * j.axis(2) * 10;


	lastUpdate_ = timeNow;
}
