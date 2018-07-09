class MaskLinearResampler_S16 : Resampler_S16
{
%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/masklinresampler.h>

  #ifndef PYAIMSALGOSIP_RESAMPLER_S16_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_S16_DEFINED
  typedef aims::Resampler<int16_t> Resampler_S16;
  #endif

  #ifndef PYAIMSALGOSIP_MASKLINEARRESAMPLER_S16_DEFINED
  #define PYAIMSALGOSIP_MASKLINEARRESAMPLER_S16_DEFINED
  typedef aims::MaskLinearResampler<int16_t> MaskLinearResampler_S16;
  #endif

  #ifndef PYAIMSSIP_AIMSDATA_S16_DEFINED
  #define PYAIMSSIP_AIMSDATA_S16_DEFINED
  typedef AimsData<int16_t> AimsData_S16;
  #endif

%End
%Docstring
Volume resampler using linear (order 1) interpolation.

This resampler will consider input voxels that are equal to -32768
(hard-coded) as masked. The mask value (-32768) will always be returned for
any interpolation involving a masked voxel.

The default background value for this resampler is -32768 (same as the mask
value).

The resampling API is described in the base class, Resampler_S16.
%End
public:

  MaskLinearResampler_S16();
  ~MaskLinearResampler_S16();

};
