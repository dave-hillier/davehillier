#ifndef INCLUDE_FONT_H
#define INCLUDE_FONT_H

#include <Windows.h>
#include <string>

class Font
{
public:
	Font(const char* font, unsigned int size, bool bold, bool italic);
	~Font();

	bool initialise();
	void render(const char* pstr, float x, float y);
	void getTextSize(const char* pstr, float &xOut, float &yOut) const;
private:
	float renderChar (int ch, float x, float y) const;

	unsigned int width_;
	unsigned int height_;
	unsigned int spacing_;
	unsigned int texture_;
	unsigned int fontSize_;

	std::string name_;
	float   texCoords_[128-32][4];

	bool bold_;
	bool italic_;

	enum PaintResult {
		Fail,
		MoreData,
		Success,
	};

	bool createGDIFont( HDC hDC, HFONT* pFont );
	PaintResult paintAlphabet( HDC hDC, bool bMeasureOnly );



};
#endif