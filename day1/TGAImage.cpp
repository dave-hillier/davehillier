// TGAImage.cpp: implementation of the TGAImage class.
//
//////////////////////////////////////////////////////////////////////

#include "TGAImage.h"

#include <iostream>
#include <cassert>
#include <fstream>
#include <vector>
//#include "DebugLog.h"

TGAImage::TGAImage()
:	pBuffer_( NULL ),
	width_( 0 ),
	height_( 0 ),
	bpp_( 0 )
{
	// Ctor
}

// static
void TGAImage::imageInfo( const char *pFilename, unsigned int &width,unsigned int &height,unsigned int &bpp )
{
	assert( pFilename );

	width = height = bpp = 0;

	std::ifstream file;
	file.open(pFilename, std::ios::binary|std::ios::in);
	if ( file.is_open() )
	{
		struct TGAHeader	hdr;
		file.read((char*)&hdr,sizeof(hdr));
		if ( file.good() )
		{
			// There are no identifying marks for a TGA.
			// Verify the format only.
			bool	trueColourOkay=((hdr.ImageType==2 || hdr.ImageType==10) && hdr.ColormapType==0);
			bool	palettisedOkay=((hdr.ImageType==1 || hdr.ImageType==9) && hdr.ColormapType!=0);
			bool	greyScaleOkay =((hdr.ImageType==3 || hdr.ImageType==11) && hdr.ColormapType==0);
			if ( palettisedOkay )
			{
				// Additional check for palettised images
				unsigned int bitCount		= (unsigned int)hdr.ColormapSpecification[4];
				if ( bitCount!=24 )
				{
					palettisedOkay=false;
				}
			}

			if ( trueColourOkay || palettisedOkay || greyScaleOkay )
			{
				width	= hdr.ImageWidth;
				height	= hdr.ImageHeight;
				bpp		= hdr.PixelDepth;
			}
		}
		file.close();
	}
}

bool TGAImage::create( unsigned int width,unsigned int height,unsigned int bpp )
{
	// Check bpp
	if ( bpp!=8 && bpp!=24 && bpp!=32 )
	{
		return false;
	}

	delete [] pBuffer_;	
	pBuffer_=NULL;

	pBuffer_	= new unsigned int[width*height];
	memset( pBuffer_,0x00,sizeof(unsigned int)*width*height );
	width_		= width;
	height_		= height;
	bpp_		= bpp;

	return true;
}

bool TGAImage::save( const char *pszFilename,bool compress ) const
{
	// Save
	bool	result=false;

	// Attempt to create the file
	FILE	*pFile=fopen(pszFilename,"wb");
	if ( pFile!=NULL )
	{
		// Define some helper members
		bool				palettised=(bpp_==8);
		int					imageType;

		if ( palettised )
		{
			imageType = (compress ? 9 : 1);
		}
		else
		{
			imageType = (compress ? 10 : 2);
		}

		// Construct the TGA header
		struct TGAHeader	hdr;
		memset( &hdr,0x00,sizeof(hdr) );
		hdr.IDLength					= 0;
		hdr.ColormapType				= (palettised ? 1 : 0);
		hdr.ImageType					= imageType;
		if ( palettised )
		{
			hdr.ColormapSpecification[0]	= 0;		// First two bytes are start index of colour map
			hdr.ColormapSpecification[1]	= 0;
			hdr.ColormapSpecification[2]	= 0;		// Next two bytes are the number of entries in the colour
			hdr.ColormapSpecification[3]	= 1;		// map.  We use the value 0x0100 i.e. 256
			hdr.ColormapSpecification[4]	= 24;		// Bits per colour map entry
		}

		hdr.XOrigin						= 0;
		hdr.YOrigin						= 0;
		hdr.ImageWidth					= width_;
		hdr.ImageHeight					= height_;
		hdr.PixelDepth					= bpp_;
		hdr.ImageDescriptor				= 0x00;			// We store images inverted

		// Indicate if we have alpha bits
		if ( bpp_==32 )
			hdr.ImageDescriptor |= 0x08;

		// Flag ourselves as successful and reset upon an error
		result=true;

		// Save out the header
		if ( result )
		{
			if ( fwrite(&hdr,sizeof(hdr),1,pFile)!=1 )
			{
				result=false;
			}
		}

		// Save out the pixel data
		if ( result )
		{
			if ( palettised )
				result=savePalettised( pFile,compress );
			else
				result=saveTrueColour( pFile,compress );
		}

		// Close the file and remove if it went pear-shaped
		fclose( pFile );
		if ( !result )
		{
			remove( pszFilename );
		}
	}

	return result;
}

