

#ifndef _AIMSCORRECTIONEPI_MUTUALINFO_H_
#define _AIMSCORRECTIONEPI_MUTUALINFO_H_


class Smoother;

// in pdf.cc

int* getMaskOffset( int sizeMask, int levels );

float* getGaussianMask2d( int sizeMask );

void getWindowParzenJointPdf( const short* data1, const short* data2,
                              int size,
                              float** p12, float* p1, float* p2, int levels,
                              Smoother* smoother );


// in mutualInfo.cxx

float mutualInformation( const float* p1,
                         const float* p2,
                         float** p12,
                         int levels );


#endif
