class MaskLinearResampler_S16:Resampler_S16
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
public:

  MaskLinearResampler_S16();
  ~MaskLinearResampler_S16();

};
