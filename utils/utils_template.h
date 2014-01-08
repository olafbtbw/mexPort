//
//  utils.h
//
//  Created by Olaf Schleusing on 12/16/13.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef __utils__
#define __utils__


void printAuxInfo();
void plot(const float* x, const size_t len);

void matlabFFT(const float* in, const size_t szIn, const size_t fftLen, float** out);
void matlabIFFT(const float* in, size_t szIn, float** out);

#endif /* defined(__readConfig__utils__) */
