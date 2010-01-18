class Histogram_%Template1typecode%
{
%TypeHeaderCode
#include <aims/histogram/histogram.h>
%Template1typeinclude%
#ifndef PYAIMS_HISTOGRAM_%Template1typecode%_DEFINED
#define PYAIMS_HISTOGRAM_%Template1typecode%_DEFINED
typedef Histogram<%Template1% > Histogram_%Template1typecode%;
#endif
%End

  public:
    Histogram_%Template1typecode%();
    Histogram_%Template1typecode%( const Histogram_%Template1typecode%
      & other );
    virtual ~Histogram_%Template1typecode%();

    AimsData_S32 & data();

    virtual void doit( const AimsData_%Template1typecode% & ) = 0;
};

namespace aims
{

  class RegularBinnedHistogram_%Template1typecode%
    : Histogram_%Template1typecode%
  {
%TypeHeaderCode
#include <aims/histogram/regularBinnedHisto.h>
%Template1typeinclude%
#ifndef PYAIMS_REGULARBINNEDHISTOGRAM_%Template1typecode%_DEFINED
#define PYAIMS_REGULARBINNEDHISTOGRAM_%Template1typecode%_DEFINED
namespace aims
{
  typedef RegularBinnedHistogram<%Template1% > RegularBinnedHistogram_%Template1typecode%;
}
#ifndef PYAIMS_HISTOGRAM_%Template1typecode%_DEFINED
#define PYAIMS_HISTOGRAM_%Template1typecode%_DEFINED
typedef Histogram<%Template1% > Histogram_%Template1typecode%;
#endif
#endif
%End

    public:
      RegularBinnedHistogram_%Template1typecode%( unsigned bins = 0 );
      RegularBinnedHistogram_%Template1typecode%( const aims::RegularBinnedHistogram_%Template1typecode% & other );
      virtual ~RegularBinnedHistogram_%Template1typecode%();

      void doit( const AimsData_%Template1typecode% & thing );
      void doit( const AimsData_%Template1typecode% & thing, %Template1% mini,
                 %Template1% maxi );
      unsigned bins() const;
      void setBins( unsigned bins );
      %Template1% minDataValue() const;
      %Template1% maxDataValue() const;
      vector_%Template1typecode% *unique( const AimsData_%Template1typecode%
        & thing ) const /Factory/;
  };

};

