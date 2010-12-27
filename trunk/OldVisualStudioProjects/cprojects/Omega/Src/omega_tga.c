#include "omega_common.h"
#include "omega_gl.h"

TargaHeader		targa_header;
byte			*targa_rgba;

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
		MessageBox(NULL, 
			"LoadTGA: Only type 2 and 10 targa RGB images supported", 
			"Error", MB_OK);
	}

	if (targa_header.colormap_type !=0 
		|| (targa_header.pixel_size!=32 && targa_header.pixel_size!=24))
	{
	//	Sys_Error ("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");
		MessageBox(NULL, 
			"Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)", 
			"Error", MB_OK);
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


void ScreenShot (void) 
{
	FILE *fileptr;
	byte		*buffer;
	char		pcxname[80]; 
	int			i, c, temp;
// 
// find a file name to save it to 
// 
	strcpy(pcxname,"screenshot/ss.tga");
		
	buffer = malloc(current_vidmode.width*current_vidmode.height*3 + 18);
	memset (buffer, 0, 18);
	buffer[2] = 2;		// uncompressed type
	buffer[12] = current_vidmode.width&255;
	buffer[13] = current_vidmode.width>>8;
	buffer[14] = current_vidmode.height&255;
	buffer[15] = current_vidmode.height>>8;
	buffer[16] = current_vidmode.bpp;	// pixel size

	glReadPixels (0, 0, current_vidmode.width, current_vidmode.height, 
		GL_RGB, GL_UNSIGNED_BYTE, buffer+18 ); 

	// swap rgb to bgr
	c = 18+current_vidmode.width*current_vidmode.height*3;
	for (i=18 ; i<c ; i+=3)
	{
		temp = buffer[i];
		buffer[i] = buffer[i+2];
		buffer[i+2] = temp;
	}

	if (fileptr = fopen(pcxname, "w"))
	{
		fwrite(buffer, current_vidmode.width*current_vidmode.height*3 + 18, 1,fileptr);
		fclose(fileptr);
		free (buffer);
		ConPrintf ("Wrote %s\n", pcxname);
	}
	else
		ConPrintf ("Couldnt write %s\n", pcxname);

} 

