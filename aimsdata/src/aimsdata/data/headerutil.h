#ifndef AIMSDATA_IO_HEADERUTIL_H
#define AIMSDATA_IO_HEADERUTIL_H

#include <cartobase/object/object.h>

namespace carto
{

  /** force or alter a specific voxels orientation on disk (if the output
  format supports it), by changing the internal "storage_to_memory" matrix,
  in a more or less convenient way, with different syntaxes:

  - "flipx yflip" : flips orientations of x and y axes.
  -"neuro", "neurological" or "left-to-right": force neuro orientation.
  - "radio", "radiological" or "right-to-left": force radio orientation.
  - "-1 1 -1": set a flip matrix flipping x and z axes (set the diagonal).
  - "-1 0 0  0 1 0  0 0 -1": same by specifying the full rotation matrix.
  - "-1 0 0 255  0 1 0 0  0 0 -1 123  0 0 0 1": same by specifying
    the full 4x4 matrix.

  By default, all orientation matrices are applied relatively to the current
  voxels orientation. To set absolute matrices (from the AIMS conventional
  orientation), specify "abs:" before the matrix. Ex:

  - "abs: -1 1 -1"
  - "abs: -1 0 0 255  0 1 0 0  0 0 -1 123  0 0 0 1"

  Absolute orientations can also be given in the 3 letters conventions ("RAS",
  "LPI", etc, with L: axis toward Left side, R: right, A: anterior, P:
  posterior, S: superior, I: inferior. Thus "RAS" is equivalent to "abs: -1 -1
  -1", "LPI" is equivalent to "abs: 1 1 1" etc.

  See: https://brainvisa.info/aimsdata/user_doc/coordinates_systems.html

  */
  void setOrientationInformation( carto::Object hdr,
                                  const std::string & orient );

}

#endif

