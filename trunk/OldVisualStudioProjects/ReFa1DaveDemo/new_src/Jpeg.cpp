#include "Jpeg.h"

#include <windows.h>
#include <gl/GLU.h>
#include "../ijl.h"


Jpeg::Jpeg()
{
}

Jpeg::~Jpeg(void)
{
}


bool Jpeg::Load(const std::string &jpegName, Jpeg &target, bool asAlpha)
{
	bool result = false;
	JPEG_CORE_PROPERTIES image;
	ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );
	if( ijlInit( &image ) != IJL_OK )
	{
		return false;
	}
	image.JPGFile = jpegName.c_str();
	if (ijlRead(&image,IJL_JFILE_READPARAMS) == IJL_OK)
	{
		const int channels = asAlpha ? 1 : 3;
		const int imgSize = image.JPGHeight * image.JPGWidth * channels;
		unsigned char *img = new unsigned char[imgSize];
		if (img)
		{
			image.DIBBytes = img;
			image.DIBColor = asAlpha ? IJL_G : IJL_RGB;
			image.DIBHeight = image.JPGHeight;
			image.DIBWidth = image.JPGWidth;
			image.DIBChannels = channels;

			if (ijlRead(&image,IJL_JFILE_READWHOLEIMAGE) == IJL_OK)
			{
				target.pixelData.resize(imgSize);
				std::copy(img, img+imgSize, target.pixelData.begin());
				target.jWidth = image.JPGWidth;
				target.jHeight = image.JPGHeight;
				target.jBPP = channels;
				result = true;
			}
		}

		delete [] img;
	}
	ijlFree(&image);

	return result;
}

