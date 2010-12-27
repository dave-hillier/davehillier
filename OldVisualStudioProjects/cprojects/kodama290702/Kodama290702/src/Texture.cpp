// TextureManager.cpp: implementation of the TextureManager class.
//
//////////////////////////////////////////////////////////////////////
#include "SDL.h"
#include <windows.h>

#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Texture.h"
#include "Console.h"
#include "ijl.h"

TextureManager texture;

// need reloading for screen size changes
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextureManager::TextureManager()
{
	numtextures = 0;

	for (int i = 0; i < MAX_TEXTURES; i++)
		textures[i].texID = 0;
}

TextureManager::~TextureManager()
{

}
int TextureManager::LoadTexture(char *name)
{
	byte *imageData;
	int width, height;//, bpp, texid, type;
	int i;

	// check to see if its already loaded
	for (i=0; i < numtextures; i++)
	{
		if (stricmp(name, textures[i].name) == 0)
			return textures[i].texID;
	}

	// check extension
	int len = strlen(name);
	if (stricmp(name+len-3, "tga") == 0)
	{
		
		// load the file
		LoadTGA(name, &imageData, &width, &height);
		if (imageData)
		{
			strcpy (textures[numtextures].name, name);
			textures[numtextures].texID = numtextures+1;
			
			Bind(numtextures+1);
			// set the filters
			
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			
			gluBuild2DMipmaps (GL_TEXTURE_2D, 4, width, height, 
				GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			
			delete imageData;
			
			return ++numtextures;
		}
	}
	else if (stricmp(name+len-3, "jpg") == 0)
	{
		// load the file
		LoadJPG(name, &imageData, &width, &height);

		if (imageData)
		{
			strcpy (textures[numtextures].name, name);
			textures[numtextures].texID = numtextures+1;
			
			Bind(numtextures+1);
			// set the filters
			
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			
			gluBuild2DMipmaps (GL_TEXTURE_2D, 3, width, height, 
				GL_RGB, GL_UNSIGNED_BYTE, imageData);
			
			delete imageData;
			
			return ++numtextures;
		}
	}

	return 0;
}

int TextureManager::GetId(char *name)
{
	int i;
	for (i=0; i < numtextures; i++)
	{
		if (stricmp(name, textures[i].name) == 0)
			return textures[i].texID;
	}
	return 0;
}

void TextureManager::Bind(char *name)
{
	int i;
	for (i=0; i < numtextures; i++)
	{
		if (stricmp(name, textures[i].name) == 0)
		{
			glBindTexture (GL_TEXTURE_2D, textures[i].texID);
			return;
		}
	}
}

void TextureManager::Bind(int id)
{
	glBindTexture (GL_TEXTURE_2D, id);
}


void TextureManager::KillTexture(char *name)
{
	int i;
	for (i=0; i < numtextures; i++)
	{
		if (stricmp(name, textures[i].name) == 0)
		{
			glDeleteTextures(1, &textures[i].texID);
			return;
		}
	}
}


/*
=========================================================

TARGA LOADING

=========================================================
*/

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


// make it read in 64k blocks??
int loadfile(char *name, byte **buff)
{
	FILE *fp;

	if (fp = fopen(name, "rb"))
	{
		int		len;

		fseek (fp, 0, SEEK_END);
		len = ftell (fp);
		rewind(fp);

		*buff = (byte *)malloc(len);
		fread (*buff, 1, len, fp);
		fclose(fp);
		return len;
	}
	return 0;
}
/*
=============
LoadTGA
=============
*/
void TextureManager::LoadTGA (char *name, byte **pic, int *width, int *height)
{
	int		columns, rows, numPixels;
	byte	*pixbuf;
	int		row, column;
	byte	*buf_p;
	byte	*buffer = NULL;
	int		length;
	TargaHeader		targa_header;
	byte			*targa_rgba;

	*pic = NULL;

	//
	// load the file
	//
	length = loadfile (name, &buffer);
	if (!buffer)
	{
		con->Printf ("Bad tga file %s\n", name);
		return;
	}

	buf_p = buffer;

	targa_header.id_length = *buf_p++;
	targa_header.colormap_type = *buf_p++;
	targa_header.image_type = *buf_p++;
	
	targa_header.colormap_index =  ( *((short *)buf_p) );
	buf_p+=2;
	targa_header.colormap_length =  ( *((short *)buf_p) );
	buf_p+=2;
	targa_header.colormap_size = *buf_p++;
	targa_header.x_origin =  ( *((short *)buf_p) );
	buf_p+=2;
	targa_header.y_origin =  ( *((short *)buf_p) );
	buf_p+=2;
	targa_header.width =  ( *((short *)buf_p) );
	buf_p+=2;
	targa_header.height =  ( *((short *)buf_p) );
	buf_p+=2;
	targa_header.pixel_size = *buf_p++;
	targa_header.attributes = *buf_p++;

	if (targa_header.image_type!=2 
		&& targa_header.image_type!=10) 
		con->Printf  ("LoadTGA: Only type 2 and 10 targa RGB images supported\n");

	if (targa_header.colormap_type !=0 
		|| (targa_header.pixel_size!=32 && targa_header.pixel_size!=24))
		con->Printf  ("LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	if (width)
		*width = columns;
	if (height)
		*height = rows;

	targa_rgba = (byte *)malloc (numPixels*4);
	*pic = targa_rgba;

	if (targa_header.id_length != 0)
		buf_p += targa_header.id_length;  // skip TARGA image comment
	
	if (targa_header.image_type==2) {  // Uncompressed, RGB images
		for(row=rows-1; row>=0; row--) {
			pixbuf = targa_rgba + row*columns*4;
			for(column=0; column<columns; column++) {
				unsigned char red,green,blue,alphabyte;
				switch (targa_header.pixel_size) {
					case 24:
							
							blue = *buf_p++;
							green = *buf_p++;
							red = *buf_p++;
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = 255;
							break;
					case 32:
							blue = *buf_p++;
							green = *buf_p++;
							red = *buf_p++;
							alphabyte = *buf_p++;
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = alphabyte;
							break;
				}
			}
		}
	}
	else if (targa_header.image_type==10) {   // Runlength encoded RGB images
		unsigned char red,green,blue,alphabyte,packetHeader,packetSize,j;
		for(row=rows-1; row>=0; row--) {
			pixbuf = targa_rgba + row*columns*4;
			for(column=0; column<columns; ) {
				packetHeader= *buf_p++;
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80) {        // run-length packet
					switch (targa_header.pixel_size) {
						case 24:
								blue = *buf_p++;
								green = *buf_p++;
								red = *buf_p++;
								alphabyte = 255;
								break;
						case 32:
								blue = *buf_p++;
								green = *buf_p++;
								red = *buf_p++;
								alphabyte = *buf_p++;
								break;
					}
	
					for(j=0;j<packetSize;j++) {
						*pixbuf++=red;
						*pixbuf++=green;
						*pixbuf++=blue;
						*pixbuf++=alphabyte;
						column++;
						if (column==columns) { // run spans across rows
							column=0;
							if (row>0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row*columns*4;
						}
					}
				}
				else {                            // non run-length packet
					for(j=0;j<packetSize;j++) {
						switch (targa_header.pixel_size) {
							case 24:
									blue = *buf_p++;
									green = *buf_p++;
									red = *buf_p++;
									*pixbuf++ = red;
									*pixbuf++ = green;
									*pixbuf++ = blue;
									*pixbuf++ = 255;
									break;
							case 32:
									blue = *buf_p++;
									green = *buf_p++;
									red = *buf_p++;
									alphabyte = *buf_p++;
									*pixbuf++ = red;
									*pixbuf++ = green;
									*pixbuf++ = blue;
									*pixbuf++ = alphabyte;
									break;
						}
						column++;
						if (column==columns) { // pixel packet run spans across rows
							column=0;
							if (row>0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row*columns*4;
						}						
					}
				}
			}
			breakOut:;
		}
	}

	free (buffer);
}
void TextureManager::LoadJPG (char *name, byte **pic, int *width, int *height)
{
	int channels = 3;
    JPEG_CORE_PROPERTIES image;
    ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );
    if( ijlInit( &image ) != IJL_OK )
    {
		con->Printf("ijl init error\n");
        return;
    }
	image.JPGFile = name;
	
	// if ((RetVal = ijlRead(&image,IJL_JFILE_READPARAMS)) == IJL_OK)
    if (ijlRead(&image,IJL_JFILE_READPARAMS) == IJL_OK)
	{
        *height = image.JPGHeight;
        *width = image.JPGWidth;
		
		*pic = new BYTE[(*height) * (*width) * channels];
		
		if (*pic)
        {
            image.DIBBytes = *pic;
            image.DIBColor = IJL_RGB;
            image.DIBHeight = *height;
            image.DIBWidth = *width;
            //image.DIBColor = IJL_YCBCR;
            image.DIBChannels = channels;
			
			if (ijlRead(&image,IJL_JFILE_READWHOLEIMAGE) == IJL_OK)
			{
				// ok
			}
		}
		// else
		// allocation error
		
	}
}


