#include "omega_common.h"
#include "omega_gl.h"
// TODO : implement type structure NFO FILE!!

texture_t textures[MAX_TEXTURES];
int		numtextures = 0;
int		texture_extension_number = 1;
int		particle_texture;
extern int		font_texture;
extern TargaHeader	targa_header;
extern byte			*targa_rgba;
// what about mip maps
void GetTexture (char *name, texture_t *tex)
{
	FILE *f;
	int format, bpp;

	if (f = fopen(name, "rb"))
	{
		tex->texid = texture_extension_number;
		glBindTexture(GL_TEXTURE_2D, texture_extension_number++);

		format = LoadTGA(f);

		tex->height = targa_header.height;
		tex->width = targa_header.width;
		strcpy (tex->name, name);

		glPixelStorei (GL_UNPACK_ALIGNMENT, 1); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_DECAL); modulate for shading
		if (format == 1) {
			bpp = GL_ALPHA;
			format = GL_ALPHA;
		}
		else if (format == 3)
			bpp = GL_RGBA;
		else if (format == 4)
			bpp = GL_RGBA;

		if (tex->mipmap)
 			gluBuild2DMipmaps(GL_TEXTURE_2D,
				format,//GL_RGB, 
				targa_header.width, 
				targa_header.height, 
				bpp, //GL_RGBA,
				GL_UNSIGNED_BYTE, targa_rgba);
		else
			glTexImage2D(GL_TEXTURE_2D, 
				0, format,//GL_RGB, 
				targa_header.width, 
				targa_header.height, 
				0, bpp, //GL_RGBA,
				GL_UNSIGNED_BYTE, targa_rgba);  

		free (targa_rgba);// Dont need it once its uploaded
	}
	else
		ConPrintf("GetTexture: Cant open %s\n", name);

}

int LoadTexture (char *name, bool mipmap)
{
	int i;
	// check to see if its loaded already
	for (i = 0; i < numtextures; i++)
	{
		if (stricmp(name, textures[i].name) == 0)
			return textures[i].texid;
	}


	textures[numtextures].mipmap = mipmap;

	GetTexture(name, &textures[numtextures++]);
	return textures[numtextures-1].texid;
}

void TextureInit ()
{
	glEnable(GL_TEXTURE_2D);
	ConPrintf("Precaching: textures\n");

	font_texture = LoadTexture("font.tga", false);

	LoadTexture("torch.tga", true);
	particle_texture = LoadTexture("star.tga", true);	
}

int UploadTexture (byte *array, int width, int height)
{
	texture_t *tex = &textures[numtextures];
	tex->height = height;
	tex->width = width;
	tex->mipmap = true;
	strcpy (tex->name, "Internal_Reserved");
	tex->texid = texture_extension_number;

	glBindTexture(GL_TEXTURE_2D, texture_extension_number);

	glPixelStorei (GL_UNPACK_ALIGNMENT, 1); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_DECAL); modulate for shading

 	gluBuild2DMipmaps(GL_TEXTURE_2D,
		4,//GL_RGB, 
		width, 
		height, 
		GL_RGBA, //GL_RGBA,
		GL_UNSIGNED_BYTE, array);

	numtextures++;
	return texture_extension_number++;
}