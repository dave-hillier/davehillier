#pragma once

class RenderTexture;

class GLApp {
public:
	void initgl();
	void display();
	void displayWorld();
	void resize(int width, int height);
	int width, height;
	// OpenGL view information
	int viewPort[4];
	double modelViewMatrix[16];
	double projectionMatrix[16];

	RenderTexture* otherScene;
};

extern GLApp glapp;