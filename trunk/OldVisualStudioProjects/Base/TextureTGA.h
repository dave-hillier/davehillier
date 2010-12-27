#pragma once
#include "Common.h"
#include <fstream>
#include <vector>


class TextureTGA
{
public:
	TextureTGA();
	~TextureTGA();

	// Interface for Texture 
	const char *data() const { return &*pixelData.begin(); }
	unsigned width() const {return header.width;}
	unsigned height() const {return header.height;}
	unsigned char bytesPerPixel() const { return header.pixelSize/8; }

private:
	struct Header 
	{
		unsigned char 	idLength, colourmapType, imageType;
		unsigned short	colourmapIndex, colourmapLength;
		unsigned char	colourmapSize;
		unsigned short	xOrigin, yOrigin, width, height;
		unsigned char	pixelSize, attributes;

		enum ColourMapBits {
			NoPalette,
			HasPalette
		};
		enum ImageTypeBits {
			None,
			Indexed,
			RGB,
			Grey,
			RGBRLE = 10 // 8 or greater, but I'm only supporting 10
		};
		enum AttributeBits
		{
			FlipVertical = (1 << 6),
			FlipHorizontal = (1 << 5),
			AlphaBitMask = 0x0f,
		};
	};
	friend std::ifstream& operator >>(std::ifstream &i, Header &h); 	
	friend std::ifstream& operator >>(std::ifstream &i, TextureTGA &t);

	Header header;

	void ReadRGB(std::ifstream &i);
	void ReadRLE(std::ifstream &i);

	std::vector<char> pixelData;

};

std::ifstream& operator >>(std::ifstream &i, TextureTGA &t);
std::ifstream& operator >>(std::ifstream &i, TextureTGA::Header &h);