
#ifndef __MATHLIBH__
#define __MATHLIBH__

#include <math.h>
#define PI (3.141592654f)
#define DEG2RAD PI/180.0

#define Deg2Rad(a) ((a)*PI/180)
#define Rad2Deg(a) ((a)*180/PI)

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

   static const float ML_PI;
   static const float Epsilon;
   static const float TwoPI;
   static const float PIOverTwo;
   static const float DegToRadVal;
};

}

#include "Vector3.h"
#include "Matrix.h"

#endif