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

	void LoadTGA (char *name, byte **pic, int *width, int *height); // required fot terrain
	// TODO make interface the consistent
	bool LoadJPG (char* szFileName, bool asAlpha, int *m_nHeight, int *m_nWidth);

	// should also have a reload procedure
private:

	Texture textures[512];
	int numtextures;
};

extern TextureManager texture;