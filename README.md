mexPort
=======

mexPort helps you to speed up the creation of .mex files. 

1. It contains project templates for Xcode on OSX and Visual Studio on Windows enabling you to build and run your .mex file from Matlab within seconds. A Matlab script automates the steps to create a new project. You simply give it a name, build it and you are ready to go (i.e. you have a callable .mex file which does nothing at this moment).

2. mexPort comes with a single-header library called mexPort.h that simplifies converting Matlab data into proper C or C++ formats. For instance, to convert a 3-dimensional Matlab array (say randn(3,4,5)) you could type:

  float*** myArray(0); size_t* arrayDimensions(0);   
  importFromMex(prhs[0], &myArray, &arrayDimensions);

  or, if you prefer to use C++ STL containers and doubles:

  vector< vector< vector< double > > > myVector;   
  importFromMex(prhs[0], &myVector);
  
  mexPort.h can be used independently in your own project, without the project creation tools. Simply include the header file in your project and you are ready to go.


mexPort.h takes care of checking dimensions, works with all internal data types (e.g. float, int, short, etc.), memory management, row vs. column vectors, etc.

Please check the file mexPort.pdf in the doc folder for examples and more information.

You can contact me at olaf At schleusing Dot com.

Olaf
