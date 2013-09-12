
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

class MaskedDiffusionSmoother_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/diffusionSmoother.h>
#include <pyaims/data/data.h>
typedef aims::MaskedDiffusionSmoother<%Template1%>
  MaskedDiffusionSmoother_%Template1typecode%;
%End

public:
	MaskedDiffusionSmoother_%Template1typecode%(float delta_t, bool safe=true);
	~MaskedDiffusionSmoother_%Template1typecode%();

	virtual AimsData_%Template1typecode% doSmoothing(
        const AimsData_%Template1typecode% & ima,
        int maxiter, bool verbose=false);
	void add_neumann_condition(const Point3df &p);

        // Known bug: when the mask is converted on the fly to AimsData in
        // setMask, it is destroyed as soon as setMask returns and the invalid
        // pointer kept by the underlying C++ creates chaos (segfault).
	void setMask( AimsData_S16 * mask, short background=0);
};
