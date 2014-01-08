//
//  utils.cpp
//
//  Created by Olaf Schleusing on 12/16/13.
//


#define PRINT_AUX_INFO_AT_BEGINNING 0

#include <string>
#include "mexPort.h"
#include "mex.h"

extern "C" {
#include "utils.h"
}

using namespace MexPort;

void printAuxInfo()
{
    std::string dbgStr(mexFunctionName());
#if PRINT_AUX_INFO_AT_BEGINNING
#ifdef NDEBUG
    dbgStr.append("; C/C++ Release version");
#else
    dbgStr.append("; C/C++ Debug version");
#endif
    mexWarnMsgTxt(dbgStr.c_str());
#endif
}

void plot(const float* x, const size_t len)
{
    mxArray* ptMexArray;
    exportToMex(x, &len, &ptMexArray);
    mexCallMATLAB(0, 0, 1, &ptMexArray, "plot");
    mxDestroyArray(ptMexArray);
}


void matlabFFT(const float* in, const size_t szIn, const size_t fftLen, float** out)
{
    mxArray* ptMexArrayIn[2];
    mxArray* ptMexArrayOut;
    exportToMex(in, &szIn, &(ptMexArrayIn[0]));
    ptMexArrayIn[1] = mxCreateDoubleScalar(fftLen);
    mexCallMATLAB(1, &ptMexArrayOut, 2, ptMexArrayIn, "fft");
    size_t* szOut;
    importFromMex(ptMexArrayOut, out, &szOut);
    mxDestroyArray(ptMexArrayOut);
    delete szOut;
}

void matlabIFFT(const float* in, size_t szIn, float** out)
{
    mxArray* ptMexArrayIn;
    mxArray* ptMexArrayOut;
    exportToMex(in, &szIn, &ptMexArrayIn);
    mexCallMATLAB(1, &ptMexArrayOut, 1, &ptMexArrayIn, "ifft");
    size_t* szOut;
    importFromMex(ptMexArrayOut, out, &szOut);
    mxDestroyArray(ptMexArrayOut);
    delete szOut;
}

