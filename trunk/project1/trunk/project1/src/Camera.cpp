
#include "Camera.h"
#include "Mouse.h"
#include "DebugLog.h"
#include "Font.h"
#include "Joystick.h"
#include "JoystickManager.h"

#include <iostream>
#include <cassert>

bool Camera::initialise()
{
	forward_ = vector3(0,0,-1);
	joystick_ = JoystickManager::instance().getJoystick(0);

	return true;
}

void Camera::update(double timeNow) 
{
	Joystick &j = *joystick_;
	yaw_ += j.axis(2) * 0.001f;
	pitch_ += -j.axis(3) * 0.001f;	
	
	Mouse &m = *Mouse::instance();

	if (m.rightButton())
	{
		yaw_ += m.relativeX() * 0.005f;
		pitch_ += m.relativeY() * 0.005f;
	}
	
	orientation_.set_orientation_zyx(-pitch_, -yaw_, -0);
	vector3 side = orientation_.rotate(vector3(1,0,0));
	vector3 up = orientation_.rotate(vector3(0,1,0));
	forward_ = orientation_.rotate(vector3(0,0,-1));

	modelView_.ident();
	modelView_.translate(-position_);
	
	quaternion invOrientation(orientation_);
	invOrientation.invert();	
	modelView_ *= invOrientation;

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
    
	position_ += forward_ * -delta * j.axis(1) * 10;
	position_ += side * delta * j.axis(0) * 10;


	lastUpdate_ = timeNow;
}
