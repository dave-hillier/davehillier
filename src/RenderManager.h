#pragma once

class Font;
class Camera;

// TODO extract interface at some point
class RenderManager
{
public:
	RenderManager();
	virtual ~RenderManager();

	virtual bool initialise();
	virtual void resize(unsigned int w, unsigned int h);
	virtual void render(double timeNow);

	virtual void renderOrthographic();
	virtual void renderPerspective();

	static void checkError(const char* where);

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
};
