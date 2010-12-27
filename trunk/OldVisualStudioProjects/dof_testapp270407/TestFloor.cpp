#include "TestFloor.h"
#include <SDL_opengl.h>

TestFloor::TestFloor(void)
{
}

TestFloor::~TestFloor(void)
{
}


void TestFloor::render()
{
	glColor3f(0.1f,0.1f,0.1f);
	unsigned n = 20, spacing = 20;
	glBegin(GL_QUADS);

	float xc = 0, yc = 0, zc = 0;
	float l = 20*20;
	glVertex3f(xc+l, yc-1, zc+l);
	glVertex3f(xc+l, yc-1, zc-l);
	glVertex3f(xc-l, yc-1, zc-l);
	glVertex3f(xc-l, yc-1, zc+l);
	glColor3f(0.2f,0.2f,0.5f);

#if 1
	for (int x = 0; x < n; ++x)
	{
		for (int z = 0; z < n; ++z)
		{
			const float xc = (float)(spacing*x)-(n*spacing/2);
			const float yc = 0;
			const float zc = (float)(spacing*z)-(n*spacing);

			float l =3;
			glVertex3f(xc+l, yc, zc+l);
			glVertex3f(xc+l, yc, zc-l);
			glVertex3f(xc-l, yc, zc-l);
			glVertex3f(xc-l, yc, zc+l);
		}
	}
#else
	float xc = 0, yc = 0, zc = 0;
	float l = 20*20;
	glVertex3f(xc+l, yc, zc+l);
	glVertex3f(xc+l, yc, zc-l);
	glVertex3f(xc-l, yc, zc-l);
	glVertex3f(xc-l, yc, zc+l);
#endif
	glEnd();
}