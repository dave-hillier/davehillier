#ifndef _NewRoad_h
#define _NewRoad_h

#include "Vector3.h"
#include <vector>

// Impl
//#include <QtOpenGL>
//#include <QColor>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <algorithm>

class NewRoad
{
public:
	NewRoad(const Vector3 &start) : roadWidth_(4)
	{
		AddNode(start);
	}

	~NewRoad()
	{
		// Delete all nodes
	}

	Vector3 HalfCornerVector(const Vector3 &a, const Vector3 &b, const Vector3 &centre) {
		float width = roadWidth_ / sinf( acosf ( a.DotProduct(b) )/2 ) ;
		Vector3 c = (a + b) * 0.5;

		float a1 = atan2(a.y(), a.x()) + 3.14159265f;
		float b1 = atan2(b.y(), b.x()) + 3.14159265f;
		float c1 = atan2(c.y(), c.x()) + 3.14159265f;

		if (( a1 < c1 ) && ( c1 < b1 ))
			c *= -1;
	// TODO There must be another case where this can be valid

		c.Normalise();
		c *= fabsf(width);
		c += centre;

		return c;
	}


	void Corners(Vector3 &end1, Vector3 &end2, const Vector3 &endMid, const Vector3 &direction)
	{
		end1 = CrossProduct (direction, Vector3(0,0,1));
		end1 *= -roadWidth_;
		end2 = end1 * -1;
		end1 += endMid;
		end2 += endMid;		
	}

	Vector3 Direction(Vector3 &end, const Vector3 &centre)
	{
		end += centre;
		end *= 0.5;
		Vector3 a(end-centre);
		a.Normalise(); 
		return a;
	}

	void Draw()
	{
		glPushMatrix();
		glTranslatef(0,-50,-150);
		roadWidth_ = 4;
		Draw1();
		roadWidth_ = 2;
		Draw1();
		glPopMatrix();
	}

