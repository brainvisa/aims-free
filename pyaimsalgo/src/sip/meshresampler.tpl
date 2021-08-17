class MeshToVoxelsResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/meshToVoxelsResampler.h>
  #include <aims/data/data.h>
  %Template1typeinclude%
  %Template1sipinclude%

  #ifndef PYAIMSSIP_MESHTOIMARESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSSIP_MESHTOIMARESAMPLER_%Template1typecode%_DEFINED
  typedef aims::MeshToVoxelsResampler<%Template1% > MeshToVoxelsResampler_%Template1typecode%;
  #endif

 
%End
  public:
    MeshToVoxelsResampler_%Template1typecode%();
    virtual ~MeshToVoxelsResampler_%Template1typecode%();

  public:
    %Template1typecode% doit(const AimsSurfaceTriangle &surface,
	float spacing=1, unsigned int connexity=26) /ReleaseGIL/;

};
