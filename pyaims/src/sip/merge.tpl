
class AimsMerge_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/merge.h>
#ifndef PYAIMSSIP_AimsMerge_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMSSIP_AimsMerge_%Template1typecode%_%Template2typecode%_DEFINED
  typedef AimsMerge<%Template1%,%Template2%>
    AimsMerge_%Template1typecode%_%Template2typecode%;
#endif
%End

public :
  AimsMerge_%Template1typecode%_%Template2typecode%(
    merge_t type, %Template1% value=0, %Template2% label=0 );
  virtual ~AimsMerge_%Template1typecode%_%Template2typecode%();
  AimsData_%Template1typecode% operator () (
    const AimsData_%Template1typecode% &data,
    const AimsData_%Template2typecode% &mask );
};

