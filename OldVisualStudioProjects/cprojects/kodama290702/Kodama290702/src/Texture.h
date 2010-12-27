#ifndef __TEXTURE_H__
#define __TEXTURE_H__

typedef unsigned char byte;
#define MAX_TEXTURES 512
class Texture{
public:
	char name[128]; // TODO dynamic alocation
	unsigned int	texID;
	int key; // TODO hashing
};

class TextureManager  
{
public:
	TextureManager();
	virtual ~TextureManager();
	int LoadTexture(char *name);
	
	void Bind(char *name);			
	void Bind(int id);			// do also by number
	void KillTexture(char *name);	// do also by number

	int GetId(char *name);

	void LoadTGA (char *name, byte **pic, int *width, int *height); // required fot terrain

	void LoadJPG (char *name, byte **pic, int *width, int *height);


	// should also have a reload procedure
private:

	Texture textures[MAX_TEXTURES];
	int numtextures;
};

extern TextureManager texture;

#endif