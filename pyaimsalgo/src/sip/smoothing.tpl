class Gaussian3DSmoothing_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/g3dsmooth.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsData_%Template1typecode%.h"
#endif

#ifndef PYAIMSALGOSIP_GAUSSIAN3DSMOOTHING_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_GAUSSIAN3DSMOOTHING_%Template1typecode%_DEFINED
	typedef Gaussian3DSmoothing<%Template1%> Gaussian3DSmoothing_%Template1typecode%;
#endif
%End

public:
  Gaussian3DSmoothing_%Template1typecode%(float sx=1.0, float sy=1.0,
    float sz=1.0);
  virtual ~Gaussian3DSmoothing_%Template1typecode%();

  AimsData_%Template1typecode% doit( const AimsData_%Template1typecode% &)
    /Factory, ReleaseGIL/;
};

