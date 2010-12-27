#pragma once
#include "windowmanager.h"
#include <vector>

class Camera;

class TestWindowManager :
	public WindowManager
{
public:
	TestWindowManager(void);
	virtual ~TestWindowManager(void);

	virtual void resize(unsigned w, unsigned h);
	virtual void update();


	void setPerspective(unsigned int width, unsigned int height, float nearZ, float farZ);
	void startOrthographic(unsigned int w, unsigned int h, float nearZ, float farZ);
	void endOrthographic();

	void renderOrthographic();
	void renderPerspective();

	void add(Renderable* r);
	void remove(Renderable* r);

private:
	Camera* camera_;
	unsigned width_;
	unsigned height_;

	float near_;
	float far_;

	std::vector<Renderable*> renderables_;

	// TODO encapsulate into a render target class
	unsigned int fbo_;
	unsigned int depth_;
	unsigned int tex_;

	unsigned int fbo1_;
	unsigned int tex1_;

};