void TGAImage::setPixel( unsigned int x,unsigned int y,unsigned int argb )
{
	assert( x<width() );
	assert( y<height() );
	pBuffer_[y*width_+x]=argb;
}

void TGAImage::setPixel( unsigned int x,unsigned int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a )
{
	assert( x<width() );
	assert( y<height() );

	unsigned int	colour=((unsigned int)a)<<24 | ((unsigned int)r)<<16 | ((unsigned int)g)<<8 | ((unsigned int)b);
	pBuffer_[y*width_+x] = colour;
}

bool TGAImage::createPalette( unsigned int *pPalette ) const
{
	// Clear the palette contents and scan the image data
	memset( pPalette,0x00,sizeof(unsigned int)*256 );
	unsigned int		numEntries=0;
	bool		result=true;
	for ( unsigned int y=0; y<height_; ++y )
	{
		for ( unsigned int x=0; x<width_; ++x )
		{
			unsigned int	colour=pBuffer_[y*width_+x];
			if ( colour&0xff000000 )
			{
				result=false;
				x=width_;
				y=height_;
				break;
			}

			// Find the entry or add a new one
			bool	found=false;
			for ( unsigned int i=0; i<numEntries; ++i )
			{
				if ( pPalette[i]==colour )
				{
					found=true;
					break;
				}
			}
			if ( !found )
			{
				if ( numEntries>=256 )
				{
					x=width_;
					y=height_;
					result=false;
					break;
				}
				else
				{
					pPalette[numEntries++] = colour;
				}
			}
		}
	}

	return result;
}

bool TGAImage::saveTrueColour( FILE *pFile,bool compress ) const
{
	// Save out the image data as either RLE or plain
	bool	result=true;
	if ( compress )
	{
		// Write out each scanline and compress if required
		unsigned int	numBytes=(bpp_==32 ? 4 : 3);
		for ( unsigned int y=height_-1; (int)y>=0; --y )
		{
			unsigned int	x=0;
			unsigned int	*p=pBuffer_+y*width_;
			unsigned int	totalCount=0;
			while ( x<width_ )
			{
				unsigned char	count=0;
				unsigned int	colour=*p;
				while ( x<width_ && colour==*p && count<0x80 )
				{
					++x;
					++p;
					++count;
				}

				// Write out a bit
				totalCount += count;
				--count;
				count |= 0x80;
				if ( fwrite( &count,sizeof(count),1,pFile )!=1 )
				{
					result=false;
					y=0;
					break;
				}
				if ( fwrite( &colour,1,numBytes,pFile )!=numBytes )
				{
					result=false;
					y=0;
					break;
				}
			}
			if ( totalCount!=width_ )
			{
				result=false;
				break;
			}
		}
	}
	else
	{
		// Stream them out as a series of 24 or 32-bit values
		for ( int y=height_-1; y>=0; --y )
		{
			unsigned int	numItems=width_;
			unsigned int	numBytes=(bpp_==32 ? 4 : 3);
			unsigned int	*p=pBuffer_ + y*width_;
			while ( numItems-- )
			{
				if ( fwrite(p,1,numBytes,pFile)!=numBytes )
				{
					result=false;
					y=0;
					break;
				}
				++p;
			}
		}
	}

	return result;
}

