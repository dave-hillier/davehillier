
#include "TestWindowManager.h"

//#include <SDL.h>
#include <gl/glee.h>
#include <SDL_opengl.h>

#include "Camera.h"
#include "Frustum.h"
#include "DebugLog.h"
#include "Renderable.h"
#include <cassert>
#include <algorithm>
#include <iostream>

#include "Shader.h"

void checkError(const char* where)
{
	GLenum error;

	while ((error = glGetError()) != GL_NO_ERROR) {
		debugLog << "RenderManager: Error (" << where << ") " << gluErrorString(error) << std::endl;
	}

	assert(glGetError() == GL_NO_ERROR);
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
	// texture dimensions
	int texWidth  = 256;
	int texHeight = 256;

	GLenum texInternalFormat = GL_RGBA8;
	GLenum filterMode = GL_LINEAR;
	int maxCoordS = 1;
	int maxCoordT = 1;
}

// TODO only one can be set as the current!
Shader *colourShader = NULL;
Shader *combine = NULL;
Shader *blurVertical = NULL;
Shader *blurHorizontal = NULL;


class RenderToTexture 
{
public:
	RenderToTexture(unsigned width, unsigned height, bool depth) : width_(width), 
		height_(height), depthUsed_(depth)
	{
		glGenFramebuffersEXT(1, &fbo_);
		glGenTextures(1, &tex_);

		if (depth)
		{
			glGenRenderbuffersEXT(1, &depth_);
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);  

		// init texture
		glBindTexture(GL_TEXTURE_2D, tex_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
			GL_TEXTURE_2D, tex_, 0);

		// initialize depth renderbuffer
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
			GL_RENDERBUFFER_EXT, depth_);

		checkError("RenderToTexture ctor");
	}

	void bind() 
	{
		glBindTexture(GL_TEXTURE_2D, tex_);
	}

	void activate()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
		glPushAttrib(GL_VIEWPORT_BIT); 
		glViewport(0, 0, width_, height_);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void deactivate()
	{
		glPopAttrib(); 
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	unsigned texture() const { return tex_; }
private:
	unsigned int width_;
	unsigned int height_;
	unsigned int fbo_;
	unsigned int depth_;
	unsigned int tex_;
	bool depthUsed_;

};
RenderToTexture *pass1 = NULL;
RenderToTexture *pass2 = NULL;
RenderToTexture *pass3 = NULL;

TestWindowManager::TestWindowManager(void) : camera_(new Camera), near_(0.1f), far_(1000.f)
{
	camera_->position(vector3(0,40,0));
	camera_->initialise();

	checkError("init1");

	// init the frame buffer
	pass1 = new RenderToTexture(1024,1024,true);
	pass2 = new RenderToTexture(256,256,false);
	pass3 = new RenderToTexture(256,256,false);

	glEnable(GL_DEPTH_TEST);

	checkError("init depth buffer");

	// initialise shaders
	colourShader = new Shader();
	colourShader->init("shaders/color.frag", "shaders/color.vert");
	blurVertical = new Shader();
	blurVertical->init("shaders/dof/blur0.frag", "shaders/textureSimple.vert");
	blurHorizontal = new Shader();
	blurHorizontal->init("shaders/dof/blur1.frag", "shaders/textureSimple.vert");
	combine = new Shader();
	combine->init("shaders/dof/combine.frag", "shaders/textureSimple.vert");
	combine->setVariable("tex0", 0);
	combine->setVariable("tex1", 1);

	checkError("init shader");
}

TestWindowManager::~TestWindowManager(void)
{
	delete camera_;
}

void TestWindowManager::resize(unsigned w, unsigned h)
{
	setPerspective(w, h, near_, far_);
	width_ = w;
	height_ = h;
}

void TestWindowManager::update()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	camera_->update();
	renderPerspective();


	startOrthographic(width_, height_, -100, 100);
	renderOrthographic();
	endOrthographic();
}

