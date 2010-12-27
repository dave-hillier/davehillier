#pragma once

#include "Colour.h"


class Material
{
public:
	Material();
	virtual ~Material();

	virtual void Apply() = 0;
};


class MColour : public Material
{
public:
	MColour(const Colour &c) : c_(c)
	{
	}
	virtual void Apply();

private:
	Colour c_;
};

class MTexture : public MColour
{
public:
	MTexture(const Colour &c, int textureId) : MColour(c), textureId_(textureId)
	{
	}
	~MTexture();
	virtual void Apply();

private:
	// Texture id?
	int textureId_;
};


