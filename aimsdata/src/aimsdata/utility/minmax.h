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

#ifndef AIMS_UTILITY_MINMAX_H
#define AIMS_UTILITY_MINMAX_H

#include <aims/data/data.h>

namespace aims
{

  //-----------------------------------//
 //  minmax< ObjectType, ValueType >  //
//-----------------------------------//
template <typename ObjectType, typename ValueType>
void minmax( const ObjectType &, ValueType &min, ValueType &max );

//-----------------------------------------------------------------------------
template <typename ObjectType, typename ValueType>
void minmax( const ObjectType &, ValueType &min, ValueType &max )
{
  ObjectType::error_minmax_not_specialized;
}


  //--------------------------------------------//
 //  minmax< AimsData<ValueType>, ValueType >  //
//--------------------------------------------//
template <typename ValueType>
void minmax( const AimsData<ValueType> &volume, ValueType &min, 
             ValueType &max )
{
  typename AimsData<ValueType>::const_iterator it = 
    volume.begin() + volume.oFirstPoint();

  min = max = *it;
  for ( int t = 0; t < volume.dimT(); t++ ) {
    for ( int z = 0; z < volume.dimZ(); z++ ) {
      for ( int y = 0; y < volume.dimY(); y++ ) {
        for ( int x = 0; x < volume.dimX(); x++ ) {
          if ( *it < min ) min = *it;
          if ( *it > max ) max = *it;
          it++;
        }
        it += volume.oPointBetweenLine();
      }
      it += volume.oLineBetweenSlice();
    }
    it += volume.oSliceBetweenVolume();
  }
}


} // namespace aims

#endif // ifndef AIMS_UTILITY_MINMAX_H
