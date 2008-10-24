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

  void doit(const rc_ptr_Volume_S16 &vol, const AimsData_FLOAT &gradX,
	const AimsData_FLOAT &gradY, const AimsData_FLOAT &gradZ,
	const set_S16 & seedlabels) /ReleaseGIL/;
  void doit(const rc_ptr_Volume_S16 &vol, const AimsData_FLOAT &gradX,
	const AimsData_FLOAT &gradY, const AimsData_FLOAT &gradZ,
	int seedlabel) /ReleaseGIL/;
};

};
