#pragma once

#include "Service.h"

#include <vector>
class dWorld;
class dSpace;

class dPlane;
class dBody;
class dJointGroup;

struct dxWorld;		/* dynamics world */
struct dxSpace;		/* collision space */
struct dxBody;		/* rigid body (dynamics object) */
struct dxGeom;		/* geometry (collision object) */
struct dxJoint;
struct dxJointNode;
struct dxJointGroup;

typedef struct dxWorld *dWorldID;
typedef struct dxSpace *dSpaceID;
typedef struct dxBody *dBodyID;
typedef struct dxGeom *dGeomID;
typedef struct dxJoint *dJointID;
typedef struct dxJointGroup *dJointGroupID;



class _vector3;
typedef _vector3 vector3;

class quaternion;



class SimulatedObject
{
public:
	SimulatedObject(){}
	virtual ~SimulatedObject(){}

	virtual void position(const vector3 &pos) = 0;
	virtual vector3 position() const = 0;

	virtual void velocity(const vector3 &v) = 0;
	virtual vector3 velocity() const = 0;

	virtual quaternion orientation() const = 0;
	virtual void orientation(const quaternion &q) = 0;

	virtual void applyForce(const vector3 &f) = 0;
};

class CollisionInterface
{
public:
	CollisionInterface() {}
	virtual ~CollisionInterface(){}

	virtual void collide(CollisionInterface *) = 0;
	virtual void collide() = 0; 

};



#include "maths/vector.h"
#include "maths/quaternion.h"
class DynamicObjectBase : public CollisionInterface
{
public:
	DynamicObjectBase(SimulatedObject *obj=NULL) : obj_(obj) 
	{

	}
	virtual ~DynamicObjectBase()
	{
		delete obj_;
	}

	virtual void position(const vector3 &pos)
	{
		obj_->position(pos);
	}
	virtual vector3 position() const
	{
		return obj_->position();
	}

	virtual void velocity(const vector3 &v)
	{
		obj_->velocity(v);
	}

	virtual vector3 velocity() const
	{
		return obj_->velocity();
	}

	virtual quaternion orientation() const
	{
		return obj_->orientation();
	}

	virtual void orientation(const quaternion &q)
	{
		obj_->orientation(q);
	}
	virtual void applyForce(const vector3 &f)
	{
		obj_->applyForce(f);
	}

	virtual void update(double time) = 0;

	void set(SimulatedObject *o) { obj_=o;}
private:
	SimulatedObject *obj_;

};

class SimulationManager/* : public Service*/
{
public:
	SimulationManager();
	virtual ~SimulationManager();

	bool initialise(){return true;}

	SimulatedObject* createSphere(const vector3 &position, float radius, float mass, DynamicObjectBase* controller);	
	SimulatedObject* createBox(const vector3 &position, const vector3& dimensions, float mass, DynamicObjectBase* controller);	

	void nearCollide(dGeomID o1, dGeomID o2);
	virtual void update(double time);

private:

	dWorld *world_;
	dSpace *space_;
	dJointGroup *contactGroup_;
	double lastUpdate_;
	double nextUpdate_;

	// Test data
	dPlane *groundPlane_;

	std::vector<DynamicObjectBase*> updateThings_;

};
