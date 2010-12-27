
#ifndef __GLMAIN_H__
#define __GLMAIN_H__

#include "Matrix.h"

class GLMain {
public:
	// constructor?
	GLMain (int, int);

	void display();
	void resize(int width, int height);
	int width, height;
	// OpenGL view information
	int viewPort[4];

	Matrix modelView;
	Matrix projection;

	float last_fov, fov;
};

extern GLMain *glapp;

#endif

