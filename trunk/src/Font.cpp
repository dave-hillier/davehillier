#include "Font.h"

#include <math.h>
#include <gl/GLU.h>
#include <fstream>

Font::Font(const char* font, unsigned int size, bool bold, bool italic) : 
	bold_(bold), italic_(italic), name_(font), texture_(0), width_(128),
	height_(128), fontSize_(size), spacing_(fontSize_ / 3)
{

}

Font::~Font()
{
	if (texture_)
	{
		glDeleteTextures(1, &texture_);
	}
}

bool Font::initialise()
{
	HDC hDC = CreateCompatibleDC( NULL );
	SetMapMode( hDC, MM_TEXT );

	HFONT hFont = NULL;
	bool ok = createGDIFont( hDC, &hFont );

	if (ok)
	{
		HFONT hFontOld = (HFONT) SelectObject( hDC, hFont );

		PaintResult p;
		while(  MoreData == (p = paintAlphabet( hDC, true )) )
		{
			width_ *= 2;
			height_ *= 2;
		}
		ok = p == Success;
		if (ok)
		{
			// Prepare to create a bitmap
			DWORD*      pBitmapBits;
			BITMAPINFO bmi;
			ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
			bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth       =  (int)width_;
			bmi.bmiHeader.biHeight      = -(int)height_;
			bmi.bmiHeader.biPlanes      = 1;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biBitCount    = 32;

			// Create a bitmap for the font
			HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
				(void**)&pBitmapBits, NULL, 0 );

			HGDIOBJ hbmOld = SelectObject( hDC, hbmBitmap );

			// Set text properties
			SetTextColor( hDC, RGB(255,255,255) );
			SetBkColor(   hDC, RGB(0,0,0) );
			SetTextAlign( hDC, TA_TOP );

			// Paint the alphabet onto the selected bitmap
			ok = paintAlphabet( hDC, false ) == Success;

			if (ok)
			{
				glGenTextures(1, &texture_);
				BITMAP	bitmap;
				GetObject(hbmBitmap,sizeof(BITMAP), &bitmap);

				glBindTexture(GL_TEXTURE_2D, texture_);	// Bind Our Texture
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

				// Its actually BGRA, but since we're rendering white...
				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bmBits);

				std::ofstream file;
				file.open("testt.raw", std::ios::out| std::ios::binary);
				if (file.is_open())
				{
					file.write((char*)bitmap.bmBits, 4* width_* height_);
					file.close();
				}
			}

			SelectObject( hDC, hbmOld );
			SelectObject( hDC, hFontOld );
			DeleteObject( hbmBitmap );
			DeleteObject( hFont );
			DeleteDC( hDC );
		}


	}

	return ok;
}

bool Font::createGDIFont( HDC hDC, HFONT* pFont )
{
	int nHeight    = fontSize_;
	DWORD dwBold   = bold_   ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = italic_ ? TRUE    : FALSE;
	*pFont         = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
		FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		VARIABLE_PITCH, name_.c_str() );

	return ( *pFont != NULL );
}

Font::PaintResult Font::paintAlphabet( HDC hDC, bool bMeasureOnly )
{
	SIZE size;
	char str[2] = "x";

	// Calculate the spacing between characters based on line height
	if( 0 == GetTextExtentPoint32( hDC, str, 1, &size ) )
		return Fail;
	spacing_ = (int)ceil(size.cy * 0.3f);

	// Set the starting point for the drawing
	DWORD x = spacing_;
	DWORD y = 0;

	// For each character, draw text on the DC and advance the current position
	for( char c = 32; c < 127; c++ )
	{
		str[0] = c;
		if( 0 == GetTextExtentPoint32( hDC, str, 1, &size ) )
			return Fail;

		if( (DWORD)(x + size.cx + spacing_) > width_ )
		{
			x  = spacing_;
			y += size.cy + 1;
		}

		// Check to see if there's room to write the character here
		if( y + size.cy > height_ )
			return MoreData;

		if( !bMeasureOnly )
		{
			// Perform the actual drawing
			if( 0 == ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL ) )
				return Fail;

			texCoords_[c-32][0] = ((FLOAT)(x))/width_;
			texCoords_[c-32][1] = ((FLOAT)(y))/height_;
			texCoords_[c-32][2] = ((FLOAT)(x + size.cx))/width_;
			texCoords_[c-32][3] = ((FLOAT)(y + size.cy))/height_;
		}

		x += size.cx + (2 * spacing_);
	}

	return Success;
}

float Font::renderChar (int ch, float x, float y) const
{
	float tx1 = texCoords_[ch-32][0];
	float ty1 = texCoords_[ch-32][1];
	float tx2 = texCoords_[ch-32][2];
	float ty2 = texCoords_[ch-32][3];

	float w = (tx2-tx1) * width_ ;
	float h = (ty2-ty1) * height_;


	glTexCoord2f(tx1, ty2);
	glVertex2f(x, y);

	glTexCoord2f(tx2, ty2);
	glVertex2f(x+w, y);

	glTexCoord2f(tx2, ty1);
	glVertex2f(x+w, y+h);

	glTexCoord2f(tx1, ty1);
	glVertex2f(x, y+h);


	return w;
}

void Font::render(const char* pstr, float xs, float ys)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glBegin (GL_QUADS);
	float x = xs;
	float y = ys - fontSize_;
	for (const char *ptr = pstr; *ptr; ++ptr)
	{
		if (*ptr == ' ')
		{
			x += spacing_;
		}
		else if (*ptr == '\n')
		{
			y -= fontSize_;
			x = xs;
		}	
		else if( (*ptr-32) < 0 || (*ptr-32) >= 128-32 )
		{
		}
		else
		{
			x += renderChar(*ptr, x, y);		
		}
	}	
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
}


void Font::getTextSize(const char* pstr, float &xOut, float &yOut) const
{
	float x = 0;
	float y = 0;
	for (const char *ptr = pstr; *ptr; ++ptr)
	{
		if (*ptr == ' ')
		{
			x += spacing_;
		}
		else if (*ptr == '\n')
		{
			y += fontSize_;
			xOut = std::max<float>(xOut, x);
		}	
		else if( (*ptr-32) < 0 || (*ptr-32) >= 128-32 )
		{
		}
		else
		{
			float tx1 = texCoords_[*ptr-32][0];
			float tx2 = texCoords_[*ptr-32][2];
			x += (tx2-tx1) * width_ ;		
		}
	}	
	yOut = y;
}
