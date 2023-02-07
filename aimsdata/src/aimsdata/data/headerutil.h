/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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

