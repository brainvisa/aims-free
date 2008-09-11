namespace aimsalgo
{

class Polynomial_%Template1typecode%_%Template2typecode%:aimsalgo::Samplable_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/resampling/polynomial.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipvector_%Template1typecode%.h"
#include "sipaimsalgosipaimsalgoSamplable_%Template1typecode%_%Template2typecode%.h"
#endif

namespace aimsalgo
{
#ifndef PYAIMSALGOSIP_POLYNOMIAL_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_POLYNOMIAL_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::Polynomial<%Template1%,%Template2%> Polynomial_%Template1typecode%_%Template2typecode%;
#endif
}

%End

%TypeCode
#if SIP_VERSION >= 0x040700
namespace aimsalgo
{
#ifndef PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
        typedef Samplable<%Template1%,%Template2%> Samplable_%Template1typecode%_%Template2typecode%;
#endif
}
#endif
%End


public:

  Polynomial_%Template1typecode%_%Template2typecode%(vector_FLOAT * /Transfer/,
    %Template1% = 1);
  virtual ~Polynomial_%Template1typecode%_%Template2typecode%();

  virtual void setCoefficients(vector_FLOAT * /Transfer/);
  virtual vector_FLOAT * getCoefficients();
  virtual void setOrderStep(%Template1%);
  virtual %Template1% getOrderStep();
  virtual bool contains(AimsVector_%Template1typecode%_%Template2typecode% &)
    /ReleaseGIL/;
  virtual %Template1% resolve(
    AimsVector_%Template1typecode%_%Template2typecode% &) /ReleaseGIL/;
  virtual void displayEquation();
};

};