	void Draw1() 
	{
		const float colour[][3] = {
			{ 1, 0, 0 },
			{ 1, 1, 0 },
			{ 0, 1, 0 },
			{ 0, 1, 1 },
			{ 0, 0, 1 },
			{ 1, 0, 1 },
			{ 1, 1, 1 },
		};

		// For each node
		for (std::vector<Node*>::const_iterator itt1 = nodes_.begin(); itt1 != nodes_.end(); ++itt1)
		{
			// For each connection, draw junction in direction of connection
		
			glPushMatrix();
			glColor3f(1,1,1);
			glTranslatef((*itt1)->position_.x(), (*itt1)->position_.y(), 0);
			glBegin(GL_LINE_STRIP);
			glVertex3f(roadWidth_, roadWidth_, 0);
			glVertex3f(roadWidth_, -roadWidth_, 0);
			glVertex3f(-roadWidth_, -roadWidth_, 0);
			glVertex3f(-roadWidth_, roadWidth_, 0);
			glVertex3f(roadWidth_, roadWidth_, 0);
			glEnd();
			glPopMatrix();

			// Draw to half way
			if ( (*itt1)->connections_.size() == 1 ) // End
			{
				const Vector3 &centre((*itt1)->position_);
				Vector3 end1((nodes_[(*itt1)->connections_[0]])->position_);
				Vector3 a(Direction(end1, centre));

				Vector3 c,d,e,f;
				Corners(c, d, centre, a);
				Corners(f, e, end1, a);
			
				int col = (itt1 - nodes_.begin()) % 7;

				glColor3fv(colour[col]);
				glBegin(GL_LINE_STRIP);
				glVertex3f(c.x(), c.y(), c.z());
				glVertex3f(d.x(), d.y(), d.z());
				glVertex3f(e.x(), e.y(), e.z());
				glVertex3f(f.x(), f.y(), f.z());
				glVertex3f(c.x(), c.y(), c.z());
				glEnd();
			}
			else if ( (*itt1)->connections_.size() == 2 ) // bend
			{
				const Vector3 &centre((*itt1)->position_);
				Vector3 end1((nodes_[(*itt1)->connections_[0]])->position_);
				Vector3 end2((nodes_[(*itt1)->connections_[1]])->position_);

				Vector3 a(Direction(end1,centre));
				Vector3 b(Direction(end2,centre));

				int col = (itt1 - nodes_.begin()) % 7;
				Vector3 ab = HalfCornerVector(a,b,centre);
				Vector3 ba = HalfCornerVector(b,a,centre);

				Vector3 al, ar, bl, br, cl, cr;

				Corners(al, ar, end1, a);
				Corners(bl, br, end2, b);


				glColor3fv(colour[col]);
				glBegin(GL_LINE_STRIP);

				//glColor3fv(colour[0]);
				glVertex3f(ar.x(), ar.y(), ar.z());
				glVertex3f(ab.x(), ab.y(), ab.z());

				//glColor3fv(colour[1]);
				glVertex3f(bl.x(), bl.y(), bl.z());
				glVertex3f(br.x(), br.y(), br.z());
				glVertex3f(ba.x(), ba.y(), ba.z());

				//glColor3fv(colour[0]);
				glVertex3f(al.x(), al.y(), al.z());
				glVertex3f(ar.x(), ar.y(), ar.z());
				glEnd();

			}
			else if ( (*itt1)->connections_.size() == 3 ) // t junction
			{
				const Vector3 &centre((*itt1)->position_);
				Vector3 end1((nodes_[(*itt1)->connections_[0]])->position_);
				Vector3 end2((nodes_[(*itt1)->connections_[1]])->position_);
				Vector3 end3((nodes_[(*itt1)->connections_[2]])->position_);

				Vector3 a(Direction(end1,centre));
				Vector3 b(Direction(end2,centre));
				Vector3 c(Direction(end3,centre));

				Vector3 ab = HalfCornerVector(a,b,centre);
				Vector3 bc = HalfCornerVector(b,c,centre);
				Vector3 ca = HalfCornerVector(c,a,centre);

				Vector3 al, ar, bl, br, cl, cr;

				Corners(al, ar, end1, a);
				Corners(bl, br, end2, b);
				Corners(cl, cr, end3, c);

				int col = (itt1 - nodes_.begin()) % 7;

				glColor3fv(colour[col]);
				glBegin(GL_LINE_STRIP);

				//glColor3fv(colour[0]);
				glVertex3f(ar.x(), ar.y(), ar.z());
				glVertex3f(ab.x(), ab.y(), ab.z());

				//glColor3fv(colour[1]);
				glVertex3f(bl.x(), bl.y(), bl.z());
				glVertex3f(br.x(), br.y(), br.z());
				glVertex3f(bc.x(), bc.y(), bc.z());

				//glColor3fv(colour[2]);
				glVertex3f(cl.x(), cl.y(), cl.z());
				glVertex3f(cr.x(), cr.y(), cr.z());
				glVertex3f(ca.x(), ca.y(), ca.z());

				//glColor3fv(colour[0]);
				glVertex3f(al.x(), al.y(), al.z());
				glVertex3f(ar.x(), ar.y(), ar.z());
				glEnd();
			}
			else if ( (*itt1)->connections_.size() == 4 ) // cross roads
			{
				const Vector3 &centre((*itt1)->position_);
				Vector3 end1((nodes_[(*itt1)->connections_[0]])->position_);
				Vector3 end2((nodes_[(*itt1)->connections_[1]])->position_);
				Vector3 end3((nodes_[(*itt1)->connections_[2]])->position_);
				Vector3 end4((nodes_[(*itt1)->connections_[3]])->position_);

				Vector3 a(Direction(end1,centre));
				Vector3 b(Direction(end2,centre));
				Vector3 c(Direction(end3,centre));
				Vector3 d(Direction(end4,centre));

				Vector3 ab = HalfCornerVector(a,b,centre);
				Vector3 bc = HalfCornerVector(b,c,centre);
				Vector3 cd = HalfCornerVector(c,d,centre);
				Vector3 da = HalfCornerVector(d,a,centre); // TODO Why neg required?

				Vector3 al, ar, bl, br, cl, cr, dl, dr;

				Corners(al, ar, end1, a);
				Corners(bl, br, end2, b);
				Corners(cl, cr, end3, c);
				Corners(dl, dr, end4, d);

				int col = (itt1 - nodes_.begin()) % 7;

				glColor3fv(colour[col]);
				glBegin(GL_LINE_STRIP);

				//glColor3fv(colour[0]);
				glVertex3f(ar.x(), ar.y(), ar.z());
				glVertex3f(ab.x(), ab.y(), ab.z());

				//glColor3fv(colour[1]);
				glVertex3f(bl.x(), bl.y(), bl.z());
				glVertex3f(br.x(), br.y(), br.z());
				glVertex3f(bc.x(), bc.y(), bc.z());

				//glColor3fv(colour[2]);
				glVertex3f(cl.x(), cl.y(), cl.z());
				glVertex3f(cr.x(), cr.y(), cr.z());
				glVertex3f(cd.x(), cd.y(), cd.z());

				//glColor3fv(colour[3]);
				glVertex3f(dl.x(), dl.y(), dl.z());
				glVertex3f(dr.x(), dr.y(), dr.z());
				glVertex3f(da.x(), da.y(), da.z());

				//glColor3fv(colour[0]);
				glVertex3f(al.x(), al.y(), al.z());
				glVertex3f(ar.x(), ar.y(), ar.z());
				glEnd();
			}
			else
			{
				if ( (*itt1)->connections_.size() > 4 )
					assert(false);// Max 4 junction implemented

			}
			// TODO selection graphics?
		}
	}

