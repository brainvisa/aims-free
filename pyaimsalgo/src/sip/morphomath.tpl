

class MorphoGreyLevel_%Template1typecode%
{
%TypeHeaderCode
#include <aims/morphology/operatormorpho.h>

#ifndef PYAIMSALGOSIP_MORPHOGREYLEVEL_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_MORPHOGREYLEVEL_%Template1typecode%_DEFINED
        typedef aims::MorphoGreyLevel<%Template1% > MorphoGreyLevel_%Template1typecode%;
#endif
%End

public:
  MorphoGreyLevel_%Template1typecode%();
  virtual ~MorphoGreyLevel_%Template1typecode%();

  rc_ptr_Volume_%Template1typecode%
    doErosion( const Volume_%Template1typecode% & dataIn, float radius );
  rc_ptr_Volume_%Template1typecode%
    doDilation( const Volume_%Template1typecode% & dataIn, float radius );
  rc_ptr_Volume_%Template1typecode%
    doClosing( const Volume_%Template1typecode% & dataIn, float radius );
  rc_ptr_Volume_%Template1typecode%
    doOpening( const Volume_%Template1typecode% & dataIn, float radius );
};

