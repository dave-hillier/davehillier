#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include "p6_common.h"

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;

TargaHeader		targa_header;
byte			*targa_rgba;
int texture_extension_number = 0;

int fgetLittleShort (FILE *f)
{
	byte	b1, b2;

	b1 = fgetc(f);
	b2 = fgetc(f);

	return (short)(b1 + b2*256);
}

int fgetLittleLong (FILE *f)
{
	byte	b1, b2, b3, b4;

	b1 = fgetc(f);
	b2 = fgetc(f);
	b3 = fgetc(f);
	b4 = fgetc(f);

	return b1 + (b2<<8) + (b3<<16) + (b4<<24);
}


/*
=============
LoadTGA
=============
*/
int LoadTGA (FILE *fin)
{
	int				columns, rows, numPixels;
	byte			*pixbuf;
	int				row, column;

	targa_header.id_length = fgetc(fin);
	targa_header.colormap_type = fgetc(fin);
	targa_header.image_type = fgetc(fin);
	
	targa_header.colormap_index = fgetLittleShort(fin);
	targa_header.colormap_length = fgetLittleShort(fin);
	targa_header.colormap_size = fgetc(fin);
	targa_header.x_origin = fgetLittleShort(fin);
	targa_header.y_origin = fgetLittleShort(fin);
	targa_header.width = fgetLittleShort(fin);
	targa_header.height = fgetLittleShort(fin);
	targa_header.pixel_size = fgetc(fin);
	targa_header.attributes = fgetc(fin);

	if (targa_header.pixel_size == 8)
	{
		targa_rgba = (byte *) malloc (targa_header.width * targa_header.height);
		fread(targa_rgba, sizeof(byte), targa_header.width * targa_header.height, fin);

		fclose(fin);
		return targa_header.pixel_size/8;
	}
	if (targa_header.image_type!=2 
		&& targa_header.image_type!=10) 
	{
	//	Sys_Error ("LoadTGA: Only type 2 and 10 targa RGB images supported\n");
	/*	MessageBox(NULL, 
			"LoadTGA: Only type 2 and 10 targa RGB images supported", 
			"Error", MB_OK);*/
	}

	if (targa_header.colormap_type !=0 
		|| (targa_header.pixel_size!=32 && targa_header.pixel_size!=24))
	{
	//	Sys_Error ("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");
	/*	MessageBox(NULL, 
			"Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)", 
			"Error", MB_OK);*/
	}

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	targa_rgba = (byte *) malloc (numPixels*4);
	
	if (targa_header.id_length != 0)
		fseek(fin, targa_header.id_length, SEEK_CUR);  // skip TARGA image comment
	
	if (targa_header.image_type==2) {  // Uncompressed, RGB images
		for(row=rows-1; row>=0; row--) {
			pixbuf = targa_rgba + row*columns*4;
			for(column=0; column<columns; column++) {
				unsigned char red,green,blue,alphabyte;
				switch (targa_header.pixel_size) {
					case 24:
							
							blue = getc(fin);
							green = getc(fin);
							red = getc(fin);
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = 255;
							break;
					case 32:
							blue = getc(fin);
							green = getc(fin);
							red = getc(fin);
							alphabyte = getc(fin);
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
				packetHeader=getc(fin);
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80) {        // run-length packet
					switch (targa_header.pixel_size) {
						case 24:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								alphabyte = 255;
								break;
						case 32:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								alphabyte = getc(fin);
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
									blue = getc(fin);
									green = getc(fin);
									red = getc(fin);
									*pixbuf++ = red;
									*pixbuf++ = green;
									*pixbuf++ = blue;
									*pixbuf++ = 255;
									break;
							case 32:
									blue = getc(fin);
									green = getc(fin);
									red = getc(fin);
									alphabyte = getc(fin);
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
	
	fclose(fin);
	return targa_header.pixel_size/8;
}


int GetTexture (char *name)
{
	FILE *f;
	int format, bpp;

	if (f = fopen(name, "rb"))
	{

		format = LoadTGA(f);

		glBindTexture(GL_TEXTURE_2D, texture_extension_number++);
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_DECAL/// modulate for shading
		if (format == 1)
			bpp = format = GL_ALPHA;//GL_COLOR_INDEX;
		else if (format == 3)
			bpp = GL_RGBA;
		else if (format == 4)
			bpp = GL_RGBA;

/*	gluBuild2DMipmaps(GL_TEXTURE_2D,
			format,//GL_RGB, 
			targa_header.width, 
			targa_header.height, 
			bpp, //GL_RGBA,
			GL_UNSIGNED_BYTE, targa_rgba);*/

		glTexImage2D(GL_TEXTURE_2D, 
			0, format,//GL_RGB, 
			targa_header.width, 
			targa_header.height, 
			0, bpp, //GL_RGBA,
			GL_UNSIGNED_BYTE, targa_rgba);
		
		free (targa_rgba);// Dont need it once its uploaded

		return texture_extension_number-1;
	}
	
	return 0;
	//	ConPrintf("GetTexture: Cant open %s\n", name);

}