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
#ifndef AIMS_INTERPOLATION_INTERPOLATEDVOLUME_D_H
#define AIMS_INTERPOLATION_INTERPOLATEDVOLUME_D_H

//--- aims -------------------------------------------------------------------
#include <aims/interpolation/interpolatedvolume.h>
#include <aims/signalfilter/splinefilter.h>
#include <aims/resampling/splineresampler.h> // aims::mirrorCoeff
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumeutil.h>            // carto::copy
#include <cartobase/type/string_conversion.h>       // carto::toString
//--- std --------------------------------------------------------------------
#include <cmath>                                    // std::min / max / floor
#include <stdexcept>                                // std::runtime_error
//----------------------------------------------------------------------------

namespace aims {

  //--------------------------------------------------------------------------
  //   Constructor / Destructor / Assignment
  //--------------------------------------------------------------------------

  template <typename T>
  InterpolatedVolume::InterpolatedVolume( const carto::Volume<T> & vol,
                                          unsigned order ):
    _order( order ),
    _spline( new FastBSpline( 3 ) )
  {
    setVolume( vol );
  }

  template <typename T>
  InterpolatedVolume::InterpolatedVolume( const carto::VolumeRef<T> & vol,
                                          unsigned order ):
    _order( order ),
    _spline( new FastBSpline( 3 ) )
  {
    setVolume( vol );
  }

  template <typename T>
  InterpolatedVolume::InterpolatedVolume( const AimsData<T> & vol,
                                          unsigned order ):
    _order( order ),
    _spline( new FastBSpline( 3 ) )
  {
    setVolume( vol );
  }


  //--------------------------------------------------------------------------
  //   Change interpolated volume
  //--------------------------------------------------------------------------

  template <typename T>
  void InterpolatedVolume::setVolume( const AimsData<T> & vol, int order )
  {
    if( order >= 0 )
      _order = order;

    InverseBSplineFilter f( _order );
    _coeff = f.execute( carto::VolumeRef<T>( vol.volume() ) );
  }

  template <typename T>
  void InterpolatedVolume::setVolume( const carto::Volume<T> & vol, int order )
  {
    AimsData<T> rvol( carto::VolumeRef<T>( new carto::Volume<T>( vol ) ) );
    setVolume( rvol, order );
  }

  template <typename T>
  void InterpolatedVolume::setVolume( const carto::VolumeRef<T> & vol, int order )
  {
    AimsData<T> rvol( vol );
    setVolume( rvol, order );
  }

  //--------------------------------------------------------------------------
  //   Change coefficient volume
  //--------------------------------------------------------------------------

  template <typename T>
  void InterpolatedVolume::setCoeff( const carto::VolumeRef<T> & vol, int order )
  {
    if( order >= 0 )
      _order = order;

    _coeff = carto::copy<double,T>( vol );
  }

  template <typename T>
  void InterpolatedVolume::setCoeff( const carto::Volume<T> & vol, int order )
  {
    carto::VolumeRef<T> rvol( new carto::Volume<T>( vol ) );
    setCoeff( rvol, order );
  }

  template <typename T>
  void InterpolatedVolume::setCoeff( const AimsData<T> & vol, int order )
  {
    carto::VolumeRef<T> rvol( vol.volume() );
    setCoeff( rvol, order );
  }



} // namespace aims

#endif // AIMS_INTERPOLATION_INTERPOLATEDVOLUME_D_H
