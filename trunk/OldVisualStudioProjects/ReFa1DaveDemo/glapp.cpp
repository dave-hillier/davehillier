#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"

#include "Timer.h"
#include "font.h"
#include "glapp.h"
#include "new_src/TextureManager.h"
#include "ParticleManager.h"
#include "camera.h"
#include "frustum.h"

#include "ms3d_model.h"
#include "md2_model.h"
#include "terrain.h"
#include "new_src/Actor.h"
#include "Crosshair.h"
#include "Overlay.h"
#include "ase_model.h"
#include "Console.h"
void LoadStaticMedia();
#include "new_src/Environment.h"

#define GLH_EXT_SINGLE_FILE 1

#include <glh/glh_extensions.h>
#include <glh/glh_genext.h>

#include "new_src/RenderTexture.h"


void checkError(const char* where)
{
	GLenum error;

	while ((error = glGetError()) != GL_NO_ERROR) {
		con->Printf("GLError %s: error - %s\n", where, (char *) gluErrorString(error));
	}

	assert(glGetError() == GL_NO_ERROR);
}

GLApp glapp;

void setProjection(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,Environment::instance().scale()*50*0.1,768.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0,0,width,height);
}
void GLApp::resize(int w, int h)
{
	width = w;
	height = h;
	if (height==0)										
	{
		height=1;										
	}
	setProjection(width, height);
}

void GLApp::initgl ()
{
	// maybe splash here?
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);

	if(! glh_init_extensions(
		"GL_ARB_multitexture "
		"GL_NV_vertex_program "
		"GL_NV_fragment_program "
		"WGL_ARB_pbuffer "
		"WGL_ARB_pixel_format "
		"WGL_ARB_render_texture "
		))	{
			// TODO log properly
			fprintf(stderr, "Error - required extensions were not supported: %s", glh_get_unsupported_extensions());
		}

	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_DEPTH_TEST);


	//const char* fp_mode = "float=16 rgba depth textureRECT";
	const char* fp_mode = "rgba depth textureRECT";
	const GLenum fp_target = GL_TEXTURE_RECTANGLE_NV;
	//const char* fp_mode = "rgba depth texture";
	//const GLenum fp_target = GL_TEXTURE_2D;

	int sizeOfTarget=256;
	//otherScene = new RenderTexture(fp_mode, sizeOfTarget, sizeOfTarget, fp_target);
	//otherScene = new RenderTexture(fp_mode, 320, 240, fp_target);
	//otherScene->Activate();
	//glEnable(GL_DEPTH_TEST);
	//setProjection(320,240);
	//otherScene->Deactivate();

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.3f, 0.4f, 0.9f, 0.0f);

	arial = new Font ("Arial Black");

	unsigned texture_id;
	TextureManager::Instance().Load("./pics/envtest1.jpg", texture_id);
	TextureManager::Instance().Load("./pics/sky.jpg", texture_id);
	TextureManager::Instance().Load("./pics/crosshair.tga", texture_id);
	TextureManager::Instance().Load("./pics/spectrum.jpg", texture_id);

	LoadStaticMedia();

	terrain.Init("map1");

	//mainScene->Activate();
	checkError("Init");

}


// display
static float a = 0;
void DrawSky();
void DrawSea();
void GLApp::display()
{
	//otherScene->Activate();
	//displayWorld();
	//checkError("DrawTestOther");
	//otherScene->Deactivate();

	float backgroundColour[] = {0.5f,0.65f,0.9f, 1};
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
	glFogfv(GL_FOG_COLOR, backgroundColour);
	glFogf(GL_FOG_DENSITY, 1.f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 64);
	glFogf(GL_FOG_END, 130.0f);
	glFogi(GL_FOG_MODE, GL_LINEAR);

	glEnable(GL_FOG);

	displayWorld();
	glDisable(GL_FOG);

	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glOrtho(0, width, 0, height, -10000, 10000);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		overlay.Display();
		overlay.Update();
		con->Render();
		xhair.Display();

		glPushMatrix();
		glTranslatef(10,10,0);
		glScalef(20, 20, 20);
		float fps = 1.0f/timer.Delta();
		arial->printf("FPS: %.2f", fps);
		glPopMatrix();   

	/*	//glEnable(GL_TEXTURE_2D);
		//glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		glColor4f(1,1,1, 0.5f);
		glEnable(GL_BLEND);
		otherScene->Bind();
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBegin(GL_QUADS);

		glTexCoord2f(0, 0);
		glVertex2i(0, 0);
		glTexCoord2f(0, height);
		glVertex2i(0, height);
		glTexCoord2f(width, height);
		glVertex2i(width, height);
		glTexCoord2f(width, 0);
		glVertex2i(width, 0);

		glEnd();
		otherScene->Release();*/
		glDisable(GL_TEXTURE_RECTANGLE_NV);
		glDisable(GL_BLEND);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();   
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();


	}
	checkError("display");

}

