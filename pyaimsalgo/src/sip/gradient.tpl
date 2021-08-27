class AimsGradient_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/math/gradient.h>
  #ifndef PYAIMSALGOSIP_AimsGradient_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_AimsGradient_%Template1typecode%_DEFINED
  typedef AimsGradient<%Template1%> AimsGradient_%Template1typecode%;
  #endif
%End

  public:
    AimsGradient_%Template1typecode%(int type=AIMS_GRADIENT_CENTRAL);

    rc_ptr_Volume_%Template1typecode% X(
      const rc_ptr_Volume_%Template1typecode% &data);
    rc_ptr_Volume_%Template1typecode% Y(
      const rc_ptr_Volume_%Template1typecode% &data);
    rc_ptr_Volume_%Template1typecode% Z(
      const rc_ptr_Volume_%Template1typecode% &data);
    rc_ptr_Volume_%Template1typecode% T(
      const rc_ptr_Volume_%Template1typecode% &data);
};
