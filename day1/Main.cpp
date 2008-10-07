#include "Main.h"

#include <iostream>
#include <SDL.h>

#define NO_SDL_GLEXT
#include <SDL_OpenGL.h>

#include "Camera.h"
#include "Frustum.h"
#include "Joystick.h"
#include "RenderTexture.h"
#include "TerrainMap.h"
#include "TerrainGenerator.h"

#include <btBulletDynamicsCommon.h>

btDiscreteDynamicsWorld *world = NULL;
btRigidBody* body1 = NULL;
btRigidBody* body2 = NULL;
btRigidBody* body3 = NULL;

Camera cam;
Joystick *joy=NULL;
RenderTexture *fbo = NULL;
TerrainMap *terrain = NULL;

int main(int argc, char *argv[])
{
	Main m;

	bool run = true;
	while (run)
	{
		run = m.Loop();
	}

	return 0;
}

namespace {

	void ReshapeGL(int width, int height)
	{
		glViewport(0,0,(GLsizei)(width),(GLsizei)(height));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLfloat aspect = (GLfloat)width/(GLfloat)height;
		GLfloat nearZ = 1;
		GLfloat farZ = 1024;
		matrix44 matrix;
		matrix.perspFovRh(45.0f, aspect, nearZ, farZ);
		glLoadMatrixf((GLfloat*)(matrix.m));
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void addGround()
	{
		btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),-5);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0,0,0));
	
		btScalar mass(0);
		btVector3 localInertia(0,0,0);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		world->addRigidBody(body);
	}
	
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape)
	{
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
		{
			shape->calculateLocalInertia(mass,localInertia);
		}

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		world->addRigidBody(body);

		return body;
	}

}
Main::Main(void) : width_(640), height_(480), pScreen_(NULL),fullscreen_(false), keys_(NULL)
{
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0)
	{
	}

	if(!CreateWindowGL(fullscreen_))
	{
	}
	atexit(SDL_Quit);
	
	if (Joystick::numberOfJoysticks()>0)
	{
		joy = new Joystick(0);
	}
	fbo = new RenderTexture(256,256,true);
	
	// Physics
	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);

	const int maxProxies = 32766;
	btAxisSweep3 *broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	world->setGravity(btVector3(0,-10,0));

	addGround();
	
	btScalar mass(1.f);
	btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0,10,-10));
	
	body1 = localCreateRigidBody(mass, startTransform, colShape);
	startTransform.setOrigin(btVector3(0,20,-10));
	body2 = localCreateRigidBody(mass, startTransform, colShape);
	startTransform.setOrigin(btVector3(0,30,-10));
	body3 = localCreateRigidBody(mass, startTransform, colShape);
	
	terrain = new TerrainMap(65*6.f, new TerrainGenerator);
}

Main::~Main(void)
{
	delete joy;
}

bool Main::Loop()
{
	bool result = true;
	SDL_Event event;	
	if(SDL_PollEvent(&event))	
	{
		switch(event.type)
		{
		case SDL_QUIT:
			result = false;
			break;
		case SDL_VIDEORESIZE:
			ReshapeGL(event.resize.w, event.resize.h);
			break;
		case SDL_KEYDOWN:
			keys_ = SDL_GetKeyState(NULL);
			break;
			// TODO Keyboard, mouse and active focus
		default:
			break;
		};
	}
	else
	{
		Update();
		Draw(/*pScreen_*/);
		SDL_GL_SwapBuffers();	
	}

	return result;
}



bool Main::CreateWindowGL(bool fullscreen)	
{
	int vflags = SDL_HWSURFACE|SDL_OPENGLBLIT;
	if (fullscreen)
		vflags|=SDL_FULLSCREEN;

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );	
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );	

	if(!(pScreen_ = SDL_SetVideoMode(width_, height_, 32, vflags)))	
	{
		return false;
	}

	SDL_FillRect(pScreen_, NULL, SDL_MapRGBA(pScreen_->format,0,0,0,0));

	ReshapeGL(width_, height_);	
	
	glClearColor(0.3f,0.6f,0.9f,1);
	return true;
}



void Main::Draw()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glEnable(GL_DEPTH_TEST);
	
	// Set perspective
	matrix44 projection;
	glGetFloatv(GL_PROJECTION_MATRIX,(GLfloat*)projection.m);
	matrix44 modelView = cam.modelView();
	Frustum f(projection, modelView);
	glLoadMatrixf((GLfloat*)modelView.m);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	/*glPushMatrix();
	glColor3f(0,1,1);
	btScalar m[16];
	body1->getCenterOfMassTransform().getOpenGLMatrix(m);
	glMultMatrixf((GLfloat*)m);
	DrawLineCube(2);
	glPopMatrix();
	
	glPushMatrix();
	glColor3f(1,1,0);
	body2->getCenterOfMassTransform().getOpenGLMatrix(m);
	glMultMatrixf((GLfloat*)m);
	DrawLineCube(2);
	glPopMatrix();	
	
	glPushMatrix();
	glColor3f(0,1,0);
	body3->getCenterOfMassTransform().getOpenGLMatrix(m);
	glMultMatrixf((GLfloat*)m);
	DrawLineCube(2);
	glPopMatrix();	*/


	glPushMatrix();
	terrain->render(f);
	glPopMatrix();
	
}

void Main::Update()
{
	static float lastUpdate = 0;
	float now = SDL_GetTicks() / 1000.f;
	float deltaSeconds = now - lastUpdate;
	world->stepSimulation(deltaSeconds);
	
	
	// fly cam
	if (joy)
	{
		joy->update();
		vector3 f = cam.forward();
		vector3 r = f * vector3(0,1,0);
		vector3 translate = (f * joy->axis(1)) + (r * -joy->axis(0));
		cam.translate(translate * 10 * deltaSeconds);
				
		vector3 joy1(-joy->axis(2), 0, -joy->axis(3));
		cam.adjustPitch(joy1.z * 2 * deltaSeconds);
		cam.adjustYaw(joy1.x * 2 * deltaSeconds);
	}
	
	if (keys_)
	{
		int axis0 = (keys_[SDLK_a] ? 1 : 0) - (keys_[SDLK_d] ? 1 : 0);
		int axis1 = (keys_[SDLK_w] ? 1 : 0) - (keys_[SDLK_s] ? 1 : 0);
		int axis2 = (keys_[SDLK_LEFT] ? 1 : 0) - (keys_[SDLK_RIGHT] ? 1 : 0);
		int axis3 = (keys_[SDLK_UP] ? 1 : 0) - (keys_[SDLK_DOWN] ? 1 : 0);
		vector3 f = cam.forward();
		vector3 r = f * vector3(0,1,0);
		vector3 translate = (f * axis1) + (r * -axis0);
		cam.translate(translate * 10 * deltaSeconds);
				
		vector3 joy1(-axis2, 0, -axis3);
		cam.adjustPitch(joy1.z * 2 * deltaSeconds);
		cam.adjustYaw(joy1.x * 2 * deltaSeconds);
	}	
	
	/*TerrainGenerator tmp;
	float h = tmp(cam.position().x, cam.position().z);
	vector3 newPos(cam.position());
	newPos.y = (0.1f*h) - 17.5;
	cam.position(newPos);
	*/
	cam.update();
	if (terrain)
	{
		terrain->update();
	}
	lastUpdate = now;

	
}



