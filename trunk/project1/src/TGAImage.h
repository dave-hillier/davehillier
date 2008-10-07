/*
	TGAImage

	A simple wrapper for a TGA file.
*/

#ifndef TGAImage_h
#define TGAImage_h

#include <stdio.h>

class TGAImage  
{
public:
	TGAImage();
	~TGAImage();

	// Fast access to an image's size and bit depth
	static void imageInfo( const char *pFilename,unsigned int &width,unsigned int &height,unsigned int &bpp );

	// Data access members
	unsigned int	width() const			{ return width_; }
	unsigned int	height() const			{ return height_; }
	unsigned int	bpp() const				{ return bpp_; }

	const unsigned int*	pData() const	{ return pBuffer_; }

	bool	isTranslucent() const;

	bool load( const char *pszFilename );
	bool create( unsigned int width, unsigned int height, unsigned int bpp );
	bool save( const char *pszFilename,bool compress ) const;

	// Individual pixel rgba component get/set methods
	// assert( x<width() );
	// assert( y<height() );
	void setPixel( unsigned int x,unsigned int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a=255 );
	void getPixel( unsigned int x,unsigned int y,unsigned char *pR,unsigned char *pG,unsigned char *pB,unsigned char *pA ) const;

	// Methods for combined pixel get/set methods.
	// Components are argb (a=high byte, b=low byte)
	// assert( x<width() );
	// assert( y<height() );
	void setPixel( unsigned int x,unsigned int y,unsigned int argb );
	void getPixel( unsigned int x,unsigned int y,unsigned int *pARGB ) const;

	// Client takes ownership of new image.  Returns
	// NULL if fails.
	TGAImage* downsample() const;

private:

	// Canonical form revoked
	TGAImage( const TGAImage &copyMe );
	TGAImage &operator =( const TGAImage &copyMe );

	// This is the header at the start of all TGA files
	#pragma pack(1)
	struct TGAHeader
	{
		unsigned char IDLength;
		unsigned char ColormapType;
		unsigned char ImageType;
		unsigned char ColormapSpecification[5];
		unsigned short XOrigin;
		unsigned short YOrigin;
		unsigned short ImageWidth;
		unsigned short ImageHeight;
		unsigned char PixelDepth;
		unsigned char ImageDescriptor;
	};
	#pragma pack()

	// Create a palette for out image data
	bool	createPalette( unsigned int *pPalette ) const;

	// TODO use fstream
	bool	savePalettised( FILE *pFile,bool compress ) const;
	bool	saveTrueColour( FILE *pFile,bool compress ) const;

	// Internal helper functions for image conversion
	bool convert( const unsigned char *pBuffer,unsigned int bufferLength );
	bool convertTrueColourRLE( const unsigned char *pBuffer, const unsigned char *pBufferGuard, const TGAHeader &hdr );
	bool convertTrueColour( const unsigned char *pBuffer, const unsigned char *pBufferGuard, const TGAHeader &hdr );
	bool convertGreyScaleRLE( const unsigned char *pBuffer, const unsigned char *pBufferGuard, const TGAHeader &hdr );
	bool convertGreyScale( const unsigned char *pBuffer, const unsigned char *pBufferGuard, const TGAHeader &hdr );
	bool convertPalettisedRLE( const unsigned char *pBuffer, const unsigned char *pBufferGuard, const TGAHeader &hdr );
	bool convertPalettised( const unsigned char *pBuffer, const unsigned char *pBufferGuard, const TGAHeader &hdr );

	// Extracts colour map information
	void extractColourMapInfo( const TGAHeader &hdr,unsigned int *pFirstIndex,unsigned int *pEntries,unsigned int *pEntrySize );

	// Data members
	unsigned int		width_;
	unsigned int		height_;
	unsigned int		bpp_;
	unsigned int		*pBuffer_;

};


#endif 
