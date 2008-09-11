namespace aimsalgo
{

class Samplable_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/resampling/samplable.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsVector_%Template1typecode%_%Template2typecode%.h"
#endif

namespace aimsalgo
{
#ifndef PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::Samplable<%Template1%,%Template2%> Samplable_%Template1typecode%_%Template2typecode%;
#endif
}

%End

public:
  Samplable_%Template1typecode%_%Template2typecode%();
  virtual ~Samplable_%Template1typecode%_%Template2typecode%();

  virtual bool contains(AimsVector_%Template1typecode%_%Template2typecode% &)
    = 0 /ReleaseGIL/;
};


class GeneralSampler_%Template1typecode%_%Template2typecode% /Abstract/
{
%TypeHeaderCode
#include <aims/resampling/generalsampler.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsVector_%Template1typecode%_%Template2typecode%.h"
#include "sipaimsalgosipaimsalgoSamplable_%Template1typecode%_%Template2typecode%.h"
#endif

namespace aimsalgo
{
#ifndef PYAIMSALGOSIP_GENERALSAMPLER_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_GENERALSAMPLER_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::GeneralSampler<%Template1%,%Template2%> GeneralSampler_%Template1typecode%_%Template2typecode%;
#endif

#ifndef PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::Samplable<%Template1%,%Template2%> Samplable_%Template1typecode%_%Template2typecode%;
#endif

}

%End

public:
  GeneralSampler_%Template1typecode%_%Template2typecode%( );
  virtual ~GeneralSampler_%Template1typecode%_%Template2typecode%();

  virtual carto::Object sample(
    aimsalgo::Samplable_%Template1typecode%_%Template2typecode% &,
    AimsVector_%Template1typecode%_%Template2typecode% &,
    AimsVector_%Template1typecode%_%Template2typecode% &,
    AimsVector_%Template1typecode%_%Template2typecode% & ) = 0 /ReleaseGIL/;
};


class BucketMapSampler_%Template1typecode%_%Template2typecode%:aimsalgo::GeneralSampler_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/resampling/bucketmapsampler.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsVector_%Template1typecode%_%Template2typecode%.h"
#include "sipaimsalgosipaimsalgoSamplable_%Template1typecode%_%Template2typecode%.h"
#include "sipaimsalgosipaimsalgoGeneralSampler_%Template1typecode%_%Template2typecode%.h"
#include "sipaimssipBucketMap_VOID.h"
#endif

namespace aimsalgo
{

#ifndef PYAIMSALGOSIP_BUCKETMAPSAMPLER_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_BUCKETMAPSAMPLER_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::BucketMapSampler<%Template1%,%Template2%> BucketMapSampler_%Template1typecode%_%Template2typecode%;
#endif

#ifndef PYAIMSALGOSIP_GENERALSAMPLER_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_GENERALSAMPLER_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::GeneralSampler<%Template1%,%Template2%> GeneralSampler_%Template1typecode%_%Template2typecode%;
#endif

#ifndef PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSALGOSIP_SAMPLABLE_%Template1typecode%_%Template2typecode%_DEFINED
	typedef aimsalgo::Samplable<%Template1%,%Template2%> Samplable_%Template1typecode%_%Template2typecode%;
#endif

}

%End

public:
  BucketMapSampler_%Template1typecode%_%Template2typecode%( );
  virtual ~BucketMapSampler_%Template1typecode%_%Template2typecode%();

  virtual carto::Object sample(
    aimsalgo::Samplable_%Template1typecode%_%Template2typecode% &,
    AimsVector_%Template1typecode%_%Template2typecode% &,
    AimsVector_%Template1typecode%_%Template2typecode% &,
    AimsVector_%Template1typecode%_%Template2typecode% & ) /ReleaseGIL/;
};

};

