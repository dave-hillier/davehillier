#include <gl/glee.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <cassert>
#include <algorithm>
#include <list>
#include <iostream>

#include "App.h"
#include "WindowManager.h"
#include "TestWindowManager.h"
#include "Timer.h"
#include "StateMachine.h"
#include "DebugLog.h"
#include "InitialState.h"
#include "EventListener.h"

#ifdef WIN32
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL.lib")
#endif

App *App::globalApp = NULL;

class TestApp2 : public App
{
public:
	TestApp2(int argc, char *argv[]) : 
	  screen_(NULL), nextUpdate_(0), states_(NULL),frameTimer_(0), frameCount_(0),
		  updating_(false)
	{
		globalApp = this;
		int error = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
		SDL_JoystickEventState(SDL_ENABLE);
		if (error)
		{
			
		}
		atexit(SDL_Quit);

		bool result = createWindow();
		if (!result)
		{

		}

		setState(createStartState());
	}

	~TestApp2()
	{
		delete windowManager_;
	}

	WindowManager &getWindowManager() const
	{
		return *windowManager_;
	}

	WindowManager *createWindowManager()
	{
		return new TestWindowManager;
	}

	State *createStartState()
	{
		return new InitialState;
	}

	void setState(State* s)
	{
		if (!states_)
			states_ = new StateMachine;
		states_->change(s);
	}

	void add(Updatable* u)
	{
		//std::vector<Updatable*>::iterator itt = std::find(updatables_.begin(), updatables_.end(), u);
		//assert(itt == updatables_.end());
		//updatables_.push_back(u);	
		newThings_.push_back(u);
	}

	void remove(Updatable* u)
	{
		// TODO this wont really work
		assert(!updating_);
		std::vector<Updatable*>::iterator itt = std::find(updatables_.begin(), updatables_.end(), u);
		if (itt != updatables_.end())
			updatables_.erase(itt);
	}

	void add(EventListener* u)
	{
		eventListeners_.push_back(u);
	}

	void remove(EventListener* u)
	{

		std::list<EventListener*>::iterator itt = std::find(eventListeners_.begin(), eventListeners_.end(), u);
		if (itt != eventListeners_.end())
			eventListeners_.erase(itt);
	}

	void update()
	{
		updating_ = true;
		updatables_.insert(updatables_.begin(), newThings_.begin(), newThings_.end());
		newThings_.clear();
		for (std::vector<Updatable*>::iterator itt = updatables_.begin(); itt != updatables_.end(); ++itt)
		{
			(*itt)->update();
		}
		updating_ = false;
	}

	int loop()
	{
		bool quit = false;
		int result = 0;
		while (!quit)
		{
			bool result = true;
			SDL_Event event;	
			if(SDL_PollEvent(&event))	
			{
				switch(event.type)
				{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_VIDEORESIZE:
					windowManager_->resize(event.resize.w, event.resize.h);
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN:	
					for (std::list<EventListener*>::iterator itt = eventListeners_.begin(); 
						itt != eventListeners_.end(); ++itt)
					{
						(*itt)->keyPress(event.key.keysym.sym, 
							event.key.keysym.mod, 
							event.key.type == SDL_KEYDOWN);
					}
					// TODO remove
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
					}
					break;
					// TODO fix this
				case SDL_JOYBUTTONUP:
				case SDL_JOYBUTTONDOWN:
					for (std::list<EventListener*>::iterator itt = eventListeners_.begin(); 
						itt != eventListeners_.end(); ++itt)
					{
						(*itt)->joystickButton(event.jbutton.which, 
							event.jbutton.button, event.jbutton.state == SDL_PRESSED);
					}
					break;
				case SDL_JOYAXISMOTION:
					for (std::list<EventListener*>::iterator itt = eventListeners_.begin(); 
						itt != eventListeners_.end(); ++itt)
					{
						float value = event.jaxis.value/-32768.f;
						value = value > 0.05f ? value : 0.0f;
						(*itt)->joystickAxis(event.jaxis.which, event.jaxis.axis, value);
					}
					break;
				default:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					for (std::list<EventListener*>::iterator itt = eventListeners_.begin(); 
						itt != eventListeners_.end(); ++itt)
					{
						(*itt)->mouseButton(event.button.button, 
							event.button.type == SDL_MOUSEBUTTONDOWN);
					}
					break;
				case SDL_MOUSEMOTION:
					for (std::list<EventListener*>::iterator itt = eventListeners_.begin(); 
						itt != eventListeners_.end(); ++itt)
					{
						(*itt)->mouseMove(event.motion.x, event.motion.y, 
							event.motion.xrel, event.motion.yrel);
					}
					break;
				};
			}
			else
			{
				Timer t;
				unsigned now = t.getTime();
				
				if (now >= nextUpdate_)
				{
					update();
					nextUpdate_ = now + 10;
				}

				if (now >= frameTimer_)
				{
					std::cout << "FPS: " << frameCount_ << std::endl;
					frameTimer_ = now + 1000;
					frameCount_ = 0;
				}
				++frameCount_;

				windowManager_->update();
				SDL_GL_SwapBuffers();
			}
		}
		return result;
	}

	bool createWindow(bool fullscreen=false)	
	{
		unsigned vflags = SDL_HWSURFACE|SDL_OPENGLBLIT;
		if (fullscreen)
			vflags|=SDL_FULLSCREEN;

		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );	
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );	

		// TODO read the window size from config
		screen_ = SDL_SetVideoMode(640, 480, 32, vflags);
		if(!screen_)	
		{
			return false;
		}
		windowManager_ = createWindowManager();
		if (!windowManager_)
		{
			return false;
		}

		windowManager_->resize(640, 480);	
		return true;
	}
private:
	SDL_Surface *screen_;
	WindowManager *windowManager_;
	unsigned nextUpdate_;
	unsigned frameTimer_;
	unsigned frameCount_;
	StateMachine *states_;
	bool updating_;

	std::vector<Updatable*> updatables_;
	std::vector<Updatable*> newThings_;
	std::list<EventListener*> eventListeners_;
};

int SDL_main(int argc, char *argv[])
{
	TestApp2 *newApp = new TestApp2(argc, argv);

	if (GLEE_EXT_framebuffer_object) // TODO group any prerequisites here
	{
		newApp->loop();
	}
	else
	{
		std::cout << "Requires FBO support, download newer drivers" << std::endl;
	}
	delete newApp;

	
	return 0;
}