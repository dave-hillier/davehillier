#include "Entity.h"

#include "JoystickManager.h"
#include "Joystick.h"

#include "maths/vector.h"
#include "maths/matrix.h"

#include "TestApp.h"

#include <iostream>
#include <cassert>
#include <Windows.h>
#include <gl/gl.h>

#include <iostream>
inline std::ostream& operator<< (std::ostream& os, const vector3 &v )
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")"; 
	return os;
}	

inline std::ostream& operator<< (std::ostream& os, const quaternion &q )
{
	os << "("  << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")"; 
	return os;
}	

namespace 
{
	class TestRender : public Renderable
	{
	public:
		TestRender(Entity *e) : parent_(e) 
		{
			globalApp().renderManager().add(this);
		}
		virtual ~TestRender() 
		{
			globalApp().renderManager().remove(this);
		}	
		void render(double timeNow) 
		{
			glPushMatrix();
			position_= parent_->position();
			orientation_ = parent_->orientation();
			

			matrix44 m(orientation_);
			m.translate(position_);

			
			glMultMatrixf((GLfloat*)m.m);
			{
				float l = .75;//size/2;
				glBegin(GL_LINE_STRIP);
				// Front
				glColor3f(1,1,1);
				glVertex3f( l,  l, l);
				glColor3f(1,0,0);
				glVertex3f( l, -l, l);
				glColor3f(1,1,0);
				glVertex3f(-l, -l, l);
				glColor3f(1,1,1);
				glVertex3f(-l,  l, l);
				glColor3f(1,1,1);
				glVertex3f( l,  l, l);
				glColor3f(1,1,1);
				// back

				glColor3f(0,1,0);

				glVertex3f( l,  l, -l);
				glVertex3f( l, -l, -l);
				glVertex3f(-l, -l, -l);
				glVertex3f(-l,  l, -l);
				glVertex3f( l,  l, -l);

				glEnd();

				glColor3f(1,1,1);
				// missing edges
				glBegin(GL_LINES);
				glVertex3f( l, -l, -l);
				glVertex3f( l, -l,  l);
				glVertex3f(-l, -l, -l);
				glVertex3f(-l, -l,  l);
				glVertex3f(-l,  l, -l);
				glVertex3f(-l,  l,  l);
				glEnd();
			}
			glPopMatrix();
		}


		virtual const vector3 &position() const {  return position_; }
		virtual const quaternion &orientation() const { return orientation_; }
		virtual float radius() const { return 1.5; }
	private:
		Entity *parent_;

		vector3 position_;
		quaternion orientation_;
	};
}

Entity::Entity()
{
//	set(globalApp().simulationManager().createBox(vector3(0,10,-20), vector3(1.5f,1.5f,1.5f), 1, this));
	set(globalApp().simulationManager().createSphere(vector3(0,10,-20), 1.5f, 1, this));

	internal_ = new TestRender(this);
}

Entity::~Entity()
{
	// Unregister from all updates
	delete internal_;
}

void Entity::update(double time)
{
	Joystick* joystick = JoystickManager::instance().getJoystick(0);

	if (joystick)
	{
		Joystick &j = *joystick;
		//yaw_ += -j.axis(2) * 0.001f;
		//pitch_ += j.axis(3) * 0.001f;
		//float roll = 0;// TODO? interesting effects?
		//orientation_.set_orientation_zyx(pitch_, yaw_, roll);

		vector3 vel(velocity());
		quaternion o(orientation());
		vector3 forward(0,0,1);
		vector3 side(1,0,0);
		vector3 up(0,1,0);
		//vector3 forward = o.rotate(vector3(0,0,1));
		//vector3 side = o.rotate(vector3(1,0,0));
		//vector3 up = o.rotate(vector3(0,1,0));
		forward *= j.axis(1) * 1000;
		side *= j.axis(0) * 1000;
		up *= -j.axis(3) * 1000;
		vector3 drag = -vel * 10.0f; 

		applyForce(forward);
		applyForce(side);
		applyForce(drag);
		applyForce(up);
	}
	else
	{
//		debugLog << "Entity::update failed to get joystick " << std::endl;
	}


}

void Entity::collide(CollisionInterface *)
{

}

void Entity::collide()
{
}


