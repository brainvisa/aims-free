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

    AimsData_%Template1typecode% X(const AimsData_%Template1typecode% &data);
    AimsData_%Template1typecode% Y(const AimsData_%Template1typecode% &data);
    AimsData_%Template1typecode% Z(const AimsData_%Template1typecode% &data);
    AimsData_%Template1typecode% T(const AimsData_%Template1typecode% &data);
};