static unsigned char findPaletteIndex( unsigned int *pPalette,unsigned int colour )
{
	for ( unsigned int i=0; i<256; ++i )
	{
		if ( *pPalette++ == colour )
		{
			return (unsigned char)i;
		}
	}

	return 0;
}

bool TGAImage::savePalettised( FILE *pFile,bool compress ) const
{
	// We need a working buffer for the colour indices
	unsigned char	*pIndices=new  unsigned char[width_*height_ ];
	bool	result=true;
	unsigned int	*pPalette=new  unsigned int[256 ];

	if ( !createPalette(pPalette) )
	{
		result=false;
	}
	else
	{
		// Scan the image and build up the pixel index table
		memset( pIndices,0,width_*height_*sizeof(unsigned char) );
		for ( unsigned int y=0; y<height_; ++y )
		{
			for ( unsigned int x=0; x<width_; ++x )
			{
				unsigned int	offset=y*width_+x;
				pIndices[offset] = findPaletteIndex( pPalette,pBuffer_[offset] );
			}
		}

		// Save the palette out
		for ( unsigned int i=0; i<256; ++i )
		{
			unsigned int	colour=pPalette[i];
			fwrite( &colour,1,3,pFile );
		}

		// Now we can actually look to save the data.
		if ( compress )
		{
			for ( unsigned int y=height_-1; (int)y>=0; --y )
			{
				unsigned int			x=0;
				unsigned char 			*p=pIndices+y*width_;
				unsigned int			totalCount=0;
				while ( x<width_ )
				{
					unsigned char	count=0;
					unsigned char	colour=*p;
					while ( x<width_ && colour==*p && count<0x80 )
					{
						++x;
						++p;
						++count;
					}

					// Write out a bit
					totalCount += count;
					--count;
					count |= 0x80;
					if ( fwrite( &count,sizeof(count),1,pFile )!=1 )
					{
						result=false;
						y=0;
						break;
					}
					if ( fwrite( &colour,sizeof(colour),1,pFile )!=1 )
					{
						result=false;
						y=0;
						break;
					}
				}
				if ( totalCount!=width_ )
				{
					result=false;
					break;
				}
			}
		}
		else
		{
			// Stream them out as a series of index values
			for ( int y=height_-1; y>=0; --y )
			{
				unsigned char			*p=pIndices + y*width_;
				if ( fwrite(p,width_,1,pFile)!=1 )
				{
					result=false;
					y=0;
					break;
				}
			}
		}
	}

	// Tidy up
	delete [] pPalette ;
	delete [] pIndices ;

	return result;
}

TGAImage::~TGAImage()
{
	delete [] pBuffer_ ;
}

void TGAImage::getPixel( unsigned int x,unsigned int y,unsigned int *pARGB ) const
{
	assert( x<width() );
	assert( y<height() );
	const unsigned int	colour=pBuffer_[y*width_+x];
	*pARGB=colour;
}

void TGAImage::getPixel( unsigned int x,unsigned int y,unsigned char *pR,unsigned char *pG,unsigned char *pB,unsigned char *pA ) const
{
	assert( x<width() );
	assert( y<height() );
	const unsigned int	colour=pBuffer_[y*width_+x];
	*pB = (unsigned char)colour&0xff;
	*pG = (unsigned char)(colour>>8)&0xff;
	*pR = (unsigned char)(colour>>16)&0xff;
	*pA = (unsigned char)(colour>>24)&0xff;
}

bool TGAImage::load( const char *pszFilename )
{
	// TGA loader.
	bool	result=false;

	std::ifstream file;
	file.open(pszFilename, std::ios::in|std::ios::binary);

	if ( file.is_open() )
	{
		std::vector<char> restOfFile(std::istreambuf_iterator<char>(file.rdbuf()), 
									 std::istreambuf_iterator<char>());
		void* pData = &restOfFile.front();
		result = convert( static_cast<const unsigned char*>(pData),static_cast<unsigned int>(restOfFile.size()));
	}

	return result;
}

