#include "project003.h"

#include <gl/gl.h>
#include <gl/glu.h>

void ScreenShot (char *filename) 
{
	unsigned char	*buffer;
	FILE			*fileptr;

	int			i, c, temp;

	buffer = malloc(win.width*win.height*3 + 18);
	memset (buffer, 0, 18);
	buffer[2] = 2;		// uncompressed type
	buffer[12] = win.width&255; 
	buffer[13] = win.width>>8;
	buffer[14] = win.height&255;
	buffer[15] = win.height>>8;
	buffer[16] = 24;	// pixel size

	glReadPixels (0, 0, win.width, win.height, GL_RGB, GL_UNSIGNED_BYTE, buffer+18 ); 

	// swap rgb to bgr
	c = 18+win.width*win.height*3;
	for (i=18 ; i<c ; i+=3)
	{
		temp = buffer[i];
		buffer[i] = buffer[i+2];
		buffer[i+2] = temp;
	}
//	COM_WriteFile (pcxname, buffer, win.width*winheight*3 + 18 );
	if (fileptr = fopen(filename, "w"))
	{
		fwrite(buffer, win.width*win.height*3 + 18, 1, fileptr);
	}

	free (buffer);

} 
