// myShip.cpp: implementation of the myShip class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "ms3d_model.h"
#include "texturemanager.h"
#include "ParticleManager.h"
#include "timer.h"
#include "input.h"
#include "myShip.h"
#include "camera.h"
#include "Rocket.h"

static const Vector3 worldAxis[3] = 
{
   Vector3(1.0f, 0.0f, 0.0f),
   Vector3(0.0f, 1.0f, 0.0f),
   Vector3(0.0f, 0.0f, 1.0f),
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

myShip::myShip()
{
	model1.Load("orca0.ms3d");
	model2.Load("orca1.ms3d");
	mass = 1000;
	roll = 0;
	turning = 0;
	heading = 0;
	pitch = 0;
	fired = false;
}

myShip::~myShip()
{
	
}

void myShip::Render()
{
	texture.Bind("envtest1.tga");
	glColor3f(1,1,1);
	glPushMatrix();

    glTranslatef (org[0],  org[1],  org[2]);

	glRotatef (-heading+180,  0, 1, 0);
	glRotatef (roll,  0, 0, 1);
	glRotatef (pitch,  1, 0, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	model1.Render();


	// engines
	glPushMatrix();
	glRotatef(-engine_pitch-90, 1,0,0);
	model2.Render();
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();


}
void myShip::Update()
{
	// add particles etc here
	// physics

	Vector3 f, g, thrust, air_res;

	g = Vector3(0, -9.81f * 40, 0);

	engine_pitch += input.mouse_y * -2.0; // * for sensitivity
	turning += input.mouse_x * 0.5 ;  // * for sensitivity
	
	if (turning > 90)
		turning = 90;
	if (turning < -90)
		turning = -90;

	roll = turning;
	if (roll > 45.f)
		roll = 45.f;
	if (roll < -45.f)
		roll = -45.f;

	heading += turning * timer.Delta();

	if (heading > 360.f)
		heading = -360.f;
	if (heading < -360.f)
		heading = 360.f;

	if (engine_pitch > 130.f)
		engine_pitch = 130.f;
	if (engine_pitch < -70.f)
		engine_pitch = -70.f;

	if (throttle_inc > 0)
		throttle += throttle_inc;	
	else
		throttle -= 0.5 * timer.Delta();

	if (input.State(K_MOUSE2))
		throttle_inc += 0.5 * timer.Delta();
	else
		throttle_inc -= 0.5 * timer.Delta() * 100;

	if (throttle < 0)
		throttle = 0;// thats pretty much as low as u want to go
	else if (throttle > 1)
		throttle = 1;

	if (throttle_inc < 0.f)
		throttle_inc = 0.f;

	GetDirection(f);

	float ang = MathLib::DegToRad(engine_pitch);
	thrust = sin(ang) * f + Vector3(0, cos(ang), 0);

	glDisable(GL_TEXTURE_2D);
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex3fv(thrust*150+org+Vector3(1,0,0));
	glVertex3fv(org);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,0);
	glBegin(GL_LINES);
	glVertex3fv(f*150+org+Vector3(1,0,0));
	glVertex3fv(org);
	glEnd();
	/**/
/*	glEnable(GL_TEXTURE_2D);

	glPushMatrix();

    glTranslatef (org[0],  org[1],  org[2]);

	glRotatef (-heading+180,  0, 1, 0);
	glRotatef (roll,  0, 0, 1);
	glRotatef (pitch,  1, 0, 0);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	model1.Render();


	// engines
	glPushMatrix();
	glRotatef(-engine_pitch-90, 1,0,0);
	model2.Render();
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();*/
/**/
	engine_power = (2.0f + throttle * 40.f) * 200000.f;
	thrust *= engine_power;

	air_res = vel * 0.75 * vel.Length();
	
	if (air_res[1] < 0)
		air_res[1] = 0;

	acc = (mass * g) + thrust - air_res;
	acc *= 1.0f/mass;


	vel += acc * timer.Delta();
	org += vel * timer.Delta();

	if (org[1] < 40)
	{
		org[1] = 40;
		vel[1] = 0;
	}

	if (input.State(K_MOUSE1) && !fired)
	{
		Fire();
		fired = true;
	}
	else if (fired && !input.State(K_MOUSE1))
		fired = false;

	if (engine_pitch > 0)	
		pitch = 15 * throttle;

	camera.SetRotateX(0);
	camera.SetRotateY(-heading);

	camera.SetPosition(org + f * -500 + Vector3(0,100,0));

}

void myShip::GetDirection (Vector3 &dir)
{ 
	dir[0] = -cos(MathLib::DegToRad(heading+90));
	dir[1] = 0;
	dir[2] = -sin(MathLib::DegToRad(heading+90));

}

void myShip::Fire()
{

	if (fire_delay > timer.GetTime())
		return;
	else
		fire_delay = timer.GetTime() + 0.5f;
	Vector3 offset = Vector3(0,0,0), f, targ; // work out from model

/*	f[0] = cos(MathLib::DegToRad(pitch))
		*sin(MathLib::DegToRad(heading));
	f[1] = sin(MathLib::DegToRad(pitch));
	f[2] = cos(MathLib::DegToRad(pitch))
		*cos(MathLib::DegToRad(heading));*/

	f[0] = -cos(MathLib::DegToRad(heading+90));
	f[1] = 0;
	f[2] = -sin(MathLib::DegToRad(heading+90));

	// fire one per click
	{
		Unit *r = new Rocket (org, f, targ, vel);
	}
}