#pragma once

#include "TGAImage.h"
#include <string>

class TGATexture
{
public:
	TGATexture(const char* filename, GLenum target) : 
		target_(target), filename_(filename) 
	{
		
	}

	bool load() 
	{
		
	}

	~TGATexture() {}
private:
	GLenum target_;
	std::string filename_;
	int textureId_;

};