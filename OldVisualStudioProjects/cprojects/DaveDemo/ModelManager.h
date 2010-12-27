// ModelManager.h: interface for the ModelManager class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __MODELMANAGER__
#define __MODELMANAGER__

#include "md2_model.h"
#include "ms3d_model.h"
#include "ase_model.h"

class ModelWrapper
{

public:
	ModelWrapper(){};
	virtual ~ModelWrapper(){};
	virtual void Render(){};
	virtual void Load(char *name){};
//	int texture;
	char name[128];
private:
};

class MD2Wrapper : public ModelWrapper
{
public:
	MD2Wrapper() {};
	virtual ~MD2Wrapper(){};
	void Render() { model.DrawFrameInt(frame, next, intepolate); };
	void Load(char *name) { model.Load(name); };
	int frame;
	int next;
	float intepolate;
private:
	md2_model model;
};

class ASEWrapper : public ModelWrapper
{
public:
	ASEWrapper(){};
	virtual ~ASEWrapper(){};
	void Render() { model.Render(); };
	void Load(char *name) { model.Load(name); };

private:
	ase_model model;
};

class MS3DWrapper : public ModelWrapper
{
public:
	MS3DWrapper(){};
	virtual ~MS3DWrapper(){};
	void Load(char *name) { model.Load(name); };
	void Render () { model.Render(); };

private:
	ms3d_model model;
};


#define MAX_MODELS 128 // thats 128 different models

class ModelManager  
{
public:
	ModelManager();
	virtual ~ModelManager();

	int Load (char *name);	
	void Render (int mdl);	


private:
	ModelWrapper *models[MAX_MODELS];
	int num_models;

};

extern ModelManager mman;

#endif // __MODELMANAGER__