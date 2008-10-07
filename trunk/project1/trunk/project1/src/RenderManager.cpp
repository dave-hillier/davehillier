#include "RenderManager.h"

#include "DebugLog.h"


#include <cassert>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#define GLH_EXT_SINGLE_FILE 1

#include <glh/glh_extensions.h>
#include <glh/glh_genext.h>
#include <sstream>
#include <iostream>

#include "maths/matrix.h"

#include "Font.h"
#include "Camera.h"
#include "Occluder.h"
#include "Frustum.h"


inline std::ostream& operator<< (std::ostream& os, const vector3 &v )
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")"; 
	return os;
}	

namespace {

	// TODO move to utility
	void drawLineCube(float size)
	{
		float l = size/2;
		glBegin(GL_LINE_STRIP);
		// Front
		glVertex3f( l,  l, l);
		glVertex3f( l, -l, l);
		glVertex3f(-l, -l, l);
		glVertex3f(-l,  l, l);
		glVertex3f( l,  l, l);
		// back
		glVertex3f( l,  l, -l);
		glVertex3f( l, -l, -l);
		glVertex3f(-l, -l, -l);
		glVertex3f(-l,  l, -l);
		glVertex3f( l,  l, -l);
		glEnd();

		// missing edges
		glBegin(GL_LINES);
		glVertex3f( l, -l, -l);
		glVertex3f( l, -l,  l);
		glVertex3f(-l, -l, -l);
		glVertex3f(-l, -l,  l);
		glVertex3f(-l,  l, -l);
		glVertex3f(-l,  l,  l);
		glEnd();

	}
}

//class vector3;
//class quaternion;
class sphere;

#include "JoystickManager.h"
#include "Joystick.h"

class ChaseThing
{
public:
	virtual const vector3 &position() const = 0;
	virtual const quaternion &orientation() const = 0;
};

class StaticCamera : public Camera
{
public:
	// TODO set pos, and orientation
	void update(double timeNow) 
	{
		orientation_.set_orientation_zyx(-pitch_, -yaw_, -0);
		modelView_.ident();
		modelView_.translate(-position_);
		quaternion invOrientation(orientation_);
		invOrientation.invert();	
		modelView_ *= invOrientation;
		lastUpdate_ = timeNow;
	}
};

class EyeCamera : public Camera
{
public:
	EyeCamera(ChaseThing *thing) : thing_(thing) {}
	void update(double timeNow) 
	{
		orientation_ = thing_->orientation();
		position_ = thing_->position();

		modelView_.ident();
		modelView_.translate(-position_);
		quaternion invOrientation(orientation_);
		invOrientation.invert();	
		modelView_ *= invOrientation;
		lastUpdate_ = timeNow;
	}
private:
	ChaseThing *thing_;
};
#include "Entity.h"
Entity *ent1=NULL;

class ChaseCamera : public Camera
{
public:
	ChaseCamera(Entity *thing) : thing_(thing) { }
	void update(double timeNow) 
	{
		quaternion q = thing_->orientation();

		vector3 back = q.rotate(vector3(0,0,-1));
		vector3 up = q.rotate(vector3(0,1,0));
		position_ = thing_->position() - (back * 10) + (up * 3) ;

		orientation_;//.set(-back.x, -back.y, -back.z, 0);

		modelView_.ident();
		modelView_.translate(-position_);
		quaternion invOrientation(orientation_);
		invOrientation.invert();	
		modelView_ *= invOrientation;
		lastUpdate_ = timeNow;
	}
private:
	Entity *thing_;
};

// TODO move to utility
void RenderManager::checkError(const char* where)
{
	GLenum error;

	while ((error = glGetError()) != GL_NO_ERROR) {
		debugLog << "RenderManager: Error (" << where << ") " << gluErrorString(error) << std::endl;
	}

	assert(glGetError() == GL_NO_ERROR);
}

RenderManager::RenderManager() : 
	width_(100), 
	height_(100), 
	near_(0.01f), 
	far_(555.f),
	fpsDisplay_(NULL),
	testCamera_(NULL)
{
}

RenderManager::~RenderManager()
{
	delete fpsDisplay_;
	delete testCamera_;
}

