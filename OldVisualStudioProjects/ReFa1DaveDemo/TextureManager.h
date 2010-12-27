// TextureManager.h: interface for the TextureManager class.
//
//////////////////////////////////////////////////////////////////////
#include <vector>


struct Texture {
	std::string name;
	unsigned int textId;
} ;

class TextureManager  
{
public:
	TextureManager();
	virtual ~TextureManager();
	int LoadTexture(const std::string &name);
	
	void Bind(const std::string &name);			
	void Bind(int id);			// do also by number
	void KillTexture(const std::string &name);	// do also by number

	int GetId(const std::string &name);

	void LoadTGA (const std::string &name, byte **pic, int *width, int *height); // required fot terrain
	// TODO make interface the consistent
	bool LoadJPG (char* szFileName, bool asAlpha, int *m_nHeight, int *m_nWidth);

	// should also have a reload procedure
private:

	std::vector<Texture> textures;
};

extern TextureManager texture;