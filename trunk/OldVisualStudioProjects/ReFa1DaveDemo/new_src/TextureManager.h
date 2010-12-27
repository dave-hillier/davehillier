#pragma once

// Required by header

// required internal
#include "Targa.h"
#include "Jpeg.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <cassert>
#include <string>

typedef unsigned int TextureId;

class TextureManager
{
public:
	TextureManager() : defaultId(0)
	{
		const unsigned char img[] = {
			255, 255, 255, 255,
			255,   0,   0, 255,
			255,   0,   0, 255,
			255, 255, 255, 255
		};
		int width = 2;
		int height = 2;
		int bpp = 4;

		TextureInternal internal("default", defaultId, (char*)img, bpp, width, height );
		names.push_back(internal);
	}

	~TextureManager();

	static TextureManager &Instance() 
	{
		static TextureManager myInstance;
		return myInstance;
	}

	bool Load(const std::string &fileName, TextureId &id)
	{
		bool result = false;
		std::string extension(fileName.substr(fileName.size()-3, fileName.size()));

		id = defaultId;

		if (extension == "jpg") 
		{
			Jpeg jpg;
			result = Jpeg::Load(fileName, jpg, false);
			assert(result);
			if (result)
			{
				TextureId nextId;
				TextureInternal internal(fileName, nextId, jpg.data(), jpg.bytesPerPixel(), jpg.width(), jpg.height() );
				names.push_back(internal);
				id = nextId;
			}
		}
		else
		{
			std::ifstream fileStream(fileName.c_str(), std::ios::binary|std::ios::in);
			if (fileStream.is_open())
			{
				if (extension == "tga")
				{
					Targa tga;
					fileStream >> tga;
					result = true;//fileStream.good();

					assert(result);
					if (result)
					{
						TextureId nextId;
						TextureInternal internal(fileName, nextId, tga.data(), tga.bytesPerPixel(), tga.width(), tga.height() );
						names.push_back(internal);
						id = nextId;
					}
				} 
				fileStream.close();
			}
		}
		return result;
	}

	bool Delete(const std::string &fileName)
	{
		bool result = false;

		for (std::vector<TextureInternal>::iterator itt = names.begin(); itt != names.end(); ++itt )
		{
			if ((*itt).Name() == fileName)
			{
				names.erase(itt);
				result = true;
				break;
			}
		}

		return result;
	}

	bool GetId(const std::string &name, TextureId &id) const
	{
		bool result = false;

		for (std::vector<TextureInternal>::const_iterator itt = names.begin(); itt != names.end(); ++itt )
		{
			if ((*itt).Name() == name)
			{
				result = true;
				id = (*itt).Id();
				break;
			}
		}

		return result;
	}

	void Apply(TextureId id) const
	{
		glBindTexture (GL_TEXTURE_2D, id);
	}

	void Apply(const std::string &name) const
	{
		TextureId id;
		if (GetId(name, id))
		{
			Apply(id);
		}
	}

private:

	class TextureInternal
	{
	public:
		TextureInternal(const std::string &name, TextureId &tid, const char* pixels, 
			unsigned char bytesPerPixel, unsigned width, unsigned height ) : 
			name(name), textureId(tid) 
		{ 
			assert(bytesPerPixel == 3 || bytesPerPixel == 4);
			unsigned int format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB; 
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

			// allocate a texture name
			glGenTextures( 1, &textureId );

			// select our current texture
			glBindTexture( GL_TEXTURE_2D, textureId );

			// Generate the mip maps
			gluBuild2DMipmaps(GL_TEXTURE_2D, bytesPerPixel, width, height, format, GL_UNSIGNED_BYTE, pixels);
			tid = textureId;
		}
		~TextureInternal()
		{
			//glDeleteTextures(1, &textureId);
		}

		std::string Name() const { return name; } 
		TextureId Id() const { return textureId; } 
	private:
		std::string name;
		TextureId textureId;
	};
	TextureId defaultId;
	std::vector<TextureInternal> names;

};