bool RenderManager::initialise()
{

	fpsDisplay_ = new Font("Arial", 25, false, false);
	fpsDisplay_->initialise();

	ent1 = new Entity;

	testCamera_ = new ChaseCamera(ent1);
	//testCamera_ = new StaticCamera;
	//testCamera_ = new Camera;
	testCamera_->initialise();

	bool result = true;

	if(!glh_init_extensions(
		"GL_ARB_multitexture "
		"GL_NV_vertex_program "
		"GL_NV_fragment_program "
		"WGL_ARB_pbuffer "
		"WGL_ARB_pixel_format "
		"WGL_ARB_render_texture "
		))	
	{
		debugLog << "Error - required extensions were not supported: " <<  glh_get_unsupported_extensions() << std::endl;		
	}

	glEnable(GL_DEPTH_TEST);


	checkError("initialise");
	return result;
}

void RenderManager::renderOrthographic(double timeNow)
{
	glPushMatrix();
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	std::stringstream str;
	str.precision(5);
	str << "Fps: " << fps_;
	str << " pos " << testCamera_->position();
	fpsDisplay_->render(str.str().c_str(), -200.f, height_/2.f);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	checkError("renderOrthographic");
}


template<int N> void lerp(const float a[N], const float b[N], float c[N], float d)
{
	for (int i =0; i < N; ++i)
	{
		c[i] = a[i]*d + b[i]*(1-d); 
	}
}

void RenderManager::renderPerspective(double timeNow)
{

	matrix44 projection;
	glGetFloatv(GL_PROJECTION_MATRIX,(GLfloat*)projection.m);

	matrix44 modelView = testCamera_->modelView();
	glLoadMatrixf((GLfloat*)modelView.m);

	Frustum f(projection, modelView);
	const vector3 &eye = testCamera_->position();

	// Set the background
	float blueish[4] = {0.f,0.5f,0.6f, 1};
	float black[4] = {0.f,0.f,0.f, 1};
	float backgroundColour[4];

	float distance = 200;
	float altitude = eye.y;
	float v =  min(1-(altitude/distance),1);
	lerp<4>(blueish, black, backgroundColour, v);

	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
	glFogfv(GL_FOG_COLOR, backgroundColour);
	glFogf(GL_FOG_DENSITY, 1.f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, near_+10);
	glFogf(GL_FOG_END, near_+10+distance);	// TODO is it a function of altitude?
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glEnable(GL_FOG);

	for (std::vector<Renderable*>::iterator itt = things_.begin(); itt != things_.end(); ++itt)
	{
		(*itt)->render(timeNow);
	}
	
	glPushMatrix();
	glColor3f(0.1f,0.9f,0.9f);

	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.9f,0.9f,0.1f);
	glTranslatef(-20, 0, -20);
	drawLineCube(10);
	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.9f,0.1f,0.9f);
	glTranslatef(20, 0, -20);
	drawLineCube(10);
	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glTranslatef(0, 0, -20);
	drawLineCube(10);
	glPopMatrix();
	glDisable(GL_FOG);

	//glPushMatrix();
	//const vector3 &f = testCamera_->forward() * 50 + testCamera_->position();
	//matrix44 m = testCamera_->rotateInverse(); // this might be overkill, just setting the identity in the rotation parts could be ok
	//m.translate(f);
	//glMultMatrixf((float*)m.m);
	//glColor3f(1.0f,0.0f,0.0f);
	//drawLineCube(5);
	//glPopMatrix();

	checkError("renderPerspective");
}

void RenderManager::render(double timeNow)
{
	fps_ = 1.f/(float)(timeNow - lastUpdate_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderPerspective(timeNow);

	startOrthographic(width_, height_, -100, 100);
	renderOrthographic(timeNow);
	endOrthographic();

	lastUpdate_ = timeNow;
}

void RenderManager::resize(unsigned int w, unsigned int h)
{
	setPerspective(w, h, near_, far_);
	width_ = w;
	height_ = h;
}

void RenderManager::setPerspective(unsigned int width, unsigned int height, float nearZ, float farZ)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspect = (GLfloat)width/(GLfloat)height;
	matrix44 matrix;
	matrix.perspFovRh(45.0f, aspect, nearZ, farZ);// TODO get the perspective from a camera
	glLoadMatrixf((float*)(matrix.m));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0,0,width,height);
}

void RenderManager::startOrthographic(unsigned int w, unsigned int h, float nearZ, float farZ)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	matrix44 matrix; // TODO get from a camera
	matrix.orthoRh((float)w, (float)h, nearZ, farZ);
	glLoadMatrixf((float*)(matrix.m)); 

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}	

void RenderManager::endOrthographic()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

bool RenderManager::add(Renderable *thing)
{
	bool result = true;
	assert(thing);
	
	things_.push_back(thing);

	return result;
}
bool RenderManager::remove(Renderable *thing)
{
	bool result = false;
	assert(thing);
	assert(false); // TODO implement

	

	return result;
}