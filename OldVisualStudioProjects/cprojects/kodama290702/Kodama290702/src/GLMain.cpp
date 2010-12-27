#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h"
#include "Console.h"
#include "ConsoleVars.h"
#include "ModelManager.h"
#include "Texture.h"
#include "Terrain.h"
#include "SDLTimer.h"
#include "Frustum.h"
#include "Camera.h"


#include "GLMain.h"
// globals
PFNGLMULTITEXCOORD1FARBPROC	glMultiTexCoord1fARB	= NULL;
PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB	= NULL;
PFNGLMULTITEXCOORD3FARBPROC	glMultiTexCoord3fARB	= NULL;
PFNGLMULTITEXCOORD4FARBPROC	glMultiTexCoord4fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB	= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB= NULL;


GLMain *glapp;

int model; // test
int texid, texidenv; // test
ConsoleVar *cv_fov;

GLMain::GLMain(int w, int h)
{
	fov = 90;
	resize(w, h);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
	glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
	glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

	// test stuff
	model = mman->Load("pill.ms3d"); 
	texid = texture.LoadTexture("pill.tga");
	texidenv = texture.LoadTexture("env4.tga");

	frustum = new Frustum();
	camera = new Camera();

	cv_fov = cvars->Add("fov", "90");

}

void GLMain::display ()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// rotate/translate for camera	
	camera->Update(); // should this be before or after the next line?
	glLoadMatrixf(camera->GetMatrix());

	//required for frustum, particles and other stuff
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetFloatv(GL_PROJECTION_MATRIX, projection.Get());// can be used for working out erverse projection i guess
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView.Get()); // used for billboarding
	// generate frustum
	frustum->Update();

	fov = cv_fov->Value();

	if (fov != last_fov)
		resize(width, height);

	// render bits here
	glEnable(GL_DEPTH_TEST);

	if (terrain)
	{
		terrain->Render(camera->GetPosition());
	}
	else 
	{
		camera->SetPosition(Vector(0,0,0));
		camera->SetRotateX(0);
		camera->SetRotateY(0);
		camera->SetRotateZ(0);

		glPushMatrix();
		glTranslatef(0,0,-64);

		glRotatef(180*timer.GetTime(), 0, 0, 1);
		glRotatef(180*timer.GetTime(), 0, 1, 0);
		glRotatef(180*timer.GetTime(), 1, 0, 0);

		glScalef(0.25,0.25,0.25);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(10);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glColor3f(0,0,0);
		glDisable(GL_TEXTURE_2D);
		mman->Render(model);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		
		glColor3f(1,1,1);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		texture.Bind(texid);
		glEnable(GL_TEXTURE_2D);

		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
		glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);

		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
		glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_KEEP);
		
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		texture.Bind(texidenv);

		mman->Render(model);

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}


	// switch to overlay
	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); //
	glLoadIdentity();
	
	glOrtho(0, width, 0, height, -10000, 10000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); //
	glLoadIdentity();

	// overlay here
	glColor3f(1,1,1);
	con->Render();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // 
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(); // 

	last_fov = fov;
}

void GLMain::resize(int w, int h)
{
	width = w;
	height = h;

	if (height==0)										
		height=1;										

	glViewport(0,0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fov/2, (GLfloat)width/(GLfloat)height, 0.1f, 300.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
