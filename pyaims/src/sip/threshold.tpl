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
%Docstring
  AimsThreshold_%Template1typecode%_%Template2typecode%(ttype, th1, th2=0,
                                                        background=0)

  Thresholding types are specified as the soma.aims.threshold_t enum.
  Values are:

  * AIMS_LOWER_THAN
  * AIMS_LOWER_OR_EQUAL_TO
  * AIMS_GREATER_THAN
  * AIMS_GREATER_OR_EQUAL_TO
  * AIMS_EQUAL_TO
  * AIMS_DIFFER
  * AIMS_BETWEEN
  * AIMS_OUTSIDE
  * AIMS_BETWEEN_EXCLUDE_LOWER_BOUND
  * AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND
  * AIMS_BETWEEN_EXCLUDE_BOUNDS
  * AIMS_OUTSIDE_INCLUDE_LOWER_BOUND
  * AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND
  * AIMS_OUTSIDE_INCLUDE_BOUNDS

  Use::

      thresholded_vol = threshold(volume)

  Apply non-binary thresholding: voxels passing the theshold are left with
  their initial value.

  ::

      thresholded_vol = threshold.bin(volume)

  Apply binary thresholding: voxels passing the theshold are set to a constant
  value -- which one ?

  Parameters
  ----------
  ttype: soma.aims.threshold_t enum
      thresholding type (see above)
  th1: %Template1%
      first (main) threshold
  th2: %Template1%
      second threshold (in case of double thresholding)
  background: %Template1%
      value set to elements not passing the threshold condition



  Parameters
  ----------
  volume: AimsData_%Template2typecode%
      input volume to be thresholded

  Returns
  -------
  thresholded_vol: AimsData_%Template1typecode%
      thresholded volume
%End

  virtual ~AimsThreshold_%Template1typecode%_%Template2typecode%();

  AimsData_%Template1typecode% operator ()(const AimsData_%Template1typecode% &) /Factory/;
%Docstring
  thresholded_vol = threshold(volume)

  Apply non-binary thresholding: voxels passing the theshold are left with
  their initial value.

  Parameters
  ----------
  volume: AimsData_%Template2typecode%
      input volume to be thresholded

  Returns
  -------
  thresholded_vol: AimsData_%Template1typecode%
      thresholded volume
%End

  AimsData_%Template2typecode% bin(const AimsData_%Template1typecode% &) /Factory/;
%Docstring
  thresholded_vol = threshold.bin(volume)

  Apply binary thresholding: voxels passing the theshold are set to a constant
  value -- which one ?

  Parameters
  ----------
  volume: AimsData_%Template1typecode%
      input volume to be thresholded

  Returns
  -------
  thresholded_vol: AimsData_%Template2typecode%
      thresholded volume
%End
};
