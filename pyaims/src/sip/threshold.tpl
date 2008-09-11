%ModuleHeaderCode
#include <aims/utility/threshold.h>
%End


class AimsThreshold_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/threshold.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipAimsData_%Template1typecode%.h"
#include "sipaimssipAimsData_%Template2typecode%.h"
#endif

#ifndef PYAIMSSIP_AimsThreshold_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSSIP_AimsThreshold_%Template1typecode%_%Template2typecode%_DEFINED
	typedef AimsThreshold<%Template1%,%Template2%> AimsThreshold_%Template1typecode%_%Template2typecode%;
#endif
%End

public:
  AimsThreshold_%Template1typecode%_%Template2typecode%(threshold_t, %Template1%, %Template1%=0, %Template1%=0);
  virtual ~AimsThreshold_%Template1typecode%_%Template2typecode%();

  AimsData_%Template1typecode% operator ()(const AimsData_%Template1typecode% &) /Factory/;
  AimsData_%Template2typecode% bin(const AimsData_%Template1typecode% &) /Factory/;
};
