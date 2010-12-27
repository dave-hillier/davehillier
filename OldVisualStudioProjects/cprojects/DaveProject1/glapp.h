class GLApp {
public:
	void initgl();
	void display();
	void resize(int width, int height);
	int width, height;
};

extern GLApp glapp;