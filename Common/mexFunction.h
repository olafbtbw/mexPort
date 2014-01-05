#ifndef __MEXFUNCTION__
#define __MEXFUNCTION__

#include "extMex.h"

#define MEX_FUNCTION_EXPORTS
#ifdef MEX_FUNCTION_EXPORTS
#define MEX_FUNCTION_API __declspec(dllexport)
#else
#define MEX_FUNCTION_API __declspec(dllimport)
#endif
MEX_FUNCTION_API void mexFunction(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[]);

#endif