	unsigned AddNode( const Vector3 &p )
	{
		Node *n = new Node(p);
		nodes_.push_back(n);
		return (unsigned)nodes_.size();
	}

	// Check whether the points wind in xy plane in a clockwise direction
	// static cos it doesnt really need to be in this class, but possibly useful elsewhere
	// TODO move to a utility class
	static bool Clockwise( const Vector3 &p1, const Vector3 &p2, const Vector3 &p3 )
	{
		bool result = false;

		Vector3 a = p1-p2;
		Vector3 b = p3-p2;

		if (a.DotProduct(b) >= 0) 
			result = true; 
		return result;

	}



	void InsertConnectionSort( unsigned n1, unsigned n2 )
	{
		assert(n1 < nodes_.size());
		assert(n2 < nodes_.size());
		nodes_[n1]->connections_.push_back(n2);
	
		ConnectionSorter a(nodes_, nodes_[n1]->position_);
		std::sort(nodes_[n1]->connections_.begin(), 
			nodes_[n1]->connections_.end(), 
			a);

	}

	void Connect(unsigned n1, unsigned n2) 
	{
		InsertConnectionSort(n1,n2);
		InsertConnectionSort(n2,n1);
	}

private:

	class Node
	{
	public:
		Node(const Vector3 &p) : position_(p) {}
		Vector3 position_;
		std::vector<unsigned> connections_; // index into nodes_
	};	
	struct ConnectionSorter /*: public std::binary_function<unsigned, unsigned, bool>*/
	{
		ConnectionSorter(std::vector<Node*> &nodes, const Vector3 &centre) 
			: nodes_(nodes), centre_(centre) {}
			bool operator()(unsigned a, unsigned b)
			{
				Vector3 v1 = nodes_[a]->position_-centre_;
				Vector3 v2 = nodes_[b]->position_-centre_;
				return atan2(v1.y(), v1.x()) > atan2(v2.y(), v2.x());
			}
			std::vector<Node*> &nodes_;
			Vector3 centre_;
	};
	
	std::vector<Node*> nodes_;

	float roadWidth_;
};

#endif // _NewRoad_h

