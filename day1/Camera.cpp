
#include "Camera.h"


#include <iostream>
#include <cassert>


void Camera::update() 
{
	orientation_.set_orientation_zyx(-pitch_, -yaw_, -0);
	vector3 side = orientation_.rotate(vector3(1,0,0));
	vector3 up = orientation_.rotate(vector3(0,1,0));
	forward_ = orientation_.rotate(vector3(0,0,-1));

	modelView_.ident();
	modelView_.translate(-position_);
	
	quaternion invOrientation(orientation_);
	invOrientation.invert();	
	modelView_ *= invOrientation;

}
