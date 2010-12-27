#ifndef _Common_h_
#define _Common_h_

#include <math.h>
#include <assert.h>

extern const float PI;
extern const float Epsilon;

extern const float DegToRadVal;

inline float DegToRad(float angle) {return angle * DegToRadVal;}


//inline void ASSERT() DebugBreak()

#endif // _Common_h_