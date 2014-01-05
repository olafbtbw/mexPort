#include "mexFunction.h"

void preProcessing(int, mxArray* [], int, mxArray* []);

MEX_FUNCTION_API void mexFunction(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[])
{
	preProcessing(nlhs, plhs, nrhs, prhs);
}
