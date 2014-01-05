//
//  mexGateway.h
//  a generic library for porting data between Matlab and C or STL data types
//
//  Created by Olaf Schleusing on 06/16/12.
//

#ifndef __mexGateway_h__
#define __mexGateway_h__

#include <vector>

#if __cplusplus == 201103 // C++11
#include <type_traits>
using std::is_pointer;
using std::remove_pointer;
using std::is_arithmetic;
#else // C++03
#ifdef _WIN32
#include <type_traits>
#else
#include <tr1/type_traits>
#endif
using std::tr1::is_pointer;
using std::tr1::remove_pointer;
using std::tr1::is_arithmetic;
#endif

#include "mex.h"

namespace MexPort {


////////////////////////////////////////////////////////////////////////////////
// PUBLIC interface
////////////////////////////////////////////////////////////////////////////////

// import/export of std::string:
template<typename StringType>
void portStringFromMex(const mxArray* mxArr, StringType* retString);
template<typename StringType>
void portStringToMex(const StringType& str, mxArray** mxArr);

// scalar values of any type:
template <typename T>
T portFromMex(const mxArray* mxArr);
template <typename T>
void portToMex(const T* nativeArray, mxArray** mxArr);

// std::vector data
template <typename Cont>
void portFromMex(const mxArray* mxArr, Cont* nativeArray);
template <typename T>
void portToMex(const std::vector<T>& nativeArray, mxArray** mxArr,
                    mxComplexity complexity = mxREAL, bool isColumnVector=false);

// C arrays
template <typename Cont>
void portFromMex(const mxArray* mxArr, Cont* nativeArray, size_t**);
template <typename T>
void portToMex(const T* nativeArray, const size_t* dimSizes, mxArray** mxArr,
                    mxComplexity complexity = mxREAL, bool isColumnVector=false);

// a helper to free multi-dimensional array memory
template <typename Cont>
    void freeNDimArray(Cont mxArr, const size_t* sz, const size_t dimCtr=1);



////////////////////////////////////////////////////////////////////////////////
// PRIVATE interface
////////////////////////////////////////////////////////////////////////////////

// forward declarations of some helper functions
template <typename T> size_t numberOfDims();
template <typename Cont>
void mxToCCppArray(const mxArray* mxArr, Cont* nativeArray, size_t** dims,
                   const size_t offset=0U, const size_t stepSz=1U);
template <typename Cont>
void mxToCCppArray(const mxArray* mxArr, Cont* nativeArray,
                   const size_t offset=0U, const size_t stepSz=1U);
static mxArray* matlabTranspose2DArray(const mxArray*);
template <typename Cont>
bool checkDimMatching(Cont* cont, const mxArray* mxArr);
template <typename T, typename S>
void getDimSizes(const std::vector<T>& nativeArray, std::vector<S>* dimSizes);
template <typename Cont>
static size_t getCurrDimension(const mxArray* mxArr, Cont*);
template <typename T>
void cCppArrayToMx(const std::vector<T>& nativeArray, mxArray** mxArr, const size_t offset=0U, const size_t stepSz=1U);
template <typename T>
void cCppArrayToMx(const T* nativeArray, const size_t* dimSizes, mxArray** mxArr, const size_t offset=0U, const size_t stepSz=1U);

    
////////////////////////////////////////////////////////////////////////////////
// porting data from C or STL data types to mex format
////////////////////////////////////////////////////////////////////////////////

template <typename T>
void portToMex(const std::vector<T>& nativeArray, mxArray** mxArr, mxComplexity complexity, bool isColumnVector)
{
    const size_t nDims = numberOfDims<std::vector<T> >();
    if (1 == nDims) {
        const size_t len = (complexity == mxREAL) ? nativeArray.size() : nativeArray.size()/2;
        *mxArr = isColumnVector ? mxCreateDoubleMatrix(len, 1, complexity) :
        mxCreateDoubleMatrix(1, len, complexity);
    }
    else {
        std::vector<mwSize> dimSizes;
        getDimSizes(nativeArray, &dimSizes);
        dimSizes.back() = (complexity == mxREAL) ? dimSizes.back() : dimSizes.back()/2;
        *mxArr = mxCreateNumericArray(nDims, &(dimSizes[0]), mxDOUBLE_CLASS, complexity);
    }
    if (*mxArr == NULL) {
        mexErrMsgTxt("Could not create mxArray.\n");
        return;
    }
    cCppArrayToMx(nativeArray, mxArr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
void portToMex(const T* nativeArray, const size_t* dimSizes, mxArray** mxArr, mxComplexity complexity, bool isColumnVector)
{
    const size_t nDims = numberOfDims<T*>();
    if (1 == nDims) {
        const size_t len = (complexity == mxREAL) ? *dimSizes : (*dimSizes)/2;
        *mxArr = isColumnVector ? mxCreateDoubleMatrix(len, 1, complexity) :
        mxCreateDoubleMatrix(1, len, complexity);
    }
    else {
        size_t* dimSizesCopy = new size_t[nDims];
        memcpy(dimSizesCopy, dimSizes, nDims*sizeof(size_t));
        dimSizesCopy[nDims-1] = (complexity == mxREAL) ? dimSizesCopy[nDims-1] : dimSizesCopy[nDims-1]/2;
        *mxArr = mxCreateNumericArray(nDims, dimSizesCopy, mxDOUBLE_CLASS, complexity);
        delete [] dimSizesCopy;
    }
    if (*mxArr == NULL) {
        mexErrMsgTxt("Could not create mxArray.\n");
        return;
    }
    cCppArrayToMx(nativeArray, dimSizes, mxArr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
void portToMex(const T val, mxArray** mxArr)
{
    *mxArr = mxCreateDoubleScalar(static_cast<T>(val));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
void cCppArrayToMxLastDim(const T* ptrData, mxArray** mxArr, const size_t offset, const size_t stepSz, const size_t len);
template <bool ReachedFinalType> struct CCppArrayToMx;
template <> struct CCppArrayToMx<true>;
template <> struct CCppArrayToMx<false>;

////////////////////////////////////////////////////////////////////////////////

template <typename T>
void cCppArrayToMx(const std::vector<T>& nativeArray, mxArray** mxArr, const size_t offset, const size_t stepSz)
{
    CCppArrayToMx<is_arithmetic<T>::value>::_cCppArrayToMx(nativeArray, mxArr, offset, stepSz);
}

template <typename T>
void cCppArrayToMx(const T* nativeArray, const size_t* dimSizes, mxArray** mxArr, const size_t offset, const size_t stepSz)
{
    CCppArrayToMx<is_arithmetic<T>::value>::_cCppArrayToMx(nativeArray, dimSizes, mxArr, offset, stepSz);
}

////////////////////////////////////////////////////////////////////////////////

template <> struct CCppArrayToMx<false> {
    template <typename T>
    static void _cCppArrayToMx(const std::vector<T>& nativeArray, mxArray** mxArr, const size_t offset, const size_t stepSz)
    {
        for (size_t k=0; k<nativeArray.size(); ++k) {
            cCppArrayToMx(nativeArray[k], mxArr, offset + k*stepSz, stepSz*nativeArray.size());
        }
    }
    template <typename T>
    static void _cCppArrayToMx(const T* nativeArray, const size_t* dimSizes, mxArray** mxArr, const size_t offset, const size_t stepSz)
    {
        for (size_t k=0; k<dimSizes[0]; ++k) {
            cCppArrayToMx(nativeArray[k], &(dimSizes[1]), mxArr, offset + k*stepSz, stepSz*dimSizes[0]);
        }
    }
};

template <> struct CCppArrayToMx<true> {
    template <typename T>
    static void _cCppArrayToMx(const std::vector<T>& nativeArray, mxArray** mxArr, const size_t offset, const size_t stepSz)
    {
        if (!(nativeArray.empty())) {
            cCppArrayToMxLastDim(&(nativeArray.at(0)), mxArr, offset, stepSz, nativeArray.size());
        }
    }
    template <typename T>
    static void _cCppArrayToMx(const T* nativeArray, const size_t* dimSizes, mxArray** mxArr, const size_t offset, const size_t stepSz)
    {
        if (0 != *dimSizes) {
            cCppArrayToMxLastDim(nativeArray, mxArr, offset, stepSz, *dimSizes);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
void cCppArrayToMxLastDim(const T* ptrData, mxArray** mxArr, const size_t offset, const size_t stepSz, const size_t len)
{
    double* pReal = mxGetPr(*mxArr) + offset;
    double* pImag = NULL;
    if (mxIsComplex(*mxArr)) {
        pImag = mxGetPi(*mxArr) + offset;
    }
    const T* endPos(ptrData+len);
    while (ptrData<endPos) {
        *pReal = static_cast<double>(*ptrData);
        ++ptrData;
        pReal += stepSz;
        if (NULL != pImag) {
            *pImag = static_cast<double>(*ptrData);
            ++ptrData;
            pImag += stepSz;
        }
    }    
}

////////////////////////////////////////////////////////////////////////////////
// porting data from mex format to C or STL data types
////////////////////////////////////////////////////////////////////////////////

// helper struct to overcome the unavailability of partial function template specialization in C++
template <bool ReachedFinalType, bool IsCArray> struct MxToCCppArray;
template <> struct MxToCCppArray<false, false>;
template <> struct MxToCCppArray<true, false>;
template <> struct MxToCCppArray<false, true>;

// STL data types
template <typename Cont>
void portFromMex(const mxArray* mxArr, Cont* nativeArray)
{
    if (checkDimMatching(nativeArray, mxArr)) {

        if (1 == numberOfDims<Cont>() && 1 == mxGetN(mxArr)) {
            mxArray* arrTransposed = matlabTranspose2DArray(mxArr);
            mxToCCppArray(arrTransposed, nativeArray, (size_t)0U, 1U);
            mxDestroyArray(arrTransposed);
        }
        else {
            mxToCCppArray(mxArr, nativeArray, (size_t)0U, 1U);
        }
    }
}

// C data types
template <typename Cont>
void portFromMex(const mxArray* mxArr, Cont* nativeArray, size_t** dims)
{
    if (checkDimMatching(nativeArray, mxArr)) {
        mxArray* mxArrTransposed(0);
        const size_t numDimsMex = mxGetNumberOfDimensions(mxArr);
        if (numDimsMex == numberOfDims<Cont>()) {
            *dims = new size_t[numDimsMex];
            memcpy(*dims, mxGetDimensions(mxArr), sizeof(size_t)*numDimsMex);
            if (mxIsComplex(mxArr)) {
                (*dims)[numDimsMex-1] *= 2;
            }
        }
        else if (2 == numDimsMex && 1 == std::min(mxGetN(mxArr),mxGetM(mxArr))) {
            *dims = new size_t;
            **dims = std::max(mxGetN(mxArr),mxGetM(mxArr));
            if (mxIsComplex(mxArr)) {
                (**dims) *= 2;
            }
            if (1 == numberOfDims<Cont>() && 1 == mxGetN(mxArr)) {
                mxArrTransposed = matlabTranspose2DArray(mxArr);
            }
        }
        if (mxArrTransposed) {
            mxToCCppArray(mxArrTransposed, nativeArray, NULL, (size_t)0U, 1U);
            mxDestroyArray(mxArrTransposed);
        }
        else {
            mxToCCppArray(mxArr, nativeArray, NULL, (size_t)0U, 1U);
        }
    }
}

// scalar values
template <typename T>
T portFromMex(const mxArray* mxArr)
{
    if (2 == mxGetNumberOfDimensions(mxArr) && 1 == mxGetN(mxArr) &&
            1 == mxGetM(mxArr) && !mxIsComplex(mxArr)) {
        return static_cast<T>(mxGetScalar(mxArr));
    }
    else {
        mexWarnMsgTxt("ERROR: number of dimensions in mxArray and C/C++ type do not match");
        return 0;
    }
}

// recursing through the N dimensions for STL containers
template <> struct MxToCCppArray<false, false> {
    template<typename Cont>
    static void _mxToCCppArray(const mxArray* mxArr,
                        Cont* nativeArray, const size_t offset, const size_t stepSz) {
        const size_t len = getCurrDimension(mxArr, nativeArray);
        
        nativeArray->assign(len, typename Cont::value_type());
        typename Cont::iterator it = nativeArray->begin();
        for (; it!=nativeArray->end(); ++it) {
            size_t ctr = it - nativeArray->begin();
            mxToCCppArray(mxArr, &(*it), offset + ctr*stepSz, stepSz*len);
        }
    }
};

// recursing through the N dimensions for C arrays
template <> struct MxToCCppArray<false, true> {
    template<typename Cont>
    static void _mxToCCppArray(const mxArray* mxArr, Cont* nativeArray,
                                    const size_t offset, const size_t stepSz) {
        const size_t len = getCurrDimension(mxArr, nativeArray);
        
        *nativeArray = new typename remove_pointer<Cont>::type[len];
        Cont it = nativeArray[0];
        for (size_t k=0; k<len; ++k) {
            mxToCCppArray(mxArr, it++, NULL, offset + k*stepSz, stepSz*len);
        }
    }
};

// recursed to the final dimension, copy the data from mex to C/C++
template <> struct MxToCCppArray<true, false> {
    template<typename Cont>
    static void _mxToCCppArray(const mxArray* mxArr, Cont* nativeArray,
                                const size_t offset, const size_t stepSz) {
        double* arrPtrRe = mxGetPr(mxArr) + offset;
        double* arrPtrIm = NULL;
        if (mxIsComplex(mxArr)) {
            arrPtrIm = mxGetPi(mxArr) + offset;
        }
        
        const size_t len = getCurrDimension(mxArr, nativeArray);
        if (mxIsComplex(mxArr)) {
            resizeContainer(nativeArray, 2*len);
        }
        else {
            resizeContainer(nativeArray, len);
        }
        
        copyDataGeneric(arrPtrRe, arrPtrIm, stepSz, nativeArray, len);
    }

    template <typename T>
    static void resizeContainer(std::vector<T>* nativeArray, const size_t len) {
        nativeArray->resize(len);
    }
    template <typename T>
    static void resizeContainer(T** nativeArray, const size_t len) {
        *nativeArray = new T[len];
    }

    template <typename T>
    static void copyDataGeneric(const double* pReal, const double* pImag,
                const size_t stepSz, std::vector<T>* nativeArray, const size_t len)
    {
        T* it = &(*(nativeArray->begin()));
        copyData(pReal, pImag, stepSz, it, len);
    }
    
    template <typename T>
    static void copyDataGeneric(const double* pReal, const double* pImag,
                const size_t stepSz, T** nativeArray, const size_t len)
    {
        T* it = &(*(nativeArray[0]));
        copyData(pReal, pImag, stepSz, it, len);
    }
    
    template <typename T>
    static void copyData(const double* pReal, const double* pImag,
                const size_t stepSz, T* ptrData, const size_t len)
    {
        for (unsigned k=0; k<len; ++k) {
            *ptrData = static_cast<T>(*pReal);
            ++ptrData;
            pReal += stepSz;
            if (NULL != pImag) {
                *ptrData = static_cast<T>(*pImag);
                ++ptrData;
                pImag += stepSz;
            }
        }        
    }
    
};

template <typename Cont>
void mxToCCppArray(const mxArray* mxArr, Cont* nativeArray, size_t** dims,
                   const size_t offset, const size_t stepSz) {
    MxToCCppArray<
    is_arithmetic<typename remove_pointer<Cont>::type>::value,
    is_pointer<typename remove_pointer<Cont>::type>::value>::
    _mxToCCppArray(mxArr, nativeArray, offset, stepSz);

}

template <typename Cont>
void mxToCCppArray(const mxArray* mxArr, Cont* nativeArray,
                   const size_t offset, const size_t stepSz) {
    MxToCCppArray<is_arithmetic<typename Cont::value_type>::value, false>::
    _mxToCCppArray(mxArr, nativeArray, offset, stepSz);
}

////////////////////////////////////////////////////////////////////////////////
// string implementations
////////////////////////////////////////////////////////////////////////////////

template<typename StringType>
void portStringFromMex(const mxArray* mxArr, StringType* retString)
{
    char* str(0);
    size_t bufLen(0);
    bufLen = mxGetN(mxArr)*sizeof(mxChar)+1;
    str = (char*)mxMalloc(bufLen);
    int status = mxGetString(mxArr, str, bufLen);
    if (0 == status) {
        retString->assign(str);
        mxFree(str);
    }
}

template<typename StringType>
void portStringToMex(const StringType& str, mxArray** mxArr)
{
    *mxArr = mxCreateString(str.c_str());
}

////////////////////////////////////////////////////////////////////////////////
// free multi-dimensional memory
////////////////////////////////////////////////////////////////////////////////

template <bool ReachedFinalType> struct FreeNDimArray;
template <> struct FreeNDimArray<true>;
template <> struct FreeNDimArray<false>;

template <typename Cont>
void freeNDimArray(Cont mxArr, const size_t* sz, const size_t dimCtr)
{
    FreeNDimArray<is_arithmetic<typename remove_pointer<Cont>::type>::value>::_freeNDimArray(mxArr, sz, dimCtr);
}

template <> struct FreeNDimArray<true> {
    template<typename Cont>
    static void _freeNDimArray(Cont mxArr, const size_t* sz, const size_t dimCtr) {
        if (0 != sz[dimCtr-1]) {
            delete [] mxArr;
        }
    }
};
template <> struct FreeNDimArray<false> {
    template<typename Cont>
    static void _freeNDimArray(Cont mxArr, const size_t* sz, const size_t dimCtr) {
        for (unsigned k=0; k<sz[dimCtr-1]; ++k) {
            freeNDimArray(mxArr[k], sz, dimCtr+1);
        }
        delete mxArr;
    }
};


////////////////////////////////////////////////////////////////////////////////
// helpers
////////////////////////////////////////////////////////////////////////////////

template <typename Cont>
bool checkDimMatching(Cont* cont, const mxArray* mxArr)
{
    size_t nDimsMx = mxGetNumberOfDimensions(mxArr);
    size_t nDimsCpp = numberOfDims<Cont>();
    if (nDimsMx != nDimsCpp)
    {
        if (!(2 == nDimsMx && (1==nDimsCpp) && (1 == std::min(mxGetN(mxArr),mxGetM(mxArr))) )) {
            mexWarnMsgTxt("ERROR: number of dimensions in mxArray and C/C++ type do not match");
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <bool ReachedFinalType> struct GetDimSizes;
template <> struct GetDimSizes<false> {
    template<typename T, typename S>
    static void getDimSizesMem(const std::vector<T>& nativeArray, std::vector<S>* dimSizes)
    {
        dimSizes->push_back(nativeArray.size());
        if (!(nativeArray.empty())) {
            getDimSizes(nativeArray.at(0), dimSizes);
        }
    }
};
template <> struct GetDimSizes<true> {
    template<typename T, typename S>
    static void getDimSizesMem(const std::vector<T>& nativeArray, std::vector<S>* dimSizes)
    {
        dimSizes->push_back(nativeArray.size());
    }
};

template <typename T, typename S>
void getDimSizes(const std::vector<T>& nativeArray, std::vector<S>* dimSizes)
{
    GetDimSizes<is_arithmetic<T>::value>::getDimSizesMem(nativeArray, dimSizes);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static inline mxArray* matlabTranspose2DArray(const mxArray* mxArr)
{
    const mxArray* mxArrayIn[2];
    mxArrayIn[0] = mxArr;
    mxArrayIn[1] = mxCreateDoubleMatrix(1, 2, mxREAL);
    double* ptrIn = mxGetPr(mxArrayIn[1]);
    ptrIn[0] = 2.0;
    ptrIn[1] = 1.0;

    mxArray* mxArrayOut;
    mexCallMATLAB(1, &mxArrayOut, 2, (mxArray**)mxArrayIn, "permute");
    
    return mxArrayOut;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    
// helper structs and functions to determine the dimensions of an array or stl container
template <bool ReachedFinalType, bool isPointerType> struct DimsCounter;
template <> struct DimsCounter<false, true>;
template <> struct DimsCounter<true, false>;
template <> struct DimsCounter<false, false>;

template <typename T>
size_t numberOfDims()
{
    return DimsCounter<is_arithmetic<T>::value, is_pointer<T>::value>::template count<T>();
}

// default implementation for stl containers
template <>
struct DimsCounter<false, false> {
    template <typename T>
    static size_t count() {
        return numberOfDims<typename T::value_type>() + 1;
    }
    
};

// partial template specialization for pointer types (e.g. float*, float**, etc.)
template <>
struct DimsCounter<false, true> {
    template <typename T>
    static size_t count() {
        return numberOfDims<typename remove_pointer<T>::type>() + 1;
    }
};

// partial template specialization for types that cannot be dereferenced any further
template <>
struct DimsCounter<true, false> {
    template <typename T>
    static size_t count() {
        return 0; // ends the recursion and returns zero dimensions
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    
template <typename Cont>
static size_t getCurrDimension(const mxArray* mxArr, Cont*)
{
    size_t dimCtr = mxGetNumberOfDimensions(mxArr) - numberOfDims<Cont>();
    const mwSize* dims = mxGetDimensions(mxArr);
    return dims[dimCtr];
}
    

} // namespace MexPort

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif 