// virtual
bool TGAImage::convert( const unsigned char *pBuffer,unsigned int bufferLength )
{
	bool		result=false;
	const unsigned char	*pBufferGuard=pBuffer+bufferLength-1;

	// Extract the header
	struct TGAHeader	hdr;

	if ( bufferLength>=sizeof(hdr) )
	{
		memcpy( &hdr,pBuffer,sizeof(hdr) );
		pBuffer += sizeof(hdr);

		// Rudimentary checks on format.  We only support true-colour non-colourmapped images
		bool	trueColourOkay=((hdr.ImageType==2 || hdr.ImageType==10) && hdr.ColormapType==0);
		bool	palettisedOkay=((hdr.ImageType==1 || hdr.ImageType==9)  && hdr.ColormapType!=0);
		bool	greyScaleOkay =((hdr.ImageType==3 || hdr.ImageType==11) && hdr.ColormapType==0);

		// Additional checks for palettised images
		if ( palettisedOkay )
		{
			unsigned int		firstIndex;
			unsigned int		entries;
			unsigned int		bitCount;
			extractColourMapInfo( hdr,&firstIndex,&entries,&bitCount );
			if ( bitCount!=24 && bitCount!=32 )
			{
				palettisedOkay=false;
				trueColourOkay=false;
			}
			if ( palettisedOkay && hdr.PixelDepth!=8 )
			{
				palettisedOkay=false;
				trueColourOkay=false;
			}
		}

		if ( !trueColourOkay && !palettisedOkay && !greyScaleOkay )
		{
			// Stub
		}
		else
		{
			// Skip over the ID field
			if ( pBuffer+hdr.IDLength > pBufferGuard )
			{
				// Stub
			}
			else
			{
				pBuffer += hdr.IDLength;

				// Assign the data members
				width_		= hdr.ImageWidth;
				height_		= hdr.ImageHeight;
				bpp_		= hdr.PixelDepth;

				// Allocate our data block
				pBuffer_	= new  unsigned int[width_*height_ ];

				// Convert the image according to the different process
				switch( hdr.ImageType )
				{
				case 10:
					// True colour compressed
					result=convertTrueColourRLE( pBuffer,pBufferGuard,hdr );
					break;
				case 2:
					// True colour uncompressed
					result=convertTrueColour( pBuffer,pBufferGuard,hdr );
					break;
				case 9:
					// Palettised compressed using RLE
					result=convertPalettisedRLE( pBuffer,pBufferGuard,hdr );
					break;
				case 1:
					// Palettised uncompressed
					result=convertPalettised( pBuffer,pBufferGuard,hdr );
					break;
				case 11:
					// Grey scale compressed
					if (hdr.PixelDepth==8)
					{
						result=convertGreyScaleRLE( pBuffer,pBufferGuard,hdr );
					}
					else
					{
						//debugLog << "TGAImage: Unknown image format '" << 
						//hdr.ImageType << "'" << std::endl;
					}
					break;
				case 3:
					// Grey scale uncompressed
					if (hdr.PixelDepth==8)
					{
						result=convertGreyScale( pBuffer,pBufferGuard,hdr );
					}
					else
					{
						//debugLog << "TGAImage: Unknown image format '" <<
						//	hdr.ImageType << "'" << std::endl;
					}
					break;
				default:
					// Force an abort
					result=false;
                }

				// Check the guard
				if ( result )
				{
					result = (pBuffer <= pBufferGuard+1);
				}
            }
        }
    }

	return result;
}

