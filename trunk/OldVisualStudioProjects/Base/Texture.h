#pragma once
#include "Common.h"

// The concept of an openGL texture built from a tga etc
// TODO Perhaps rename to material
class Texture
{
public:
	Texture(){}
	Texture( const char* pixels, unsigned char bytesPerPixel, unsigned width, unsigned height );
	~Texture();

	// The textures are used.
	// Perhaps it should be part of a higher material class

	// TODO remember the attributes for this texture
	// 

	// TODO Bind
private:
	uint textureId;



};
