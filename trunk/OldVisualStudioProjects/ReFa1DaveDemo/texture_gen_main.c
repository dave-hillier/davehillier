#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef unsigned char	byte;
typedef enum { false, true } bool;

// quake style typedefs
typedef float vec_t;
typedef vec_t vec2_t[2];  /* st */
typedef vec_t vec3_t[3];  /* xyz */
typedef vec_t vec4_t[4];  /* xyzw */
typedef float mat3_t[9];  /* 3x3 matrix */
typedef float mat4_t[16]; /* 4x4 matrix */
typedef int	bbox_t[6];        /* Integer bounding box (mins, maxs)*/
typedef float bboxf_t[6];     /* Float bounding box */

#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])

vec3_t *calc_normals (byte *data, int width, int height);
bool load_targa(const char *fname, byte **rgb, int *w, int *h, int *format);
void save_targa (char *filename, byte *data, int width, int height, int bpp);

void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

int main (int argc, char *argv[])
{
	char file_name[128], mapname[128];
	int x, z, i, bpp;
	byte *img, *lightmap, *gradient_map, *material_map;
	byte *red_chan, *green_chan, *blue_chan, *alpha_chan, *outmap;

	vec3_t *n;	

	if (argc < 2)
	{
		printf("enter map dir: ");
		scanf("%s", mapname);
		sprintf(file_name, "maps/%s/elev.tga", mapname);
	}
	else
	{
		strcpy(mapname, argv[1]);
		sprintf(file_name, "maps/%s/elev.tga", argv[1]);
	}


	if (!load_targa(file_name, &img, &x, &z, &i))
	{
		printf("cant open %s\n", file_name);
		return 1;
	}
	bpp = i;

	red_chan = (byte *)malloc(x*z);
	green_chan = (byte *)malloc(x*z);
	blue_chan = (byte *)malloc(x*z);
	alpha_chan = (byte *)malloc(x*z);
	// map is in red channel, other channels can be used for stuff!
	for (i=0; i < x*z*bpp; i += bpp) 
	{
		red_chan[i/bpp] = img[x*z*bpp - (i + 0)];
		green_chan[i/bpp] = img[x*z*bpp - (i + 1)];
		blue_chan[i/bpp] = img[x*z*bpp - (i + 2)];
		alpha_chan[i/bpp] = img[x*z*bpp - (i + 3)];
	}

	n = calc_normals(red_chan, x, z);

	lightmap = (byte *)malloc (x*z);
	gradient_map = (byte *)malloc (x*z);
	material_map = (byte *)malloc (x*z);

	outmap = (byte *)malloc(x*z*4);

	for (i = 0; i < x*z; i++)
	{
		vec3_t sun = { 0.50f, 0.00f, 0.50f };
		vec3_t up = { 0.0f, 1.0f, 0.0f };
		float gradient = (float)fabs(DotProduct (n[i], up));
		float ang = (DotProduct (n[i], sun) + 1.0f)/2.0f;// diffuse + ambient (no specular)
			 
		lightmap[i] = (byte)(ang * 255);
		gradient_map[i] = (byte)((1.0-gradient) * 255);

		material_map[i] = 64;


		outmap[i*3 + 0] = 0;  
		outmap[i*3 + 1] = 0;  
		outmap[i*3 + 2] = lightmap[i];  

	/*	if (gradient_map[i] < 100)
		{
		//	outmap[i*3 + 0] = 0;  
			outmap[i*3 + 1] = 250;  
		//	outmap[i*3 + 2] = 0;  
		}
		else if (gradient_map[i] < 200)
		{
		//	outmap[i*3 + 0] = 0;  
			outmap[i*3 + 1] = 200;  
		//	outmap[i*3 + 2] = 0;  
		}*/
		outmap[i*3 + 1] = gradient_map[i];

		if (red_chan[i] > 250)		// top
		{
			outmap[i*3 + 0] = 250;  
		//	outmap[i*3 + 1] = 0;  
		//	outmap[i*3 + 2] = 0;  
		}
		else if (red_chan[i] > 210)		// v high
		{
			outmap[i*3 + 0] = 210;  
		//	outmap[i*3 + 1] = 210;  
		//	outmap[i*3 + 2] = 20;  
		}
		else if (red_chan[i] > 128)	// high
		{
			outmap[i*3 + 0] = 128;;  
		//	outmap[i*3 + 1] = 0;  
		//	outmap[i*3 + 2] = 0;  
		}
		else if (red_chan[i] > 32)			// grass
		{
			outmap[i*3 + 0] = 96;  
		//	outmap[i*3 + 1] = 0;  
		//	outmap[i*3 + 2] = 0;  
		}		
		else if (red_chan[i] > 0)			// beach?
		{
			outmap[i*3 + 0] = 64;  
		//	outmap[i*3 + 1] = 235;  
		//	outmap[i*3 + 2] = 10; 
			
		}		
		else 
		{
			outmap[i*3 + 0] = 0;  
		//	outmap[i*3 + 1] = 0;  
		//	outmap[i*3 + 2] = 0;  
		}
		

	}
//	sprintf(file_name, "test.tga");
	sprintf(file_name, "maps/%s/texture_help.tga", mapname);

	printf("Light-map created %s\n", file_name);
	save_targa(file_name, outmap, x, z, 24);
	free(outmap);

	// free stuff
	return 0;
}


