
class DiffusionSmoother_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/diffusionSmoother.h>
#include <pyaims/data/data.h>
typedef aims::DiffusionSmoother<%Template1%>
  DiffusionSmoother_%Template1typecode%;
%End

public:
  DiffusionSmoother_%Template1typecode%(float delta_t);
  void SetDt(float Delta_t);
  float dt();
  AimsData_%Template1typecode%
    doSmoothing(const AimsData_%Template1typecode% & ima, int maxiter,
						bool verbose=0) /ReleaseGIL/;
  bool optimal();
  void setConstantSources( const AimsData_%Template1typecode% &,
    const %Template1PyType% & background ) /ReleaseGIL/;
  void removeConstantSources() /ReleaseGIL/;
};

