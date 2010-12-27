
#ifndef __MATHLIBH__
#define __MATHLIBH__

#include <math.h>
#define PI (3.141592654f)
#define DEG2RAD PI/180.0

template<typename T> T Deg2Rad(const T &a) { return a*PI/180; }
template<typename T> T Rad2Deg(const T &a) { return a*180/PI; }

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



#endif