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


#ifndef AIMS_DISTANCEMAP_DANIELSSON_H
#define AIMS_DISTANCEMAP_DANIELSSON_H

#include <aims/config/aimsalgo_config.h>

template <class T> class AimsData;

enum DanielssonSide
{
  AIMS_DANIELSSON_OUTSIDE,
  AIMS_DANIELSSON_INSIDE
};

/**@name Danielsson distance maps.
    Two distance maps can be processed : the distance map of the outside
    of the object (give AIMS_DANIELSSON_OUTSIDE argument), and the distance map
    of the inside of the object (give the AIMS_DANIELSSON_INSIDE argument).
*/
//@{
/// Return float euclidean Danielsson distance map
AIMSALGO_API AimsData<float>
AimsDanielssonDistanceMap(const AimsData<int16_t> &vol,
			  DanielssonSide side=AIMS_DANIELSSON_OUTSIDE);
/// Return float euclidean signed distance map (minus out, plus in object)
AIMSALGO_API AimsData<float> 
AimsSignedDanielssonDistanceMap(const AimsData<int16_t> &vol);
//@}

#endif