vec3_t *calc_normals (byte *data, int width, int height)
{
	int k, ox, oz, x, z;
	float a, b, c;
	float ab[3], ac[3], n[3], div;

	vec3_t *normals = (vec3_t *)malloc(sizeof(vec3_t)*width*height);

	if (!normals)
		return NULL;

	for (x=0; x<width; x++) {
		for (z=0; z<height; z++) {
			if (x>0)
				ox=-1;
			else 
				ox=1;
			if (z>0) 
				oz=-width;
			else 
				oz=width;

			k = x+z*width;
			a = data[k];
			b = data[k+ox];
			c = data[k+oz];
			
			ab[0] = 1.0f;
			ab[1] = a - b;
			ab[2] = 0.0f;

			ac[0] = 0.0f;
			ac[1] = a - c;
			ac[2] = 1.0f;

			CrossProduct(ac, ab, n);
			div = (float )sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
			normals[x+z*width][0] = n[0] / div;
			normals[x+z*width][1] = n[1] / div;
			normals[x+z*width][2] = n[2] / div;
		}
	}

	return normals;
}



int flength (FILE *f)
{
	int             pos;
	int             end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

typedef struct
{
    byte idlen;
    byte cmtype;
    byte imgtype;
    byte cmspec[5];
    unsigned short xorig, yorig;
    unsigned short width, height;
    byte pixsize;
    byte imgdesc;
} tgaheader_t;


bool load_targa(const char *fname, byte **rgb, int *w, int *h, int *format)
{
    tgaheader_t *tgahead;
    byte *img, *tga, *tgacur, *tgaend;
    int tgalen, len, depth = 0;
	FILE    *f;
	
	f = fopen(fname, "rb");
	if (!f)
	{
		fprintf(stderr, "Error opening file %s\n", fname);
		return 0;
	}

	tgalen = flength(f);
	tga = (byte *)malloc(tgalen);
	fread(tga, tgalen, 1, f);	
	fclose(f);

    if (!tgalen) 
	{
		fprintf(stderr, "Error opening file %s\n", fname);
		return 0;
	}
	
    tgaend = tga + tgalen;
    
    tgahead = (tgaheader_t*)tga;

    if (tgahead->imgtype != 2 && tgahead->imgtype != 10)
		fprintf(stderr, "Bad tga image type\n");
	
    if (tgahead->pixsize == 24)
		depth = 3;
    else if (tgahead->pixsize == 32)
		depth = 4;
    else
		fprintf(stderr, "Non 24 or 32 bit tga image\n");
    
    len = tgahead->width * tgahead->height * depth;
    img = (byte *) malloc(len);
	
    tgacur = tga + sizeof(tgaheader_t) + tgahead->idlen;
	// run-length encoded
    if (tgahead->imgtype == 10)
    {
		int i, j, packetlen;
		byte packethead;
		byte *c = img, *end = img + len;
		byte rlc[4];
		
		while (c < end)
		{
			packethead = *tgacur;
			if (++tgacur > tgaend)
				fprintf(stderr, "Unexpected end of tga file\n");
			if (packethead & 0x80)
			{
				/* Run-length packet */
				packetlen = (packethead & 0x7f) + 1;
				memcpy(rlc, tgacur, depth);
				if ((tgacur += depth) > tgaend)
					fprintf(stderr, "Unexpected end of tga file\n");
				for (j=0; j < packetlen; ++j)
					for(i=0; i < depth; ++i)
						*c++ = rlc[i];
			}
			else
			{
				/* Raw data packet */
				packetlen = packethead + 1;
				memcpy(c, tgacur, depth * packetlen);
				if ((tgacur += depth * packetlen) > tgaend)
					fprintf(stderr, "Unexpected end of tga file\n");
				c += packetlen * depth;
			}
		}
		
		/* Flip image in y */
		{
			int i, linelen;
			byte *temp;
			
			linelen = tgahead->width * depth;
			temp = (byte *) malloc(linelen);
			for (i=0; i < tgahead->height/2; ++i)
			{
				memcpy(temp, &img[i * linelen], linelen);
				memcpy(&img[i * linelen], &img[(tgahead->height - i - 1)
					* linelen], linelen);
				memcpy(&img[(tgahead->height - i - 1) * linelen], temp,
					linelen);
			}
			free(temp);
		}	
    }
    else
    {
		int i, linelen;
		
		if (tgaend - tgacur + 1 < len)
			fprintf(stderr, "Bad tga image data length\n");
		
		/* Flip image in y */
		linelen = tgahead->width * depth;
		for (i=0; i < tgahead->height; ++i)
			memcpy(&img[i * linelen],
			&tgacur[(tgahead->height - i - 1) * linelen], linelen);
    }    
	
    /* Exchange B and R to get RGBA ordering */
    {
		int i;
		byte temp;
		
		for (i=0; i < len; i += depth)
		{
			temp = img[i];
			img[i] = img[i+2];
			img[i+2] = temp;
		}
    }
    
    *rgb = img;
    *w = tgahead->width;
    *h = tgahead->height;
    *format = depth;
	
	free(tga); 
    return 1;
}
void save_targa (char *filename, byte *data, int width, int height, int bpp)
{
	FILE *fp;
	byte *buffer;
	int i,tmp, buffersize, colordepth;
	tgaheader_t *hdr;


	if (bpp == 24)
		colordepth = 3;
	else if (bpp == 32)
		colordepth = 4;
	else if (bpp == 8)
		colordepth = 1;

	buffersize = width * height * colordepth + sizeof(tgaheader_t);

	buffer = (byte *)malloc(buffersize);
	memset(buffer, 0, buffersize);
	hdr = (tgaheader_t *)buffer;

	hdr->width = width;
	hdr->height = height;
	hdr->pixsize = bpp;//24;
	hdr->imgtype = 2; //alway uncompressed
	hdr->idlen = 0;//sizeof(tgaheader_t) + width * height * colordepth;
	// what does it do??
	
	// copy data into buffer
	memcpy(buffer+sizeof(tgaheader_t), data, width * height * colordepth);

	if ((bpp == 24) || (bpp == 32))
	{
		for (i = sizeof(tgaheader_t) + hdr->idlen; i < buffersize; i += colordepth)
		{
			tmp = buffer[i];
			buffer[i] = buffer[i+2];
			buffer[i+2] = tmp;
		}
	}
	if (fp = fopen(filename, "wb"))
	{
		fwrite(buffer, buffersize, 1, fp);
		fclose(fp);
	}
	free(buffer);
}