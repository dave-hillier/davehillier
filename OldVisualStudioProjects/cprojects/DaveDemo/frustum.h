#ifndef __FRUSTUM__
#define __FRUSTUM__

#include "mathlib.h"
using namespace platformMath;

void gen_clipmat(void);
int cliptest(Bounds bb);

#endif // __FRUSTUM__