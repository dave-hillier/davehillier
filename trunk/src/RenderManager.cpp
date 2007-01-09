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


// TODO Two scene graphs, one for 
class Renderable
{
	virtual ~Renderable() {}
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

	testCamera_ = new Camera;
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

void RenderManager::renderOrthographic()
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

vector3 testPoints[] = {
	vector3(-10, -10, -10),
	vector3(-10, 10, -10),
	vector3(10, 10, -10),
	vector3(10, -10, -10),
};

class TestRenderThing
{
public:

	TestRenderThing(const vector3 &pos) : position_(pos)
	{
		for (int i = 0; i < 4; ++i)
		{
			points_.push_back(testPoints[i]);
		}
		//orientation_.set_orientation_xyz(0,0.5f,0);


		occluder_= new Occluder(position_, orientation_, points_);
	}
	~TestRenderThing()
	{
		delete occluder_;
	}

	void render() const
	{
		glPushMatrix();
		matrix44 m = orientation_;
		m.invert();
		glTranslatef(position_.x, position_.y, position_.z);
		glMultMatrixf((GLfloat*)m.m);
		glBegin(GL_LINE_STRIP);
		for (std::vector<vector3>::const_iterator itt = points_.begin(); itt != points_.end(); ++itt)
		{
			glVertex3f( itt->x, itt->y, itt->z );
		}
		glVertex3f( points_[0].x, points_[0].y, points_[0].z );
		glEnd();
		glPopMatrix();
	}

	Occluder *occluder_;
	std::vector<vector3> points_;
	vector3 position_;
	quaternion orientation_;
};

TestRenderThing tests[] = 
{
	TestRenderThing(vector3(0, 0, -10)),
	TestRenderThing(vector3(0, 0, -30)),
	TestRenderThing(vector3(0, 0, -50)),
	TestRenderThing(vector3(0, 0, -70)),
	TestRenderThing(vector3(0, 0, -90)),
	TestRenderThing(vector3(0, 0, -110)),
	TestRenderThing(vector3(0, 0, -130)),
	TestRenderThing(vector3(0, 0, -150)),
	TestRenderThing(vector3(0, 0, -170)),
	TestRenderThing(vector3(0, 0, -190)),
	TestRenderThing(vector3(0, 0, -210)),
	TestRenderThing(vector3(0, 0, -230)),
	TestRenderThing(vector3(0, 0, -250)),
	TestRenderThing(vector3(0, 0, -270)),
	TestRenderThing(vector3(0, 0, -290)),
	TestRenderThing(vector3(0, 0, -310)),

	TestRenderThing(vector3(30, 0, -10)),
	TestRenderThing(vector3(30, 0, -30)),
	TestRenderThing(vector3(30, 0, -50)),
	TestRenderThing(vector3(30, 0, -70)),
	TestRenderThing(vector3(30, 0, -90)),
	TestRenderThing(vector3(30, 0, -110)),
	TestRenderThing(vector3(30, 0, -130)),
	TestRenderThing(vector3(30, 0, -150)),
	TestRenderThing(vector3(30, 0, -170)),
	TestRenderThing(vector3(30, 0, -190)),
	TestRenderThing(vector3(30, 0, -210)),
	TestRenderThing(vector3(30, 0, -230)),
	TestRenderThing(vector3(30, 0, -250)),
	TestRenderThing(vector3(30, 0, -270)),
	TestRenderThing(vector3(30, 0, -290)),
	TestRenderThing(vector3(30, 0, -310)),


	TestRenderThing(vector3(-30, 0, -10)),
	TestRenderThing(vector3(-30, 0, -30)),
	TestRenderThing(vector3(-30, 0, -50)),
	TestRenderThing(vector3(-30, 0, -70)),
	TestRenderThing(vector3(-30, 0, -90)),
	TestRenderThing(vector3(-30, 0, -110)),
	TestRenderThing(vector3(-30, 0, -130)),
	TestRenderThing(vector3(-30, 0, -150)),
	TestRenderThing(vector3(-30, 0, -170)),
	TestRenderThing(vector3(-30, 0, -190)),
	TestRenderThing(vector3(-30, 0, -210)),
	TestRenderThing(vector3(-30, 0, -230)),
	TestRenderThing(vector3(-30, 0, -250)),
	TestRenderThing(vector3(-30, 0, -270)),
	TestRenderThing(vector3(-30, 0, -290)),
	TestRenderThing(vector3(-30, 0, -310)),

	TestRenderThing(vector3(-60, 0, -10)),
	TestRenderThing(vector3(-60, 0, -30)),
	TestRenderThing(vector3(-60, 0, -50)),
	TestRenderThing(vector3(-60, 0, -70)),
	TestRenderThing(vector3(-60, 0, -90)),
	TestRenderThing(vector3(-60, 0, -110)),
	TestRenderThing(vector3(-60, 0, -130)),
	TestRenderThing(vector3(-60, 0, -150)),
	TestRenderThing(vector3(-60, 0, -170)),
	TestRenderThing(vector3(-60, 0, -190)),
	TestRenderThing(vector3(-60, 0, -210)),
	TestRenderThing(vector3(-60, 0, -230)),
	TestRenderThing(vector3(-60, 0, -250)),
	TestRenderThing(vector3(-60, 0, -270)),
	TestRenderThing(vector3(-60, 0, -290)),
	TestRenderThing(vector3(-60, 0, -310)),
}; 

void RenderManager::renderPerspective()
{

	matrix44 projection;
	glGetFloatv(GL_PROJECTION_MATRIX,(GLfloat*)projection.m);

	matrix44 modelView = testCamera_->modelView();
	glLoadMatrixf((GLfloat*)modelView.m);

	Frustum f(projection, modelView);
	bool out = f.outside(vector3(0,0,0));
	
	const vector3 &eye = testCamera_->position();
	float maxI = 0;
	Occluder *occluderToUse = NULL;
	for (int i = 0; i < 16*4; ++i)
	{
		float influence = tests[i].occluder_->influence(eye);
		if ( influence >= maxI)
		{
			maxI = influence;
			occluderToUse = tests[i].occluder_;
		}

	}
	if (occluderToUse)
	{
		occluderToUse->update(eye);
	}

	for (int i = 0; i < 16*4; ++i)
	{
		bool dontOcclude = occluderToUse == tests[i].occluder_;
		bool render = occluderToUse == NULL || dontOcclude;
		if (occluderToUse)
		{
			if (occluderToUse->outside(tests[i].position_))
			{
				render = true;
			}
		}
		
		if (dontOcclude)
			glColor3f(0,1,0);
		else if (render)
			glColor3f(1,1,1);
		else
			glColor3f(1,0,0);

		tests[i].render();
	}

	// Set the background
	//float backgroundColour[] = {0.f,0.5f,0.6f, 1};
	//glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
	//glFogfv(GL_FOG_COLOR, backgroundColour);
	//glFogf(GL_FOG_DENSITY, 1.f);
	//glHint(GL_FOG_HINT, GL_DONT_CARE);
	//glFogf(GL_FOG_START, near_+10);
	//glFogf(GL_FOG_END, far_);
	//glFogi(GL_FOG_MODE, GL_LINEAR);

	//glEnable(GL_FOG);

	// Draw some cubes
	/*
	glPushMatrix();
	if (out)
		glColor3f(0.9f,0.9f,0.9f);
	else
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
	//glDisable(GL_FOG);*/

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
	testCamera_->update(timeNow);
	fps_ = 1.f/(float)(timeNow - lastUpdate_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderPerspective();

	startOrthographic(width_, height_, -100, 100);
	renderOrthographic();
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