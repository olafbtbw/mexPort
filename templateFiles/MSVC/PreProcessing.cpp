#include <vector>
#include "extMex.h"
#define VERBOSE

void preProcessing(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[])
{
#ifdef VERBOSE
#ifdef NDEBUG
	std::string dbgStr("C++ Release version");
#else
	std::string dbgStr("C++ Debug version");
#endif
	mexWarnMsgTxt(mexFunctionName());
	mexWarnMsgTxt(dbgStr.c_str());
#endif

	if (1 == nrhs)
	{
		// the input
		double* xPtr = mxGetPr(prhs[0]);
		size_t sizeX = std::max(mxGetN(prhs[0]), mxGetM(prhs[0]));


		//// prepare the output containers
		std::vector<std::vector<double>> idx;
		idx.push_back(std::vector<double>());
		idx.push_back(std::vector<double>());


		//// write output
		if (1 == nlhs)
		{
			plhs[0] = mxCreateDoubleMatrix(idx[0].size(), 2, mxREAL);
			double* plhsPtr = mxGetPr(plhs[0]);
			memcpy(plhsPtr, idx[0].data(), idx[0].size()*sizeof(double));
			plhsPtr = mxGetPr(plhs[1])+idx[0].size();
			memcpy(plhsPtr, idx[1].data(), idx[1].size()*sizeof(double));
		}
	}
}

