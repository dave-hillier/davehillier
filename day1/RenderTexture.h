#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H


#include <SDL.h>
#define NO_SDL_GLEXT
#include <SDL_OpenGL.h>

#include <OpenGL/gl.h>	/* Header File For The OpenGL Library */
#include <OpenGL/glu.h>	/* Header File For The GLU Library */
#include <OpenGL/glext.h>

#include <iostream>
#include <fstream>
#include <string>

class RenderTexture 
{
public:
	RenderTexture(unsigned width, unsigned height, bool depth) : width_(width), 
		height_(height), depthUsed_(depth)
	{
		glGenFramebuffersEXT(1, (GLuint*)&fbo_);
		glGenTextures(1, (GLuint*)&tex_);

		if (depth)
		{
			glGenRenderbuffersEXT(1, (GLuint*)&depth_);
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);  

		// init texture
		glBindTexture(GL_TEXTURE_2D, tex_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
			GL_TEXTURE_2D, tex_, 0);

		// initialize depth renderbuffer
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
			GL_RENDERBUFFER_EXT, depth_);

		//checkError("RenderToTexture ctor");
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);  
	}

	void bind() 
	{
		glBindTexture(GL_TEXTURE_2D, tex_);
	}

	void activate()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
		glPushAttrib(GL_VIEWPORT_BIT); 
		glViewport(0, 0, width_, height_);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void deactivate()
	{
		glPopAttrib(); 
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	unsigned texture() const { return tex_; }
private:
	unsigned int width_;
	unsigned int height_;
	unsigned int fbo_;
	unsigned int depth_;
	unsigned int tex_;
	bool depthUsed_;

};

#endif 