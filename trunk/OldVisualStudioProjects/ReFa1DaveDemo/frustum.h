#ifndef __FRUSTUM__
#define __FRUSTUM__

#include "mathlib.h"
#include "Vector3.h"

void gen_clipmat(void);
int cliptest(Bounds bb);

#endif // __FRUSTUM__