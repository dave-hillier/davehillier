#include "omega_common.h"
#include "omega_gl.h"
/*

  Whats an texture_info?
  Its a text file that contains itexture_info about models and skins, 
  and gl itexture_info
  its loaded at the start, pre-caching textures

  what about layering?

  what about texture coordinate changing for 
  envmaps, moving textures, etc?

	
*/

// for parsing texture_infos
typedef struct {
	int value;
	char name[64];
} gl_constant_t;

gl_constant_t constants[] = {
	// blend funcs
	{ GL_ONE,					"GL_ONE" },
	{ GL_ZERO,					"GL_ZERO" },
	{ GL_DST_COLOR,				"GL_DST_COLOR" },
	{ GL_ONE_MINUS_DST_COLOR,	"GL_ONE_MINUS_DST_COLOR" },
	{ GL_SRC_ALPHA,				"GL_SRC_ALPHA" },
	{ GL_SRC_COLOR,				"GL_SRC_COLOR" },
	{ GL_ONE_MINUS_SRC_COLOR,	"GL_ONE_MINUS_SRC_COLOR" },
	// alpha funcs
	{ GL_NEVER,					"GL_NEVER" },
	{ GL_ALWAYS,				"GL_ALWAYS" },
	{ GL_LESS,					"GL_LESS" },
	{ GL_LEQUAL,				"GL_LEQUAL" },
	{ GL_EQUAL,					"GL_EQUAL" },
	{ GL_GEQUAL,				"GL_GEQUAL" },
	{ GL_GREATER,				"GL_GREATER" },
	{ GL_NOTEQUAL,				"GL_NOTEQUAL" },
};

typedef enum {
	BLEND = 1,
	DEPTHMASK = 2,
	NOCULL = 4,
	NOLIGHT = 8,
	ALPHATEST = 16,
	NOMIPMAP = 32,
} texture_info_modes;

typedef struct {
	texture_info_modes	modes;
	int			texid;
	char		texture[64];
	int			alphafunc;
	float		alphaval;
	int			blendsrc;
	int			blenddst;
	float		rgba[4];// rgba
	// tex coord mod's?
	// layers?
} texture_info;

int GetConstantFromName (char *name)
{
	int i;
	for (i = 0; i < ( sizeof (constants) / sizeof (gl_constant_t) ); i++)
	{
		if (stricmp (name, constants[i].name) == 0)
			return constants[i].value;
	}
	
	return 0;

}
// Gets itexture_info for a single texture, between { and }
texture_info *GetTextureInfo (FILE *f, char *name, int start, int end)
{
	char data[255];
	texture_info *ptr = (texture_info *)malloc (sizeof(texture_info));

	fseek (f, start, SEEK_SET);

	ptr->modes = 0;
	while (ftell(f) < end) 
	{
		fscanf(f, "%s", data);

		if (strcmp ("texture", data) == 0)
			fscanf(f, "%s", ptr->texture);
		else if (strcmp ("nocull", data) == 0)
			ptr->modes |= NOCULL;
		else if (strcmp ("nomipmap", data) == 0)
			ptr->modes |= NOMIPMAP;
		else if (strcmp ("nolight", data) == 0)
			ptr->modes |= NOLIGHT;
		else if (strcmp ("depthmask", data) == 0)
			ptr->modes |= DEPTHMASK;
		else if (strcmp ("alphatest", data) == 0)
		{
			fscanf(f, "%s %f", data, ptr->alphaval);
			ptr->alphafunc = GetConstantFromName (data);
			ptr->modes |= ALPHATEST;
		}
		else if (strcmp ("blend", data) == 0)
		{
			char src[64], dst[64];
			fscanf(f, "%s %s", src, dst);
			ptr->blendsrc = GetConstantFromName (dst);
			ptr->blendsrc = GetConstantFromName (dst);
			ptr->modes |= BLEND;
		}
	}
	return ptr;
}

void ChangeModes (texture_info *ptr)
{
	//static
	// Disabling, saving and only enabling the ones needed??? 
	// hmm its probably quicker not to bother checking

	if (ptr->modes & NOCULL)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

	if (ptr->modes & NOLIGHT)
		glDisable(GL_LIGHTING);
	else
		glEnable(GL_LIGHTING);

	if (ptr->modes & DEPTHMASK)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);

	if (ptr->modes & BLEND)
	{
		glEnable(GL_BLEND);
		glBlendFunc(ptr->blendsrc, ptr->blenddst);
	}

	if (ptr->modes & ALPHATEST)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(ptr->alphafunc, ptr->alphaval);
	}

	if (!ptr->texid)
		ptr->texid = LoadTexture(ptr->texture, !(ptr->modes & NOMIPMAP));
		
	glBindTexture(GL_TEXTURE_2D, ptr->texid);
}