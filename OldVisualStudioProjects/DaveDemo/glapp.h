class GLApp {
public:
	void initgl();
	void display();
	void resize(int width, int height);
	int width, height;
	// OpenGL view information
	int viewPort[4];
	double modelViewMatrix[16];
	double projectionMatrix[16];
};

extern GLApp glapp;