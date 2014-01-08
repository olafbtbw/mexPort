//
//  cppEntry.cpp
//  entry point for importing and exporting your data between
//  Matlab mxArrays and your C/C++ data types
//
//  Created by Olaf Schleusing on 05/16/12.
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

#include "mexPort.h"
#include "cppEntry.h"
#include "mex.h"

#include <vector>
#include <string>

extern "C" {
#include "utils.h"
}

using namespace MexPort;


void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
        
    printAuxInfo(); // switch this on or off in utils.cpp
    
    // ## 1 ## safety net: insert number of Matlab parameters here
	if (0 == nrhs)	{
        
        // ## 2 ## prepare variables to receive the Matlab parameters
        //
        // Examples
        // C++
//        std::vector<std::vector<float> > in1;// a 2-D Matlab array
//        importFromMex(prhs[0], &in1);
//        int a = importFromMex<int>(prhs[1]);
//        std::string cppStr;
//        portStringFromMex(prhs[2], &cppStr);

        // C
//        float*** in2(0); size_t* sizeIn2(0U); // a 3-D Matlab array
//        importFromMex(prhs[3], &in2, &sizeIn2);

        // ## 3 ## declare output variables
//        std::vector<float> res1;
//        float* res2(0); size_t* sizeRes2(0);

        // ## 4 ## implement your algorithm (probably better in a dedicated C or C++ file

        // ## 5 ## safety net: insert number of Matlab return values here and return data
        if (0 == nlhs) {
//            exportToMex(res1, &plhs[0]); // C++
//            exportToMex(res2, sizeRes2, &plhs[1]); // C
        }
        else {
            mexWarnMsgTxt("Wrong number of output parameters. Aborting.");
        }

        // ## 6 ## don't forget to delete your C arrays
//        freeNDimArray(in2, sizeIn2); delete sizeIn2;
//        freeNDimArray(res2, sizeRes2); delete sizeRes2;
	}
    else {
        mexWarnMsgTxt("Wrong number of input parameters. Aborting.");
    }
}
