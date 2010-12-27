// TextureManager.h: interface for the TextureManager class.
//
//////////////////////////////////////////////////////////////////////

typedef struct {
	char name[128];
	unsigned int	texID;
} Texture;

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

	// should also have a reload procedure
private:
	void LoadTGA (char *name, byte **pic, int *width, int *height);

	Texture textures[512];
	int numtextures;
};

extern TextureManager texture;