void GLApp::displayWorld()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadMatrixf(camera.GetMatrix());
	gen_clipmat();
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

	glEnable(GL_TEXTURE_2D);

	DrawSky();
	DrawSea();
	terrain.Render(camera.position);//();
	Environment::instance().Render();
	pman.Render();

	checkError("displayWorld");


}



/*
void R_RotateForEntity (entity_t *e)
{
    glTranslatef (e->origin[0],  e->origin[1],  e->origin[2]);

    glRotatef (e->angles[1],  0, 0, 1);
    glRotatef (-e->angles[0],  0, 1, 0);
    glRotatef (e->angles[2],  1, 0, 0);
}*/
void DrawSky ()
{
	return; // TODO disabled
	float i, j, d;

//	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(camera.position.x, camera.position.y, camera.position.z);
	glColor3f(1,1,1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	TextureManager::Instance().Apply("./pics/sky.jpg");

	for (j=-5; j<=5; j+=1.0f) {
		glBegin(GL_TRIANGLE_STRIP);
		for (i=-5; i<=5; i+=1.0f) {
			d = (float )sqrt(i*i+j*j);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, j/2);
			glVertex3f(i, 1-d/3, j);
			d = j + 1.0f;
			d = (float )sqrt(i*i+d*d);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, (j+1.0f)/2);
			glVertex3f(i, 1-d/3, (j+1.0f));
		}
		glEnd();
	}
	glPushMatrix();
	glScalef(1,-1,1);
	for (j=-5; j<=5; j+=1.0f) {
		glBegin(GL_TRIANGLE_STRIP);
		for (i=-5; i<=5; i+=1.0f) {
			d = (float )sqrt(i*i+j*j);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, j/2);
			glVertex3f(i, 1-d/3, j);
			d = j + 1.0f;
			d = (float )sqrt(i*i+d*d);
			glColor4f(1, 1, 1, 4-d);
			glTexCoord2f(i/2, (j+1.0f)/2);
			glVertex3f(i, 1-d/3, (j+1.0f));
		}
		glEnd();
	}
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
}


void DrawSea () {
/*	static float cosa=0, sea_move;
	cosa += 0.01;

	sea_move += 0.001*cos(cosa);

	glDisable(GL_CULL_FACE);
//	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glBindTexture(GL_TEXTURE_2D, terrain_texture.sea1);
	TextureManager::Instance().Apply("sea1.tga");

	glActiveTextureARB(GL_TEXTURE1_ARB);
//	glBindTexture(GL_TEXTURE_2D, terrain_texture.sea2);
	TextureManager::Instance().Apply("sea2.tga");

	glColor4f(0.6, 0.7, 1.0, 0.3);
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0+sea_move, 200+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0+sea_move, 200-sea_move);
	glVertex3f(-1024.0f, 0.0f,  1024.0f);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 200+sea_move, 200+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 200+sea_move, 200-sea_move);
	glVertex3f( 1024.0f, 0.0f,  1024.0f);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 200+sea_move, 0+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 200+sea_move, 0-sea_move);
	glVertex3f( 1024.0f, 0.0f, -1024.0f);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0+sea_move, 0+sea_move);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0+sea_move, 0-sea_move);
	glVertex3f(-1024.0f, 0.0f, -1024.0f);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0, 1.0, 1.0, 1.0);

//	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);


	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
*/
	// draw sun?
}

