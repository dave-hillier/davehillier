#pragma once

#include <vector>

#include "Renderable.h"

class Font;
class Camera;

class _vector3;
typedef _vector3 vector3;
class quaternion;


// TODO extract interface at some point
class RenderManager
{
public:
	RenderManager();
	virtual ~RenderManager();

	virtual bool initialise();
	virtual void resize(unsigned int w, unsigned int h);
	virtual void render(double timeNow);

	virtual void renderOrthographic(double timeNow);
	virtual void renderPerspective(double timeNow);

	static void checkError(const char* where);

	bool add(Renderable *thing);
	bool remove(Renderable *thing);

	Camera* camera() const { return testCamera_; }

protected:
	virtual void setPerspective(unsigned int w, unsigned int h, float near, float far);

	virtual void startOrthographic(unsigned int w, unsigned int h, float near, float far);
	virtual void endOrthographic();

private:
	unsigned int width_;
	unsigned int height_;
	float near_;
	float far_;
	
	Font *fpsDisplay_;
	Camera *testCamera_;

	float fps_;
	double lastUpdate_;

	std::vector<Renderable *> things_;
};
