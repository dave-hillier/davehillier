#include "Texture.h"
#include <Windows.h>
#include <GL/GLU.h>

Texture::Texture( const char* pixels, unsigned char bytesPerPixel, unsigned width, unsigned height)
{
	assert(bytesPerPixel == 3 || bytesPerPixel == 4);
	uint format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB; 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

	// allocate a texture name
	glGenTextures( 1, &textureId );

	// select our current texture
	glBindTexture( GL_TEXTURE_2D, textureId );

	// Generate the mip maps
	gluBuild2DMipmaps(GL_TEXTURE_2D, bytesPerPixel, width, height, format, GL_UNSIGNED_BYTE, pixels);
}

Texture::~Texture()
{
	// TODO delete texture textureId
	// TODO ref counting is probably required
	// TODO glDeleteTextures( 1, &textureId );
}
