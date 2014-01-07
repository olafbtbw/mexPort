//
//  utils.h
//
//  Created by Olaf Schleusing on 12/16/13.
//

#ifndef __utils__
#define __utils__


void printAuxInfo();
void plot(const float* x, const size_t len);

void matlabFFT(const float* in, const size_t szIn, const size_t fftLen, float** out);
void matlabIFFT(const float* in, size_t szIn, float** out);

#endif /* defined(__readConfig__utils__) */