void TestWindowManager::renderOrthographic()
{

	glEnable(GL_TEXTURE_2D);  

	glColor3f(1.0, 1.0, 1.0);


	glEnable(GL_TEXTURE_2D);  

#if 0
	pass1->bind();
	glPushMatrix();
	//glScalef(width_/2.f,height_/2.f,1.f);
	glTranslatef(-128,0,0);
	glScalef(256/2.f,256/2.f,1.f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(1, 0); glVertex2f( 1, -1);
	glTexCoord2f(1, 1); glVertex2f( 1,  1);
	glTexCoord2f(0, 1); glVertex2f(-1,  1);
	glEnd();
	glPopMatrix();
	
	pass3->bind();
	glEnable(GL_TEXTURE_2D);  
	glPushMatrix();
	//glScalef(width_/2.f,height_/2.f,1.f);
	glTranslatef(128,0,0);
	glScalef(256/2.f,256/2.f,1.f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(1, 0); glVertex2f( 1, -1);
	glTexCoord2f(1, 1); glVertex2f( 1,  1);
	glTexCoord2f(0, 1); glVertex2f(-1,  1);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
#endif
	
#if 1
	combine->useProgram();
	glActiveTexture(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D); 
	pass1->bind();
	glActiveTexture(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);  
	pass3->bind();

	glPushMatrix();
	glScalef(width_/2.f,height_/2.f,1.f);
	//glTranslatef(0,100,0);
	//glScalef(256.f/2,256.f/2,1.f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(1, 0); glVertex2f( 1, -1);
	glTexCoord2f(1, 1); glVertex2f( 1,  1);
	glTexCoord2f(0, 1); glVertex2f(-1,  1);
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);  
	glActiveTexture(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);  
	checkError("dis");

	combine->useFixedFunction();
#endif


	glColor3f(1,1,1);

	glBegin(GL_LINE_STRIP);
	glVertex2f(10, 10);
	glVertex2f(10, -10);
	glVertex2f(-10, -10);
	glVertex2f(-10, 10);
	glVertex2f(10, 10);
	glEnd();
	checkError("test point");

}

void TestWindowManager::renderPerspective()
{
	matrix44 projection;
	glGetFloatv(GL_PROJECTION_MATRIX,(GLfloat*)projection.m);
	matrix44 modelView = camera_->modelView();
	Frustum f(projection, modelView);

	pass1->activate();
	colourShader->useProgram();
	glColor3f(1.0, 1.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glLoadMatrixf((GLfloat*)modelView.m);
	for (std::vector<Renderable*>::iterator itt = renderables_.begin(); itt != renderables_.end(); ++itt)
	{
		(*itt)->render();
	}

	colourShader->useFixedFunction();
	pass1->deactivate();

	// Next pass
	pass2->activate();
	startOrthographic(256,256,-10,10);
	pass1->bind();
	blurVertical->useProgram();
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-128, -128);
	glTexCoord2f(1, 0); glVertex2f(128, -128);
	glTexCoord2f(1, 1); glVertex2f(128, 128);
	glTexCoord2f(0, 1); glVertex2f(-128, 128);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);  
	blurVertical->useFixedFunction();
	endOrthographic();
	pass2->deactivate();

	// Next pass
	pass3->activate();
	startOrthographic(256,256,-10,10);
	pass2->bind();
	blurHorizontal->useProgram();
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-128, -128);
	glTexCoord2f(1, 0); glVertex2f(128, -128);
	glTexCoord2f(1, 1); glVertex2f(128, 128);
	glTexCoord2f(0, 1); glVertex2f(-128, 128);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);  
	blurHorizontal->useFixedFunction();
	endOrthographic();
	pass3->deactivate();

	glDisable(GL_TEXTURE_2D);
	
	/*glColor3f(1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf((GLfloat*)modelView.m);
	for (std::vector<Renderable*>::iterator itt = renderables_.begin(); itt != renderables_.end(); ++itt)
	{
		(*itt)->render();
	}*/
}


void TestWindowManager::setPerspective(unsigned int width, unsigned int height, float nearZ, float farZ)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspect = (GLfloat)width/(GLfloat)height;
	matrix44 matrix;
	matrix.perspFovRh(45.0f, aspect, nearZ, farZ);
	glLoadMatrixf((float*)(matrix.m));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0,0,width,height);
}

void TestWindowManager::startOrthographic(unsigned int w, unsigned int h, float nearZ, float farZ)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	matrix44 matrix;
	matrix.orthoRh((float)w, (float)h, nearZ, farZ);
	glLoadMatrixf((float*)(matrix.m)); 

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

}	

void TestWindowManager::endOrthographic()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void TestWindowManager::add(Renderable* r)
{
//	std::vector<Renderable*>::iterator itt = std::find(renderables_.begin(), renderables_.end(), r);
//	assert(itt == renderables_.end());

	renderables_.push_back(r);
}

void TestWindowManager::remove(Renderable* r)
{
	std::vector<Renderable*>::iterator itt = std::find(renderables_.begin(), renderables_.end(), r);
	if (itt != renderables_.end())
		renderables_.erase(itt);
}