bool TGAImage::convertTrueColourRLE( const unsigned char *pBuffer,const unsigned char *pBufferGuard,const TGAHeader &hdr )
{
	// True colour compressed using RLE
	bool	result=true;

	// How many actual colour bits are we dealing with
	bool	is32Bit=(hdr.PixelDepth==32);

	for ( unsigned int y=0; y<height_; ++y )
	{
		// Offset into the image by scanline which may be inverted
		unsigned int	scanlineOffset=y*width_;

		if ( (hdr.ImageDescriptor&0x20)==0x00 )
			scanlineOffset=(height_-y-1)*width_;
		
		unsigned int	*p=pBuffer_ + scanlineOffset;
		unsigned int	x=width_;
		while ( x>0 )
		{
			// RLE encoding via the high bit
			if ( pBuffer > pBufferGuard )
			{
				y=height_;
				result=false;
				break;
			}
			unsigned char	c=*pBuffer++;
			unsigned int	count=(c&0x7f)+1;
			if ( c&0x80 )
			{
				unsigned int	b=*pBuffer++;
				unsigned int	g=*pBuffer++;
				unsigned int	r=*pBuffer++;
				unsigned int	a=(is32Bit ? *pBuffer++ : 0xff);
				unsigned int	abgr=(a<<24)|(r<<16)|(g<<8)|b;
				while ( count-- )
				{
					*p++ = abgr;
					--x;
				}
			}
			else
			{
				// We're not RLE but the next count pixels are all different
				while ( count-- )
				{
					if ( pBuffer > pBufferGuard )
					{
						y=height_;
						count=0;
						x=0;
						result=false;
						break;
					}
					unsigned int	b=*pBuffer++;
					unsigned int	g=*pBuffer++;
					unsigned int	r=*pBuffer++;
					unsigned int	a=(is32Bit ? *pBuffer++ : 0xff);
					unsigned int	abgr=(a<<24)|(r<<16)|(g<<8)|b;
					*p++ = abgr;
					--x;
				}
			}
		}
	}	

	return result;
}

bool TGAImage::convertTrueColour( const unsigned char *pBuffer,const unsigned char *pBufferGuard,const TGAHeader& hdr )
{
	// Convert true colour (uncompressed)
	bool	result=true;

	// How many actual colour bits are we dealing with
	bool		is32Bit=(hdr.PixelDepth==32);

	for ( unsigned int y=0; y<height_; ++y )
	{
		// Offset into the image by scanline which may be inverted
		unsigned int	scanlineOffset=y*width_;

		if ( (hdr.ImageDescriptor&0x20)==0x00 )
			scanlineOffset=(height_-y-1)*width_;
		
		unsigned int	*p=pBuffer_ + scanlineOffset;
		for ( unsigned int x=width_; x>0; )
		{
			if ( pBuffer > pBufferGuard ) 
			{
				y=height_;
				result=false;
				break;
			}
			unsigned int	b=*pBuffer++;
			unsigned int	g=*pBuffer++;
			unsigned int	r=*pBuffer++;
			unsigned int	a=(is32Bit ? *pBuffer++ : 0xff);
			unsigned int	abgr=(a<<24)|(r<<16)|(g<<8)|b;
			*p++ = abgr;
			--x;
		}
    }

	return result;
}

void TGAImage::extractColourMapInfo( const TGAHeader &hdr,unsigned int *pFirstIndex,unsigned int *pEntries,unsigned int *pEntrySize )
{
	// Extract the colour map (palette) details
	// Little-endian
	*pFirstIndex	= (unsigned int)((unsigned int)hdr.ColormapSpecification[1])*256 + (unsigned int)hdr.ColormapSpecification[0];
	*pEntries		= (unsigned int)((unsigned int)hdr.ColormapSpecification[3])*256 + (unsigned int)hdr.ColormapSpecification[2];
	*pEntrySize		= (unsigned int)hdr.ColormapSpecification[4];
}

