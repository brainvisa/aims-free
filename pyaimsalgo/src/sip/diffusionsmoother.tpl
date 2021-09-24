
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
  rc_ptr_Volume_%Template1typecode%
    doSmoothing( const rc_ptr_Volume_%Template1typecode% & ima, int maxiter,
                 bool verbose=0 ) /ReleaseGIL/;

  bool optimal();
  void setConstantSources( const rc_ptr_Volume_%Template1typecode% &,
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

  rc_ptr_Volume_%Template1typecode% doSmoothing(
        const rc_ptr_Volume_%Template1typecode% & ima,
        int maxiter, bool verbose=false);

	void add_neumann_condition(const Point3df &p);

	void setMask( const rc_ptr_Volume_S16 & mask, short background=0);
};
