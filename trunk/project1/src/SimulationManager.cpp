#include "SimulationManager.h"

#include "maths/vector.h"
#include "maths/quaternion.h"

#include <ode/ode.h>
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>
#include <cassert>
#include <iostream>


inline std::ostream& operator<< (std::ostream& os, const vector3 &v )
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")"; 
	return os;
}	

namespace
{
	class InternalSimBase : public SimulatedObject
	{
	public:
		InternalSimBase(dWorld *w, const vector3 &position, CollisionInterface *controller) : 
			body_(new dBody(w->id())), controller_(controller)
		{
			body_->setPosition(position.x, position.y, position.z);
			body_->setData(controller_);

			quaternion qa(0,1,0,0);
			orientation(qa);

		}
			
		virtual ~InternalSimBase() 
		{
			delete body_;
		}
		virtual void position(const vector3 &pos)
		{
			body_->setPosition(pos.x, pos.y, pos.z);
		}

		virtual vector3 position() const
		{
			const dReal *v = body_->getPosition();
			return vector3(v[0], v[1], v[2]);
		}

		virtual void velocity(const vector3 &v)
		{
			body_->setLinearVel(v.x, v.y, v.z);
		}

		virtual vector3 velocity() const
		{
			const dReal *v = body_->getLinearVel();
			return vector3(v[0], v[1], v[2]);
		}

		virtual quaternion orientation() const
		{
			const dReal *q = body_->getQuaternion();
			return quaternion(q[3], q[0], q[1], q[2]);
		}

		virtual void orientation(const quaternion &q)  
		{
			dReal qR[4] = { q.w, q.x, q.y, q.z }; 
			body_->setQuaternion(qR);
		}

		virtual void applyForce(const vector3 &f)
		{
			body_->addForce(f.x, f.y, f.z);
		}


	protected:
		dBody *body_;
		CollisionInterface *controller_;
	};
	class TestSphere : public InternalSimBase
	{
	public:
		TestSphere(dWorld *w, dSpace *s, const vector3 &position, float radius, float mass, 
			CollisionInterface *controller) :
			InternalSimBase(w, position, controller), geom_(new dSphere(s->id(), radius))
		{
			dMass m;
			m.setSphere(mass, radius);
			m.mass = 1;
			body_->setMass(&m);

			geom_->setBody(body_->id());
		}
		virtual ~TestSphere()
		{
			delete geom_;
		}

	private:
		dSphere *geom_;
	};

	class TestBox : public InternalSimBase
	{
	public:
		TestBox(dWorld *w, dSpace *s, const vector3 &position, const vector3 & dimensions, float mass, 
			CollisionInterface *controller) :
			InternalSimBase(w, position, controller),  
				geom_(new dBox(s->id(), dimensions.x, dimensions.y, dimensions.z))
		{
			dMass m;
			m.setBox(mass, dimensions.x, dimensions.y, dimensions.z);
			body_->setMass(&m);

			geom_->setBody(body_->id());
		}
		virtual ~TestBox()
		{
			delete geom_;
		}

	private:
		dBox *geom_;
	};
}

namespace {

	void callBack(void *data, dGeomID o1, dGeomID o2)
	{
		static_cast<SimulationManager*>(data)->nearCollide(o1, o2);
	}

}

SimulationManager::SimulationManager() : world_(new dWorld()), space_(new dHashSpace(0)),
	contactGroup_(new dJointGroup(0)), lastUpdate_(0), nextUpdate_(0)
{
	world_->setGravity (0,-9.81f, 0);
	world_->setCFM (1e-5f);

	dWorldSetContactMaxCorrectingVel(world_->id(),0.1f);
	dWorldSetContactSurfaceLayer(world_->id(),0.001f);

	groundPlane_ = new dPlane(space_->id(), 0,1,0,-5);
}

SimulationManager::~SimulationManager()
{
	delete groundPlane_;
	delete space_;
	delete world_;
}

void SimulationManager::update(double time)
{
	space_->collide(this, &callBack);
	world_->step((dReal)(time - lastUpdate_));	// not fixed, but close
	contactGroup_->empty();

	for(std::vector<DynamicObjectBase*>::iterator itt = updateThings_.begin(); itt != updateThings_.end(); ++itt)
	{
		(*itt)->update(time);
	}
	lastUpdate_ = time;
}


// pretty much a cut and paste from the example
void SimulationManager::nearCollide(dGeomID o1, dGeomID o2)
{
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;

	const int maxContacts = 10;
	dContact contact[maxContacts];   // up to numContacts contacts per box-box
	for (int i=0; i<maxContacts; i++) 
	{
		contact[i].surface.mode = dContactBounce /*| dContactSoftCFM*/;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.mu2 = 0;
		contact[i].surface.bounce = 0.5f;
		contact[i].surface.bounce_vel = 0.5f;
		contact[i].surface.soft_cfm = 0.01f;
	}
	int numContacts = dCollide (o1,o2,maxContacts,&contact[0].geom, sizeof(dContact));
	if (numContacts > 0) 
	{
		for (int i=0; i<numContacts; i++) 
		{
			// Perform the dynamics
			dContactJoint c(world_->id(),contactGroup_->id(),&contact[i]);
			c.attach(b1,b2);

			// make call backs to the objects
			CollisionInterface *obj1 = NULL;
			if (b1)
			{
				obj1 = static_cast<CollisionInterface*>(dBodyGetData(b1));
			}

			CollisionInterface *obj2 = NULL;
			if (b2)
			{
				obj2 = static_cast<CollisionInterface*>(dBodyGetData(b2));
			}

			if (obj1 && obj2)
			{
				// collide together
				obj1->collide(obj2);
				obj2->collide(obj1);
			}
			
			if (obj1)
			{
				obj1->collide();
			}
			else
			{
				obj2->collide();
			}
		}
	}
}

SimulatedObject* SimulationManager::createSphere(const vector3 &position, float radius, 
												 float mass, DynamicObjectBase* controller)	
{
	updateThings_.push_back(controller);
	return new TestSphere(world_, space_, position, radius, mass, controller);
}
SimulatedObject* SimulationManager::createBox(const vector3 &position, const vector3& dimensions, float mass, 
											  DynamicObjectBase* controller)
{
	updateThings_.push_back(controller);
	return new TestBox(world_, space_, position, dimensions, mass, controller);
}

