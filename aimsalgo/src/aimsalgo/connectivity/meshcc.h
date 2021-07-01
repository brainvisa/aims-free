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



#ifndef AIMS_MESH_CONNECTIVITY_MESHCC_H
#define AIMS_MESH_CONNECTIVITY_MESHCC_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <map>
#include <set>

///
AIMSALGO_API

/* threshold a texture and label resultiong connected components
   modes : 0 = above threshold, 1 = below threshold
*/
template<class T>
Texture<T> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                             const Texture<T> & inittex,
                             T threshold, int mode=1);
template<class T>
unsigned AimsMeshLabelNbConnectedComponent( const AimsSurface<3,Void> & mesh,
                           const Texture<T> & inittex, const T lab );
template<class T>
Texture<T> AimsMeshLabelConnectedComponent2Texture(
  const AimsSurface<3,Void> & mesh, const Texture<T> & inittex,  T threshold );
/** Split label "label" into connected components, then filter smaller ones
    out.
    Original values are left unchanged, except for filtered out regions which
    will be given the label "background" (0 by default).

    Filtering can keep the "ncomp" largest components, and suppress regions
    with fewer than "min_npts" vertices, and regions under the surface area
    "min_surf". If any of these criterions is 0, then filtering for this
    criterion doesn't happen.
*/
template<class T>
Texture<T> AimsMeshFilterConnectedComponent(
  const AimsSurface<3,Void> & mesh, const Texture<T> & inittex, T label,
  const T & background = 0, unsigned long ncomp = 1,
  unsigned long min_npts = 0, float min_surf = 0. );

/// hum, should not be there as a global, non-namespaced variable...
const int FORBIDDEN = -1;

#endif

