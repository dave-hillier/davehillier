/*
   Copyright (C) 2002 Nate Miller nathanm@uci.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
/*
   WORKLOG

   TODO
      - make it so I can script camera events
*/
#ifndef __CAMERAH__
#define __CAMERAH__

#include "Quat.h"
#include "Mathlib.h"

using namespace platformMath;

// Position you pass camera is where the camera is in space.
//
// The matrix the camera generates will negate the position so it can be
// used in OpenGL for example.
struct Camera
{
   Camera();

   void SetRotateX(const float &amnt);
   void SetRotateY(const float &amnt);
   void SetRotateZ(const float &amnt);
   
   void RotateY(const float &amnt);
   void RotateX(const float &amnt);
   void RotateZ(const float &amnt);

   void Translate(const Vector3 &trans);
   void SetPosition(const Vector3 &p)   {position.Set(p); dirty = 1;}
   const Vector3 &GetPosition(void)     {return position;}

   // !!TODO temp, just a test
   void GetDirection(Vector3 &dir);

   float *GetMatrix(void);
   void GetMatrix(Matrix &m);

   Vector3 rotation;
   Vector3 position;

protected:
   bool dirty;
   Quat axis[3];
   Matrix matrix;
};
extern Camera camera;
#endif