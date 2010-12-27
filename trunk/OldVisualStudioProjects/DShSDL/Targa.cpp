#include "Targa.h"
#include <fstream>
#include <assert.h>

namespace
{
	// TODO Move to template lib
	template<typename T>
	std::ifstream& operator >>(std::ifstream &i, T &t)
	{
		i.read((char*)&t, sizeof(T));
		return i;
	}
	template<typename T>
	void Read(std::ifstream &i, T &t)
	{
		i.read((char*)&t, sizeof(T));
	}
}

Targa::Targa()
{
}

Targa::~Targa()
{
}

void Targa::ReadRGB(std::ifstream &i)
{
	assert(header.imageType == Header::RGB); // For use loading uncompressed RGB files only

	assert(pixelData.size() == 0);
	unsigned bytesPerPixel = header.pixelSize/8;
	std::streamsize s = header.width * header.height * bytesPerPixel;
	pixelData.resize(s);

	i.read(&(*pixelData.begin()), s);

	// BGR ordering, swap to RGB
	for (std::vector<char>::iterator itt = pixelData.begin(); itt < pixelData.end(); itt += bytesPerPixel)
	{
		std::swap(*itt, *(itt+2));
	}

	// TODO flip the image - although probably not very important since the hardware doesn't care.
}

void Targa::ReadRLE(std::ifstream &i)
{
	assert(header.imageType == Header::RGBRLE); // For use loading run length encoded RGB files only

	assert(pixelData.size() == 0);
	unsigned bytesPerPixel = header.pixelSize/8;
	pixelData.resize( header.width * header.height * 4 );

	assert(i.good());
	assert(i.is_open());
	//std::vector<char> restOfFile(std::istreambuf_iterator<char>(i.rdbuf()), std::istreambuf_iterator<char>());

	//	int restOfFileSize = restOfFile.size();

	int x = 0;
	int y = 0;	

	//for (std::vector<char>::const_iterator itt = restOfFile.begin(); itt != restOfFile.end(); )
	for (std::istreambuf_iterator<char> itt(i.rdbuf()); itt != std::istreambuf_iterator<char>(); )
	{
		const bool runLengthPacket = (*itt & 0x80) != 0;
		const int packetSize = 1 + (*itt & 0x7f);

		++itt;
		if (runLengthPacket)
		{
			// get the pixel, and paste packetSize into the data
			const unsigned char b = *(itt++);
			const unsigned char g = *(itt++);
			const unsigned char r = *(itt++);
			const unsigned char a = (bytesPerPixel == 4) ? *(itt++) : 255;

			for (int p = 0; p < packetSize; ++p)
			{
				const int coord = x + y * 4;
				pixelData[ 0 + coord ] = r;
				pixelData[ 1 + coord ] = g;
				pixelData[ 2 + coord ] = b;
				pixelData[ 3 + coord ] = a;
				if (++x == header.width)
				{
					++y;
					x = 0;
				}
			}
		}
		else
		{
			for (int p = 0; p < packetSize; ++p)
			{
				const unsigned char b = *(itt++);
				const unsigned char g = *(itt++);
				const unsigned char r = *(itt++);
				const unsigned char a = (bytesPerPixel == 4) ? *(itt++) : 255;

				const int coord = x + y * 4;
				pixelData[ 0 + coord ] = r;
				pixelData[ 1 + coord ] = g;
				pixelData[ 2 + coord ] = b;
				pixelData[ 3 + coord ] = a;
				if (++x == header.width)
				{
					++y;
					x = 0;
				}
			}
		}
	}
}


std::ifstream& operator >>(std::ifstream &i, Targa &t)
{
	i >> t.header;
	i.seekg(t.header.idLength, std::ios_base::cur); // Skip comment
	if (t.header.imageType == Targa::Header::RGB)
		t.ReadRGB(i);
	else if (t.header.imageType == Targa::Header::RGBRLE)
		t.ReadRLE(i);;
	return i;
}
// TODO perhaps template across stream types
std::ifstream& operator >>(std::ifstream &i, Targa::Header &h)
{
	//i >> h.idLength;
	//i >> h.colourmapType;
	//i >> h.imageType;
	//i >> h.colourmapIndex;
	//i >> h.colourmapLength;
	//i >> h.colourmapSize;
	//i >> h.xOrigin;
	//i >> h.yOrigin;
	//i >> h.width;
	//i >> h.height;
	//i >> h.pixelSize;
	//i >> h.attributes;

	Read(i, h.idLength);
	Read(i, h.colourmapType);
	Read(i, h.imageType);
	Read(i, h.colourmapIndex);
	Read(i, h.colourmapLength);
	Read(i, h.colourmapSize);
	Read(i, h.xOrigin);
	Read(i, h.yOrigin);
	Read(i, h.width);
	Read(i, h.height);
	Read(i, h.pixelSize);
	Read(i, h.attributes);

	return i;
}