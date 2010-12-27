#include "Material.h"
#include <Windows.h> // for GL
#include <gl/GL.h>

Material::Material()
{
}

Material::~Material()
{
}

void
MColour::Apply()
{
	glColor4b(c_.r(), c_.g(), c_.b(), c_.a());
}

void
MTexture::Apply()
{
	MColour::Apply();

	// TODO Upload the texture if it hasnt been
	// TODO Bind the texture - if its loaded otherwise do a default texture
}

MTexture::~MTexture()
{
	// TODO Delete the texture if needed
}