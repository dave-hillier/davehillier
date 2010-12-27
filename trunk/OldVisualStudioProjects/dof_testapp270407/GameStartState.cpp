#include "GameStartState.h"
#include "TestFloor.h"
#include "App.h"
#include "WindowManager.h"
#include "Timer.h"
#include "DebugLog.h"
#include "Joystick.h"

//
#include "DynamicParticle.h"
#include <SDL_opengl.h>
#include <iostream>

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
class ADot : public Renderable
{
public: 
	ADot(const vector3& pos, const vector3& vel) : p_(pos, vel, 1)
	{
		App::instance().add(&p_);
	}
	virtual ~ADot()
	{
		App::instance().remove(&p_);
	}

	void render()
	{
		//glPushMatrix();
		const vector3 &pos = p_.position(); 

		glColor3f(0.6f, 0.1f, 0.1f);
		float l = 1;
		glBegin(GL_QUADS);
		glVertex3f(pos.x+l, pos.y+l, pos.z);
		glVertex3f(pos.x+l, pos.y-l, pos.z);
		glVertex3f(pos.x-l, pos.y-l, pos.z);
		glVertex3f(pos.x-l, pos.y+l, pos.z);

		glEnd();
		//glTranslatef(pos.x, pos.y, pos.z);
		//glColor3f(1,1,1);
		//drawLineCube(1);
		//glPopMatrix();
	}

	DynamicParticle p_;
};
//

GameStartState::GameStartState(Joystick *joy) : started_(0), test_(NULL), joy_(joy)
{
}

GameStartState::~GameStartState(void)
{
}

void GameStartState::start()
{
	Timer t;
	started_ = t.getTime();


	srand(0xadadadad);
	for (int i = 0; i < 100; ++i)
	{
		vector3 vel(1.0f*(rand() - 32767/2),
			1.0f*(rand() - 32767/2),
			1.0f*(rand() - 32767/2));
		vel.norm();
		vel *= 20;

		ADot* dot = new ADot(vector3(0,0,0), vel);
		owned_.push_back(dot);
		parts_.push_back(&dot->p_);
	}
	owned_.push_back(new TestFloor);

	for (std::vector<Renderable*>::iterator itt = owned_.begin(); itt != owned_.end(); ++itt)
	{
		App::instance().getWindowManager().add(*itt);
	}
}

void GameStartState::end()
{

	for (std::vector<Renderable*>::iterator itt = owned_.begin(); itt != owned_.end(); ++itt)
	{
		Renderable* r = *itt;
		App::instance().getWindowManager().remove(r);
        delete r;
	}
	owned_.clear();

}

#include "InitialState.h"
void GameStartState::update()
{
	float f = joy_->axis(1) * 20;	
	float g = -joy_->axis(0) * 20;	
	float h = -joy_->axis(3) * 20;	
	for (std::vector<DynamicParticle*>::iterator itt = parts_.begin(); itt != parts_.end(); ++itt)
	{

		DynamicParticle* p = *itt;
		vector3 dist = vector3(0,40,-100) - p->position();
		dist.norm();
		dist *= 20;
		p->applyForce(vector3(g,f,h)+dist);
	}
}