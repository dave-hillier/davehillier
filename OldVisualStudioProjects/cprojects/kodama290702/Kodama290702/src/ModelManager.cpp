// ModelManager.cpp: implementation of the ModelManager class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "ModelManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ModelManager *mman;

ModelManager::ModelManager()
{
	num_models = 0;
}

ModelManager::~ModelManager()
{
	// TODO delete all models
}

int ModelManager::Load (char *name)
{ 
	int len = strlen(name);

	for (int i = 0; i < num_models; i++)
	{
		if (stricmp(name, models[i]->name) == 0)
			return i;
	}

	if (stricmp(name+len-3, "ase") == 0)
	{
		models[num_models] = new ASEWrapper();
		models[num_models]->Load(name);
		strcpy(models[num_models]->name, name);

		return num_models++;
	}
	else if (stricmp(name+len-3, "md2") == 0)
	{
		models[num_models] = new MD2Wrapper();
		models[num_models]->Load(name);
		strcpy(models[num_models]->name, name);

		return num_models++;

	}
	else if (stricmp(name+len-4, "ms3d") == 0)
	{
		models[num_models] = new MS3DWrapper();
		models[num_models]->Load(name);
		strcpy(models[num_models]->name, name);

		return num_models++;
	}

	return -1;

}

void ModelManager::Render (int mdl)
{
	models[mdl]->Render();
}
