#pragma once

#include <vector>
#include <fstream>

class Jpeg
{
public:
	static bool Load(const std::string &jpegName, Jpeg &target, bool asAlpha);

	Jpeg();
	~Jpeg(void);

	const char *data() const { return &*pixelData.begin(); }
	unsigned width() const {return jWidth;}
	unsigned height() const {return jHeight;}
	unsigned char bytesPerPixel() const { return jBPP; }

private:
	std::vector<char> pixelData;

	unsigned jWidth;
	unsigned jHeight;
	unsigned char jBPP;

};

