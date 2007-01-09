
#include "TestApp.h" 
	
#include "AppFactory.h"
#include "RenderManager.h"
#include "Timer.h"
#include "Service.h"
#include "Mouse.h"


#include <glh/glh_glut.h>

#include <cassert>

// Link all the libs required here
#ifdef WIN32
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif		

namespace {

	class TestAppGlhInteractor : public glh::glut_interactor
	{
	public:
		TestAppGlhInteractor(TestApp *app) : app_(app) 
		{ 
		}

		virtual void display() 
		{
			app_->render();
			glutSwapBuffers();
		}

		virtual void idle() 
		{
			app_->update();
			glutPostRedisplay();
		}

		virtual void keyboard(unsigned char key, int x, int y) 
		{

		}

		virtual void menu_status(int status, int x, int y) 
		{

		}

		virtual void motion(int x, int y) 
		{
		}

		virtual void mouse(int button, int state, int x, int y) 
		{
		}

		virtual void passive_motion(int x, int y) 
		{
		}

		virtual void reshape(int w, int h) 
		{
			app_->screenResize(w,h);
		}

		virtual void special(int  key, int x, int y) {}

		virtual void timer(int value) {}

		virtual void visibility(int v) {}

	private:
		TestApp *app_;
	};
}



TestApp::TestApp() : 
	timer_(new Timer),
	width_(720),
	height_(480),
	fullscreen_(false),
	renderManager_(NULL)
{

}

void TestApp::update() 
{
	const double timeNow = timer_->currentTime();
	// TODO replace with a for each?
	for (std::vector<Service*>::iterator itt = services_.begin(); itt != services_.end(); ++itt)
	{
		(*itt)->update(timeNow);
	}

	Mouse::instance()->update(timeNow);
}

void TestApp::render() 
{
	assert(timer_);
	const double timeNow = timer_->currentTime();
	// forward to whatever we're letting manage GL
	assert(renderManager_);
	renderManager_->render(timeNow);
}

TestApp::~TestApp(void)
{
	delete timer_;
}

bool TestApp::mainLoop()
{
	bool exit = false;
	glutMainLoop();
	return exit;
}

void TestApp::screenResize(unsigned int w, unsigned int h)
{
	// forward to whatever we're letting manage GL
	if (renderManager_)
	{
		renderManager_->resize(w,h);
	}
}

bool TestApp::createWindow()	
{
	glutInitWindowSize (width_, height_);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutCreateWindow ("TestApp");

	screenResize(width_, height_);	
	return true;
}

bool TestApp::initialise(int argc, char **argv)
{
	bool result = true;
	timer_->start();
	if(!createWindow())
	{
		result = false;
		assert(!"failed to init Window");
	}

	//const char *ext = (const char*)glGetString(GL_EXTENSIONS);
	//std::cout << strstr(ext, "GL_EXT_framebuffer_object") << std::endl;

	renderManager_ = AppFactory::instance().createRenderManager();
	assert(renderManager_);
	screenResize(width_, height_);	

	return result;
}


int main(int argc, char **argv)
{
	glutInit (&argc, argv);
	TestApp *app = AppFactory::instance().createApplication();

	bool result = app->initialise(argc, argv);
	TestAppGlhInteractor interactor(app);
	glh::glut_helpers_initialize();
	glh::glut_add_interactor(&interactor);	

	app->mainLoop();

	delete app;
	return 0;
}