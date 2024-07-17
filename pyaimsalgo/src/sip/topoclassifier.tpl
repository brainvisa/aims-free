

class TopologicalClassifier_Volume_%Template1typecode%
{
%TypeHeaderCode
#include <aims/topology/topoClassifier.h>
#include <cartodata/volume/volume.h>

#ifndef PYAIMSALGOSIP_TOPOCLASSIFIER_VOLUME_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_TOPOCLASSIFIER_VOLUME_%Template1typecode%_DEFINED
typedef TopologicalClassifier<carto::VolumeRef<%Template1%> > TopologicalClassifier_Volume_%Template1typecode%;
#endif
%End

%Docstring
Topological classification of voxels (normally on a skeleton) for a whole volume
%End

public:
  TopologicalClassifier_Volume_%Template1typecode%();
  virtual ~TopologicalClassifier_Volume_%Template1typecode%();

  rc_ptr_Volume_%Template1typecode%
    doit( const rc_ptr_Volume_%Template1typecode% & );
%MethodCode
  carto::VolumeRef< %Template1% > res = sipCpp->doit( *a0 );
  sipRes = new carto::rc_ptr<carto::Volume< %Template1% > >( res.get() );
%End
};


class TopologicalClassification_%Template1typecode%
  : TopologicalClassificationBase
{
%TypeHeaderCode
#include <aims/topology/topoClassif.h>

#ifndef PYAIMSALGOSIP_TOPOCLASSIFFICATION_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_TOPOCLASSIFFICATION_%Template1typecode%_DEFINED
typedef TopologicalClassification<%Template1%> TopologicalClassification_%Template1typecode%;
#endif
%End

%Docstring
Topological classification per voxel
%End

public:

  TopologicalClassification_%Template1typecode% ( const rc_ptr_Volume_%Template1typecode% & );
  virtual ~TopologicalClassification_%Template1typecode%();
};