bool TGAImage::convertPalettisedRLE( const unsigned char *pBuffer,const unsigned char *pBufferGuard,const TGAHeader &hdr )
{
	// Convert a palettised RLE image
	bool	result=true;

	// Number of entries in the colour table
	unsigned int	firstIndex;
	unsigned int	numColours;
	unsigned int	bitCount;

	extractColourMapInfo( hdr,&firstIndex,&numColours,&bitCount );

	// On entry, the buffer points to our colour map.
	const unsigned char	*pColourMap=pBuffer;

	// Point to the image data
	unsigned int	entrySize=bitCount/8;

	pBuffer		+= entrySize * numColours;
	pColourMap	+= entrySize * firstIndex;

	// Create our palette and populate it
	unsigned int	*pPalette=new  unsigned int[256 ];
	for ( unsigned int i=0; i<numColours; ++i )
	{
		const unsigned char	b=*pColourMap++;
		const unsigned char	g=*pColourMap++;
		const unsigned char	r=*pColourMap++;
		const unsigned char	a=(entrySize==4 ? *pColourMap++ : 255);
		pPalette[i] = ((unsigned int)a)<<24 | ((unsigned int)r)<<16 | ((unsigned int)g)<<8 | ((unsigned int)b);
	}

	// Process the image data itself which are simply indices into the colour table
	for ( unsigned int y=0; y<height_; ++y )
	{
		// Offset into the image by scanline which may be inverted
		unsigned int	scanlineOffset=y*width_;

		if ( (hdr.ImageDescriptor&0x20)==0x00 )
			scanlineOffset=(height_-y-1)*width_;
		
		unsigned int	*p=pBuffer_ + scanlineOffset;
		unsigned int	x=width_;
		while ( x>0 )
		{
			// RLE encoding via the high bit
			if ( pBuffer > pBufferGuard )
			{
				y=height_;
				result=false;
				break;
			}
			unsigned char	c=*pBuffer++;
			unsigned int	count=(c&0x7f)+1;
			if ( c&0x80 )
			{
				// Automatic conversion of unsigned char to unsigned int
				unsigned int	index=*pBuffer++;
				unsigned int	colour=pPalette[index];
				while ( count-- )
				{
					*p++ = colour;
					--x;
				}
			}
			else
			{
				// We're not RLE but the next count pixels are all different
				while ( count-- )
				{
					if ( pBuffer > pBufferGuard )
					{
						y=height_;
						count=0;
						x=0;
						result=false;
						break;
					}
					// Automatic conversion of unsigned char to unsigned int
					unsigned int colour=pPalette[*pBuffer++];
					*p++ = colour;
					--x;
				}
			}
		}
	}

	delete []  pPalette ;

	return result;
}

bool TGAImage::convertPalettised( const unsigned char *pBuffer,const unsigned char *pBufferGuard,const TGAHeader &hdr )
{
	// Convert a palettised image
	bool	result=true;

	// Number of entries in the colour table
	unsigned int	firstIndex;
	unsigned int	numColours;
	unsigned int	bitCount;

	extractColourMapInfo( hdr,&firstIndex,&numColours,&bitCount );

	// On entry, the buffer points to our colour map.
	const unsigned char	*pColourMap=pBuffer;

	// Point to the image data
	unsigned int	entrySize=bitCount/8;

	pBuffer		+= entrySize * numColours;
	pColourMap	+= entrySize * firstIndex;

	// Create our palette and populate it
	unsigned int *pPalette=new  unsigned int[256 ];
	for ( unsigned int i=0; i<numColours; ++i )
	{
		const unsigned char	b=*pColourMap++;
		const unsigned char	g=*pColourMap++;
		const unsigned char	r=*pColourMap++;
		const unsigned char	a=(entrySize==4 ? *pColourMap++ : 255);
		pPalette[i] = ((unsigned int)a)<<24 | ((unsigned int)r)<<16 | ((unsigned int)g)<<8 | ((unsigned int)b);
	}

	// Process the image data itself which are simply indices into the colour table
	for ( unsigned int y=0; y<height_; ++y )
	{
		// Offset into the image by scanline which may be inverted
		unsigned int	scanlineOffset=y*width_;

		if ( (hdr.ImageDescriptor&0x20)==0x00 )
			scanlineOffset=(height_-y-1)*width_;
		
		unsigned int	*p=pBuffer_ + scanlineOffset;
		for ( unsigned int x=width_; x>0; )
		{
			if ( pBuffer > pBufferGuard ) 
			{
				y=height_;
				result=false;
				break;
			}
			// Automatic conversion of unsigned char to unsigned int
			unsigned int colour=pPalette[*pBuffer++];
			*p++ = colour;
			--x;
		}
    }

	delete [] ( pPalette );

	return result;
}

