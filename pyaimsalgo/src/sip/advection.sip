namespace aims
{

class GradientAdvection
{
%TypeHeaderCode
  #include <aims/distancemap/advection.h>
%End
public:
  GradientAdvection();
  virtual ~GradientAdvection();

  void setVerbose(bool = true);
  void setPropagationThreshold(float);

  rc_ptr_Volume_S16 doit(const rc_ptr_Volume_S16 &label_vol,
                         const rc_ptr_Volume_FLOAT &gradX,
                         const rc_ptr_Volume_FLOAT &gradY,
                         const rc_ptr_Volume_FLOAT &gradZ,
                         int seedlabel) /ReleaseGIL/;

  rc_ptr_Volume_S16 propagate_all_labels(
    const rc_ptr_Volume_S16 &seeds,
    const rc_ptr_Volume_FLOAT &fieldx,
    const rc_ptr_Volume_FLOAT &fieldy,
    const rc_ptr_Volume_FLOAT &fieldz) /ReleaseGIL/;

  rc_ptr_Volume_S16 descend_scalar_field(
    const rc_ptr_Volume_S16 &seeds,
    const rc_ptr_Volume_FLOAT &field) /ReleaseGIL/;
};

};
