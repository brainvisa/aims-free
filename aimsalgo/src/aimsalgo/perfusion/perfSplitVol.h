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



#ifndef AIMS_PERFUSION_PERF_SPLIT_VOL_H
#define AIMS_PERFUSION_PERF_SPLIT_VOL_H

#include <aims/data/data.h>

namespace aims
{

  //! Perfusion class
  /*!
    This class provides functions to get a volume from a sequence of volumes.
  */
  template< class T >
  class PerfusionSplitVolume
  {
  public:

    //! Constructor
    PerfusionSplitVolume()  { }
    //! Destructor
    virtual ~PerfusionSplitVolume()  { }

    //! Get the first volume of the sequence
    /*!
      \param d is the input sequence of volume
      \return the first volume of the sequence
    */
    AimsData< T > first( AimsData< T >& d );
    //! Get the last volume of the sequence
    /*!
      \param d is the input sequence of volume
      \return the last volume of the sequence
    */
    AimsData< T > last( AimsData< T >& d );
    //! Get a volume of the sequence
    /*!
      \param d is the input sequence of volume
      \param t is the volume number in the sequence
      \return the selected volume of the sequence
    */
    AimsData< T > volume( AimsData< T >& d, int t );
  };


  template< class T >
  AimsData< T > PerfusionSplitVolume< T >::first( AimsData< T >& d )
  {
    return volume( d, 0 );
  }


  template< class T >
  AimsData< T > PerfusionSplitVolume< T >::last( AimsData< T >& d )
  {
    return volume( d, d.dimT() - 1 );
  }


  template< class T >
  AimsData< T > PerfusionSplitVolume< T >::volume( AimsData< T >& d, int t )
  {
    ASSERT( t >= 0 && t < d.dimT() );

    int dx = d.dimX();
    int dy = d.dimY();
    int dz = d.dimZ();

    AimsData< T > res( dx, dy, dz );
    res.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ() );

    int k, j, i;
    for ( k=0; k<dz; k++ )
      for ( j=0; j<dy; j++ )
	for ( i=0; i<dx; i++ )
	  res( i, j, k ) = d( i, j, k, t );

    return res;
  }

}

#endif