bool TGAImage::convertGreyScaleRLE( const unsigned char *pBuffer,const unsigned char *pBufferGuard,const TGAHeader &hdr )
{
	assert(hdr.PixelDepth==8);

	// True colour compressed using RLE
	bool	result=true;

	for ( unsigned int y=0; y<height_; ++y )
	{
		// Offset into the image by scanline which may be inverted
		unsigned int	scanlineOffset=y*width_;

		if ( (hdr.ImageDescriptor&0x20)==0x00 )
			scanlineOffset=(height_-y-1)*width_;
		
		unsigned int	*p=pBuffer_ + scanlineOffset;
		unsigned int	x=width_;
		while ( x>0 )
		{
			// RLE encoding via the high bit
			if ( pBuffer > pBufferGuard )
			{
				y=height_;
				result=false;
				break;
			}
			unsigned char	c=*pBuffer++;
			unsigned int	count=(c&0x7f)+1;
			if ( c&0x80 )
			{
				unsigned int	v=*pBuffer++;
				unsigned int	a=0xff;
				unsigned int	abgr=(a<<24)|(v<<16)|(v<<8)|v;
				while ( count-- )
				{
					*p++ = abgr;
					--x;
				}
			}
			else
			{
				// We're not RLE but the next count pixels are all different
				while ( count-- )
				{
					if ( pBuffer > pBufferGuard )
					{
						y=height_;
						count=0;
						x=0;
						result=false;
						break;
					}
					unsigned int	v=*pBuffer++;
					unsigned int	a=0xff;
					unsigned int	abgr=(a<<24)|(v<<16)|(v<<8)|v;
					*p++ = abgr;
					--x;
				}
			}
		}
	}	

	return result;
}

bool TGAImage::convertGreyScale( const unsigned char *pBuffer,const unsigned char *pBufferGuard,const TGAHeader& hdr )
{
	assert(hdr.PixelDepth==8);

	// Convert grey-scale (uncompressed)
	bool	result=true;

	for ( unsigned int y=0; y<height_; ++y )
	{
		// Offset into the image by scanline which may be inverted
		unsigned int	scanlineOffset=y*width_;

		if ( (hdr.ImageDescriptor&0x20)==0x00 )
			scanlineOffset=(height_-y-1)*width_;
		
		unsigned int	*p=pBuffer_ + scanlineOffset;
		for ( unsigned int x=width_; x>0; )
		{
			if ( pBuffer > pBufferGuard ) 
			{
				y=height_;
				result=false;
				break;
			}
			// Make r, g and b all the same.  Set alpha to opaque.
			unsigned int	v = *pBuffer++;
			unsigned int	a = 0xff;
			unsigned int	abgr=(a<<24)|(v<<16)|(v<<8)|v;
			*p++ = abgr;
			--x;
		}
    }

	return result;
}

static bool IsValidImageSize( unsigned int s )
{
	// Must be a power of 2 and max 256 in size
	switch( s )
	{
	case 2:
	case 4:
	case 8:
	case 16:
	case 32:
	case 64:
	case 128:
	case 256:
	case 512:
	case 1024:
	case 2048:
	case 4096:
		return true;
	}

	return false;
}

