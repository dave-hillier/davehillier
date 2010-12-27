#include "omega_common.h"
#include "omega_gl.h"

void LoadTGA (FILE *fin);
void Load8BitTGA (FILE *fin);

texture_t textures[MAX_TEXTURES];
int		numtextures = 0;
int		texture_extension_number = 1;
int		particle_texture;
extern int		font_texture;

extern TargaHeader	targa_header;
extern byte			*targa_rgba;

void GetTexture (char *name, texture_t *tex)
{
	FILE *f;

	if (f = fopen(name, "rb"))
	{
		tex->texid = texture_extension_number;
		glBindTexture(GL_TEXTURE_2D, texture_extension_number++);

		LoadTGA(f);

		tex->height = targa_header.height;
		tex->width = targa_header.width;
		tex->mipmap = false;
		strcpy (tex->name, name);

		glTexImage2D(GL_TEXTURE_2D, 
			0, 3, 
			targa_header.width, 
			targa_header.height, 
			0, GL_RGBA,
			GL_UNSIGNED_BYTE, targa_rgba);
	  

		free (targa_rgba);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
			GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	}
}

void GetColourMap (char *name, texture_t *tex)
{
	FILE *f;

	if (f = fopen(name, "rb"))
	{
		tex->texid = texture_extension_number;
		glBindTexture(GL_TEXTURE_2D, texture_extension_number++);

		Load8BitTGA(f);

		tex->height = targa_header.height;
		tex->width = targa_header.width;
		tex->mipmap = false;
		strcpy (tex->name, name);

		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 
				0, GL_ALPHA, 
				targa_header.width, 
				targa_header.height, 
				0, GL_ALPHA,
				GL_UNSIGNED_BYTE, targa_rgba);
		free (targa_rgba);

	}
}

void TextureInit ()
{
	GetTexture("star.tga", &textures[numtextures++]);
	GetTexture("water.tga", &textures[numtextures++]);
	particle_texture = textures[numtextures-1].texid;

	GetColourMap("font.tga", &textures[numtextures++]);
	font_texture = textures[numtextures-1].texid;

}

