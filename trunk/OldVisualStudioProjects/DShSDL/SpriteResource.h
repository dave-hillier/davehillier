#pragma once

#include "Resource.h"

class TextureResource;

class SpriteResource : public Resource
{
public:
	SpriteResource();
	~SpriteResource();

private:
	TextureResource *texture_;
};
