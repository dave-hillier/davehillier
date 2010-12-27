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
   *** Do not include Vector3 or Matrix on their own, must use MathLib.h ***

   WORKLOG
   
   TOGO
*/
#ifndef __MATHLIBH__
#define __MATHLIBH__

namespace platformMath {

#include <math.h>

#ifndef uint
typedef unsigned int uint;
#endif

struct MathLib
{
   template <class type>
   static type Abs(type t)         {return (t >= 0 ? t : -t);}
   template <class type>
   static type Max(type a, type b) {return (a > b ? a : b);}
   template <class type>
   static type Min(type a, type b) {return (a < b ? a : b);}

   static double DegToRad(float angle)
    {return angle * DegToRadVal;}
   static float Cos(float angle)
    {return (float) cos(angle * DegToRadVal);}
   static float Sin(float angle)
    {return (float) sin(angle * DegToRadVal);}

   static const float PI;
   static const float Epsilon;
   static const float TwoPI;
   static const float PIOverTwo;
   static const float DegToRadVal;
};

}

#include "Vector3.h"
#include "Matrix.h"

#endif