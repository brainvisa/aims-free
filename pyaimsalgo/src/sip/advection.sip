namespace aims
{

class GradientAdvection
{
%TypeHeaderCode
  #include <aims/distancemap/advection.h>
%End
public:
  GradientAdvection();
  ~GradientAdvection();

  void setVerbose();

  void doit(const rc_ptr_Volume_S16 &vol, const rc_ptr_Volume_FLOAT &gradX,
	const rc_ptr_Volume_FLOAT &gradY, const rc_ptr_Volume_FLOAT &gradZ,
	const set_S16 & seedlabels) /ReleaseGIL/;
  void doit(const rc_ptr_Volume_S16 &vol, const rc_ptr_Volume_FLOAT &gradX,
	const rc_ptr_Volume_FLOAT &gradY, const rc_ptr_Volume_FLOAT &gradZ,
	int seedlabel) /ReleaseGIL/;
};

};
