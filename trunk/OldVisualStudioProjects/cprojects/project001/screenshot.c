#include "project001.h"
#include <malloc.h>
typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;



void ScreenShot (void) 
{
	FILE *fileptr;
	byte		*buffer;
	char		pcxname[80]; 
//	char		checkname[256];
	int			i, c, temp;
// 
// find a file name to save it to 
// 
	strcpy(pcxname,"save.tga");
		
/*	for (i=0 ; i<=99 ; i++) 
	{ 
		pcxname[5] = i/10 + '0'; 
		pcxname[6] = i%10 + '0'; 
		sprintf (checkname, "%s/%s", com_gamedir, pcxname);
		if (Sys_FileTime(checkname) == -1)// FIX ME
			break;	// file doesn't exist
	} 
	if (i==100) 
	{
		Con_Printf ("SCR_ScreenShot_f: Couldn't create a PCX file\n"); 
		return;
 	}*/

	buffer = malloc(WIDTH*HEIGHT*3 + 18);
	memset (buffer, 0, 18);
	buffer[2] = 2;		// uncompressed type
	buffer[12] = WIDTH&255;
	buffer[13] = WIDTH>>8;
	buffer[14] = HEIGHT&255;
	buffer[15] = HEIGHT>>8;
	buffer[16] = 24;	// pixel size

	glReadPixels (0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer+18 ); 

	// swap rgb to bgr
	c = 18+WIDTH*HEIGHT*3;
	for (i=18 ; i<c ; i+=3)
	{
		temp = buffer[i];
		buffer[i] = buffer[i+2];
		buffer[i+2] = temp;
	}
//	COM_WriteFile (pcxname, buffer, glwidth*glheight*3 + 18 );
	fileptr = fopen(pcxname, "w");
	fwrite(buffer, WIDTH*HEIGHT*3 + 18, 1,fileptr);

	free (buffer);
//	Con_Printf ("Wrote %s\n", pcxname);
} 
