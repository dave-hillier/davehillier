#include "Main.h"
#include <SDL.h>
#include <Windows.h> // For GL
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Joystick.h"
#include "DebugLog.h"
#include "Vector3.h"


#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL.lib")

Joystick *js1;
const char dave[] = "Secret";

int main(int argc, char *argv[])
{
	int a = *(int*)dave;
	Main m;

	bool run= true;
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

		gluPerspective(45.0f,(GLfloat)(width)/(GLfloat)(height),0.1f,1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

}
Main::Main(void) : width_(640), height_(480), pScreen_(NULL),fullscreen_(false), keys_(NULL)
{
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0)
	{
	}

    js1 = new Joystick(0);

	if(!CreateWindowGL(fullscreen_))
	{
	}

	atexit(SDL_Quit);
}

Main::~Main(void)
{
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
	return true;
}


void DrawLineCube(float size)
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

Vector3 position(0,0,0);
Vector3 speed(0,0,0);
float heading=0;

void Main::Draw()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(position.x(), position.y(), position.z());
	glRotatef(heading, 0, 0, 1);
	glBegin(GL_LINE_STRIP);
	glColor4f(1,1,1,1);
	glVertex3f( 1, -1, -50);
	glVertex3f( -1, -1, -50);
	glVertex3f(-1, 1, -50);
	glVertex3f(1, 1, -50);
	glEnd();
	glPopMatrix();

	glColor4f(1,0,0,1);
	glPushMatrix();
	glTranslatef(0,0,-20);
	DrawLineCube(10);
	glTranslatef(0,0,-20);
	DrawLineCube(10);
	glTranslatef(0,0,-20);
	DrawLineCube(10);
	glPopMatrix();

	glColor4f(1,1,0,1);
	glPushMatrix();
	glTranslatef(0,0,20);
	DrawLineCube(10);
	glTranslatef(0,0,20);
	DrawLineCube(10);
	glTranslatef(0,0,20);
	DrawLineCube(10);
	glPopMatrix();

	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(-20,0,0);
	DrawLineCube(10);
	glTranslatef(-20,0,0);
	DrawLineCube(10);
	glTranslatef(-20,0,0);
	DrawLineCube(10);
	glPopMatrix();

	glColor4f(0,1,1,1);
	glPushMatrix();
	glTranslatef(20,0,0);
	DrawLineCube(10);
	glTranslatef(20,0,0);
	DrawLineCube(10);
	glTranslatef(20,0,0);
	DrawLineCube(10);
	glPopMatrix();


	glColor4f(0,0.5f,1,0.5f);
	glPushMatrix();
	glTranslatef(0,-20,0);
	DrawLineCube(10);
	glTranslatef(0,-20,0);
	DrawLineCube(10);
	glTranslatef(0,-20,0);
	DrawLineCube(10);
	glPopMatrix();


}

void Main::Update()
{
	Joystick::Update();

	position += speed;

	speed.x(js1->AxisLeftX()*0.1f);
	speed.y(js1->AxisLeftY()*0.1f);

	float s = 360.f * atan2f(speed.y(), speed.x()) / (PI*2.f);

	heading = heading * 0.9f + s * 0.1f;
	//Base::debugLog << "x=" << js1->AxisLeftX() << std::endl;
	//Base::debugLog << "y=" << js1->AxisLeftY() << std::endl;
	//Base::debugLog << "heading=" << heading << std::endl;
}