static void AveragePixel( const TGAImage &source,unsigned int x,unsigned int y,TGAImage *pDest,unsigned int dx,unsigned int dy )
{
	assert( pDest );
	assert( dx<pDest->width() );
	assert( dy<pDest->height() );
	assert( x<source.width()-1 );
	assert( y<source.height()-1 );
	
	unsigned int		red0=0, grn0=0, blu0=0, alp0=0;
	unsigned int		red1=0, grn1=0, blu1=0, alp1=0;
	unsigned int		red2=0, grn2=0, blu2=0, alp2=0;
	unsigned int		red3=0, grn3=0, blu3=0, alp3=0;
	
	source.getPixel( x,y,(unsigned char *)&red0,(unsigned char *)&grn0,(unsigned char *)&blu0,(unsigned char *)&alp0 );
	source.getPixel( x+1,y,(unsigned char *)&red1,(unsigned char *)&grn1,(unsigned char *)&blu1,(unsigned char *)&alp1 );
	source.getPixel( x,y+1,(unsigned char *)&red2,(unsigned char *)&grn2,(unsigned char *)&blu2,(unsigned char *)&alp2 );
	source.getPixel( x+1,y+1,(unsigned char *)&red3,(unsigned char *)&grn3,(unsigned char *)&blu3,(unsigned char *)&alp3 );

	// Average them out
	unsigned int		red=(red0+red1+red2+red3) >> 2;			// Faster than doing a divide by 4
	unsigned int		grn=(grn0+grn1+grn2+grn3) >> 2;
	unsigned int		blu=(blu0+blu1+blu2+blu3) >> 2;
	unsigned int		alp=(alp0+alp1+alp2+alp3) >> 2;
	
	// Alpha is a special case.  Look to preserve 1-bit alpha
	if ( (alp0!=0 && alp0!=255) || (alp1!=0 && alp1!=255) || (alp2!=0 && alp2!=255) || (alp3!=0 && alp3!=255) )
	{
		// Multi-bit alpha so interpolated value is fine
	}
	else
	{
		// We had single-bit so force single bit again
	}

	// Assign the interpolated value into the image
	pDest->setPixel( dx,dy,(unsigned char)red,(unsigned char)grn,(unsigned char)blu,(unsigned char)alp );
}

TGAImage* TGAImage::downsample() const
{
	// Takes the supplied image and generates a downsampled one
	// of half the dimensions. This method can cope with non-power of 2 images,
	// although the result may not be completely "correct" i.e., it just makes sure
	// that the width and height are both even before downsampling.
	unsigned int	halfWidth=width() / 2;
	unsigned int	halfHeight=height() / 2;

	TGAImage *pSmall = 0;

	if ( halfWidth>1 && halfHeight>1 )
	{
		pSmall =new  TGAImage();
		unsigned int		reqdBpp=(bpp()==8 ? 24 : bpp());
		if ( pSmall->create(halfWidth,halfHeight,reqdBpp) )
		{
			unsigned int		w=width() & 0xFFFFFFFE;	// The & 0xFFFFFFFE is there to take width and height
			unsigned int		h=height() & 0xFFFFFFFE; // as even numbers before applying the box filter.
			unsigned int		sy=0;
			for ( unsigned int y=0; y<h; y+=2 )
			{
				unsigned int	sx=0;
				for ( unsigned int x=0; x<w; x+=2 )
				{
					AveragePixel( *this,x,y,pSmall,sx,sy );
					++sx;
				}
				++sy;
			}
		}
		else
		{
			delete pSmall;
			pSmall=NULL;
		}
	}

	return pSmall;
}

bool TGAImage::isTranslucent() const
{
	bool			result=false;
	const unsigned int	h=height();
	const unsigned int	w=width();
	for ( unsigned int y=0; y<h; ++y )
	{
		for ( unsigned int x=0; x<w; ++x )
		{
			unsigned char	a, r, g, b;
			getPixel( x,y,&r,&g,&b,&a );
			if ( a!=0 && a!=255 )
			{
				result=true;
				x=w;
				y=h;
				break;
			}
		}
	}

	return result;
}


