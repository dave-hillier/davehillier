// Font.cpp: implementation of the Font class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdarg.h>

#include "Font.h"
extern HDC	hDC;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Font *arial;

Font::Font(char *name)
{
	GLYPHMETRICSFLOAT agmf[256]; 
	HFONT font = CreateFont(	-100,					// Height Of Font
				0,							// Width Of Font
				0,							// Angle Of Escapement
				0,							// Orientation Angle
				FW_NORMAL,					// Font Weight
				FALSE,						// Italic
				FALSE,						// Underline
				FALSE,						// Strikeout
				ANSI_CHARSET,				// Character Set Identifier
				OUT_TT_PRECIS,				// Output Precision
				CLIP_DEFAULT_PRECIS,		// Clipping Precision
				ANTIALIASED_QUALITY,		// Output Quality
				FF_DONTCARE|DEFAULT_PITCH,	// Family And Pitch
				"Arial");					// Font Name

	SelectObject(hDC, font);				// Selects The Font We Created


	base = glGenLists(256);
	// try use true type
	// generate lists
	/*wglUseFontBitmaps(hDC, 0, 255, base );*/
	wglUseFontOutlines(	hDC,		// Select The Current DC
				0,					// Starting Character
				255,				// Number Of Display Lists To Build
				base,				// Starting Display Lists
				0.0f,				// Deviation From The True Outlines
				0.0f,				// Font Thickness In The Z Direction
				WGL_FONT_POLYGONS,	// Use Polygons, Not Lines
				agmf);				// Address Of Buffer To Recieve Data

	// free hdc etc
}

Font::~Font()
{
	glDeleteLists(base, 256);
}

void Font::printf(char *format, ...)
{
	char buffer[1000];
	int len;
	va_list		ap;

	va_start(ap, format);
	vsprintf(buffer, format, ap);
	va_end(ap);	

	len = strlen(buffer);
	glListBase(base);
	glCallLists(len, GL_UNSIGNED_BYTE, buffer);

}
