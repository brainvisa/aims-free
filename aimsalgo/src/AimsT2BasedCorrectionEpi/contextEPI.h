

#ifndef _AIMSCORRECTIONEPI_CONTEXTEPI_H_
#define _AIMSCORRECTIONEPI_CONTEXTEPI_H_


#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <cartobase/thread/loopContext.h>


class ContextEpi : public carto::LoopContext
{

  public:

    enum SmoothingType
    {

      DericheGaussian,
      DiscreteGaussian

    };

    ContextEpi( int numT2,
                int size, int sizeDivider, float inPlaneResolution,
                int levels, double ftol,
		const AimsVector< float, 3 >& deltaP,
                AimsData< float >& S,
                AimsData< float >& T0,
                AimsData< float >& T1,
		bool verbose = false );
    ContextEpi( int numT2,
                int size, int sizeDivider, float inPlaneResolution,
                int levels, double ftol,
		const AimsVector< float, 3 >& deltaP,
                AimsData< float >& S,
                AimsData< float >& T0,
                AimsData< float >& T1,
                carto::ThreadGauge* gauge, carto::ThreadCancel* cancel,
		bool verbose = false );
    ~ContextEpi();

    void setDericheGaussianSmoothing( float sigma );
    void setDiscreteGaussianSmoothing( int sizeMask );

    void doIt( int startIndex, int countIndex );

    void setLocation( short** t2Image, int location );

    bool verbose;

  protected:

    void init();

    int _numT2;

    int _realSize;
    float _realRes;
    int _procSize;
    float _procRes;

    int _sizeDivider;
    int _levels;
    double _ftol;
    AimsVector< float, 3 > _deltaP;

    AimsData< float >& _Sss;	//  _S seems to be a macro on solaris...
    AimsData< float >& _T0;
    AimsData< float >& _T1;

    short** _t2Images;

    short* _ref;
    short** _dis;
    short** _out;

    float** _p1;
    float** _p2;
    float** _p12linear;
    float*** _p12;

    SmoothingType _smoothingType;
    float _sigma;
    int _sizeMask;

    int _location;

};


#endif
