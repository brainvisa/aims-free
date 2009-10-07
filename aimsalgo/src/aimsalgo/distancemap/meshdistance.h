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



#ifndef AIMS_DISTANCEMAP_MESHDISTANCE_H
#define AIMS_DISTANCEMAP_MESHDISTANCE_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <set>
#include <aims/data/data.h>

namespace aims
{

  namespace meshdistance
  {
    

    /** Compute the geodesic depth of a triangulation
	The vol define the object and the background
	The radius-* are morphological parameter */
    Texture<float> GeodesicDepth ( const AimsSurface<3,Void> & mesh, 
				   const AimsData <short> & vol ,
				   float radius_close, float radius_erode) ; 

    template<class T>
    Texture<float> MeshDistance( const AimsSurface<3,Void> &  mesh, 
				 const Texture<T> & inittex, 
				 bool allowUnreached );

    extern template Texture<float> 
    MeshDistance( const AimsSurface<3,Void> &, const Texture<short> &, bool );
    extern template Texture<float>
    MeshDistance( const AimsSurface<3,Void> &, const Texture<float> &, bool );
  }

}

#endif

