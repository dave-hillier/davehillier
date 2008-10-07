/*
 *  Imposter.h
 *  Day1
 *
 *  Created by Dave Hillier on 25/06/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
class Renderable;
class Camera;

class Imposter
{
public:
	Imposter(){}
private:
	
	void update(Renderable *obj, Camera* cam)
	{
		//obj
		//cam
		
		// project obj's bound box into screen space 
		// get the extents of this box
		// unproject 
		// set the frustum to around the box. near plane = imposter, far = near+bounding box
		// point the camera at the centre of the imposter
	}
	
	// render
	// do we need to update
	
};