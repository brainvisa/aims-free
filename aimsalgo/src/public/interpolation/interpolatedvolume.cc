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

#include <aims/interpolation/interpolatedvolume_d.h>

namespace aims {

  //--------------------------------------------------------------------------
  //   Constructor / Destructor / Assignment
  //--------------------------------------------------------------------------

  InterpolatedVolume::InterpolatedVolume():
    _order( 3 ),
    _spline( new TabulBSpline( 3, 3 ) ),
    _coeff( (carto::Volume<double> *)0 )
  {}

  InterpolatedVolume::InterpolatedVolume( const InterpolatedVolume & other ):
    _order( other._order ),
    _spline( other._spline ),
    _coeff( other._coeff )
  {}

  InterpolatedVolume::~InterpolatedVolume()
  {}

  InterpolatedVolume &
  InterpolatedVolume::operator=( const InterpolatedVolume & other )
  {
    if( this != &other ) {
      _order = other._order;
      _spline = other._spline;
      _coeff = other._coeff;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   Interpolation
  //--------------------------------------------------------------------------

  double InterpolatedVolume::at( double x, double y,
                                 double z, long t ) const
  {
    double half = (_order + 1) / 2.;
    Point3dl low( (long)std::ceil(x - half),
                  (long)std::ceil(y - half),
                  (long)std::ceil(z - half) );
    Point3dl upp( (long)std::floor(x + half),
                  (long)std::floor(y + half),
                  (long)std::floor(z + half) );

    // 2d cases
    bool is2d = _coeff.getSizeZ() == 1;
    low[2] = ( is2d ? 0 : low[2] );
    upp[2] = ( is2d ? 0 : upp[2] );

    // compute
    double bk, bj, bi;
    double ck, cj, ci;
    bk = 0.;
    for( long k = low[2]; k <= upp[2]; ++k )
    {
      ck = aims::mirrorCoeff( k, _coeff.getSizeZ() );
      bj = 0.;
      for( long j = low[1]; j <= upp[1]; ++j )
      {
        cj = aims::mirrorCoeff( j, _coeff.getSizeY() );
        bi = 0.;
        for( long i = low[0]; i <= upp[0]; ++i )
        {
          ci = aims::mirrorCoeff( i, _coeff.getSizeX() );
          bi += _coeff(ci, cj, ck, t) * _spline->at(x - i);
        }
        bj += _spline->at(y - j) * bi;
      }
      bk += ( is2d ? 1. :  _spline->at(z - k) ) * bj;
    }

    return bk;
  }

  InterpolatedVolume::Vector
  InterpolatedVolume::derivative( double x, double y,
                                  double z, long t ) const
  {
    double half = (_order + 1) / 2.;
    Point3dl low( (long)std::ceil(x - half),
                  (long)std::ceil(y - half),
                  (long)std::ceil(z - half) );
    Point3dl upp( (long)std::floor(x + half),
                  (long)std::floor(y + half),
                  (long)std::floor(z + half) );

    // 2d cases
    bool is2d = _coeff.getSizeZ() == 1;
    low[2] = ( is2d ? 0 : low[2] );
    upp[2] = ( is2d ? 0 : upp[2] );

    // compute
    double dbk, dbj, dbi;
    double bk, bj, bi;
    double ck, cj, ci;
    double coeff;
    Point3dd value(0., 0., 0.);
    for( long k = low[2]; k <= upp[2]; ++k )
    {
      dbk = ( is2d ? 0. : _spline->derivative(z - k) );
      bk = ( is2d ? 1. : _spline->at(z - k) );
      ck = aims::mirrorCoeff( k, _coeff.getSizeZ() );
      for( long j = low[1]; j <= upp[1]; ++j )
      {
        dbj = _spline->derivative(y - j);
        bj = _spline->at(y - j);
        cj = aims::mirrorCoeff( j, _coeff.getSizeY() );
        for( long i = low[0]; i <= upp[0]; ++i )
        {
          dbi = _spline->derivative(x - i);
          bi = _spline->at(x - i);
          ci = aims::mirrorCoeff( i, _coeff.getSizeX() );
          coeff = _coeff(ci, cj, ck, t);

          value[0] += coeff * dbi * bj  * bk;
          value[1] += coeff * bi  * dbj * bk;
          value[2] += coeff * bi  * bj  * dbk;
        }
      }
    }

    return value;
  }

  InterpolatedVolume::Matrix
  InterpolatedVolume::nderivative( unsigned n, double x, double y,
                                   double z, long t ) const
  {
    ASSERT( n <= _order && n <= 4 ); //< else we cannot store it in a volume

    carto::VolumeRef<double> value;
    if( n == 1 )
      value->reallocate(3);
    else if( n == 2 )
      value->reallocate(3, 3);
    else if( n == 3 )
      value->reallocate(3, 3, 3);
    else if( n == 4 )
      value->reallocate(3, 3, 3, 3);
    value->fill(0);

    carto::VolumeRef<double> b;
    if( n == 1 )
      b->reallocate(3, 2);
    else if( n == 2 )
      b->reallocate(3, 3);
    else if( n == 3 )
      b->reallocate(3, 4);
    else if( n == 4 )
      b->reallocate(3, 5);
    b->fill(0.);
    b(0) = 1.;

    double half = (_order + 1) / 2.;
    Point3dl low( (long)std::ceil(x - half),
                  (long)std::ceil(y - half),
                  (long)std::ceil(z - half) );
    Point3dl upp( (long)std::floor(x + half),
                  (long)std::floor(y + half),
                  (long)std::floor(z + half) );

    // 2d cases
    bool is2d = _coeff.getSizeZ() == 1;
    low[2] = ( is2d ? 0 : low[2] );
    upp[2] = ( is2d ? 0 : upp[2] );

    // compute
    double ck, cj, ci;
    double coeff;
    for( long k = low[2]; k <= upp[2]; ++k )
    {
      ck = aims::mirrorCoeff( k, _coeff.getSizeZ() );
      if( !is2d )
      {
        for( int d = 0; d < b.getSizeX(); ++d )
          b(2, d) = _spline->derivative(z - k, d);
      }
      for( long j = low[1]; j <= upp[1]; ++j )
      {
        cj = aims::mirrorCoeff( j, _coeff.getSizeY() );
        for( int d = 0; d < b.getSizeX(); ++d )
          b(1, d) = _spline->derivative(y - j, d);
        for( long i = low[0]; i <= upp[0]; ++i )
        {
          ci = aims::mirrorCoeff( i, _coeff.getSizeX() );
          for( int d = 0; d < b.getSizeX(); ++d )
            b(0, d) = _spline->derivative(x - i, d);
          coeff = _coeff(ci, cj, ck, t);

          if( n == 1 )
          {
            value(0) += coeff * b(0, 1) * b(1, 0) * b(2, 0);
            value(1) += coeff * b(0, 0) * b(1, 1) * b(2, 0);
            value(2) += coeff * b(0, 0) * b(1, 0) * b(2, 1);
          }
          else if( n == 2 )
          {
            value(0,0) += coeff * b(0, 2) * b(1, 0) * b(2, 0);
            value(0,1) += coeff * b(0, 1) * b(1, 1) * b(2, 0);
            value(0,2) += coeff * b(0, 1) * b(1, 0) * b(2, 1);

            value(1,0) += coeff * b(0, 1) * b(1, 1) * b(2, 0);
            value(1,1) += coeff * b(0, 0) * b(1, 2) * b(2, 0);
            value(1,2) += coeff * b(0, 0) * b(1, 1) * b(2, 1);

            value(2,0) += coeff * b(0, 1) * b(1, 0) * b(2, 1);
            value(2,1) += coeff * b(0, 0) * b(1, 1) * b(2, 1);
            value(2,2) += coeff * b(0, 0) * b(1, 0) * b(2, 2);
          }
          else if( n == 3 )
          {
            value(0,0,0) += coeff * b(0, 3) * b(1, 0) * b(2, 0);
            value(0,0,1) += coeff * b(0, 2) * b(1, 1) * b(2, 0);
            value(0,0,2) += coeff * b(0, 2) * b(1, 0) * b(2, 1);

            value(0,1,0) += coeff * b(0, 2) * b(1, 1) * b(2, 0);
            value(0,1,1) += coeff * b(0, 1) * b(1, 2) * b(2, 0);
            value(0,1,2) += coeff * b(0, 1) * b(1, 1) * b(2, 1);

            value(0,2,0) += coeff * b(0, 2) * b(1, 0) * b(2, 1);
            value(0,2,1) += coeff * b(0, 1) * b(1, 1) * b(2, 1);
            value(0,2,2) += coeff * b(0, 1) * b(1, 0) * b(2, 2);

            value(1,0,0) += coeff * b(0, 2) * b(1, 1) * b(2, 0);
            value(1,0,1) += coeff * b(0, 1) * b(1, 2) * b(2, 0);
            value(1,0,2) += coeff * b(0, 1) * b(1, 1) * b(2, 1);

            value(1,1,0) += coeff * b(0, 1) * b(1, 2) * b(2, 0);
            value(1,1,1) += coeff * b(0, 0) * b(1, 3) * b(2, 0);
            value(1,1,2) += coeff * b(0, 0) * b(1, 2) * b(2, 1);

            value(1,2,0) += coeff * b(0, 1) * b(1, 1) * b(2, 1);
            value(1,2,1) += coeff * b(0, 0) * b(1, 2) * b(2, 1);
            value(1,2,2) += coeff * b(0, 0) * b(1, 1) * b(2, 2);

            value(2,0,0) += coeff * b(0, 2) * b(1, 0) * b(2, 1);
            value(2,0,1) += coeff * b(0, 1) * b(1, 1) * b(2, 1);
            value(2,0,2) += coeff * b(0, 1) * b(1, 0) * b(2, 2);

            value(2,1,0) += coeff * b(0, 1) * b(1, 1) * b(2, 1);
            value(2,1,1) += coeff * b(0, 0) * b(1, 2) * b(2, 1);
            value(2,1,2) += coeff * b(0, 0) * b(1, 1) * b(2, 2);

            value(2,2,0) += coeff * b(0, 1) * b(1, 0) * b(2, 2);
            value(2,2,1) += coeff * b(0, 0) * b(1, 1) * b(2, 2);
            value(2,2,2) += coeff * b(0, 0) * b(1, 0) * b(2, 3);
          }
          else if( n == 4 )
            throw std::runtime_error("not implemented");

        }
      }
    }

    return value;
  }

  // ALIAS

  double
  InterpolatedVolume::operator() ( double x, double y, double z, long t ) const
  {
    return at(x, y, z, t);
  }

  InterpolatedVolume::Vector
  InterpolatedVolume::der( double x, double y, double z, long t ) const
  {
    return derivative(x, y, z, t);
  }

  InterpolatedVolume::Matrix
  InterpolatedVolume::der2( double x, double y, double z, long t ) const
  {
    return nderivative(2, x, y, z, t);
  }

  InterpolatedVolume::Matrix
  InterpolatedVolume::der3( double x, double y, double z, long t ) const
  {
    return nderivative(3, x, y, z, t);
  }

  //--------------------------------------------------------------------------
  //   Header / Dim
  //--------------------------------------------------------------------------

  carto::PropertySet & InterpolatedVolume::header()
  {
    return _coeff.header();
  }

  const carto::PropertySet & InterpolatedVolume::header() const
  {
    return _coeff.header();
  }

  double InterpolatedVolume::getSizeX() const
  {
    return (double)_coeff.getSizeX();
  }

  double InterpolatedVolume::getSizeY() const
  {
    return (double)_coeff.getSizeY();
  }

  double InterpolatedVolume::getSizeZ() const
  {
    return (double)_coeff.getSizeZ();
  }

  long   InterpolatedVolume::getSizeT() const
  {
    return _coeff.getSizeT();
  }


  //--------------------------------------------------------------------------
  //   Mm accessors
  //--------------------------------------------------------------------------


  double
  InterpolatedVolume::atMm( double x, double y, double z, long t ) const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    return at( x/vs[0], y/vs[1], z/vs[2], t );
  }

  InterpolatedVolume::Vector
  InterpolatedVolume::derivativeMm( double x, double y, double z, long t ) const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    Vector der =  derivative( x/vs[0], y/vs[1], z/vs[2], t );
    der[0] /= vs[0];
    der[1] /= vs[1];
    der[2] /= vs[2];
    return der;
  }

  InterpolatedVolume::Matrix
  InterpolatedVolume::nderivativeMm( unsigned n, double x, double y, double z, long t ) const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    Matrix der =  nderivative( x/vs[0], y/vs[1], z/vs[2], t );
    for( int i = 0; i < der.getSizeX(); ++i )
      for( int j = 0; j < der.getSizeY(); ++j )
        for( int k = 0; k < der.getSizeZ(); ++k )
          for( int l = 0; l < der.getSizeT(); ++l )
            der(i, j, k, l) /= ( n > 0 ? vs[i] : 1. )
                             * ( n > 1 ? vs[j] : 1. )
                             * ( n > 2 ? vs[k] : 1. )
                             * ( n > 3 ? vs[l] : 1. );
    return der;
  }

  InterpolatedVolume::Vector
  InterpolatedVolume::derMm( double x, double y, double z, long t ) const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    Vector der = this->der( x/vs[0], y/vs[1], z/vs[2], t );
    der[0] /= vs[0];
    der[1] /= vs[1];
    der[2] /= vs[2];
    return der;
  }

  InterpolatedVolume::Matrix
  InterpolatedVolume::der2Mm( double x, double y, double z, long t ) const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    Matrix der =  der2( x/vs[0], y/vs[1], z/vs[2], t );
    for( int i = 0; i < der.getSizeX(); ++i )
      for( int j = 0; j < der.getSizeY(); ++j )
        der(i, j) /= vs[i] * vs[j];
    return der;
  }

  InterpolatedVolume::Matrix
  InterpolatedVolume::der3Mm( double x, double y, double z, long t ) const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    Matrix der =  der3( x/vs[0], y/vs[1], z/vs[2], t );
    for( int i = 0; i < der.getSizeX(); ++i )
      for( int j = 0; j < der.getSizeY(); ++j )
        for( int k = 0; k < der.getSizeZ(); ++k )
          der(i, j, k) /= vs[i] * vs[j] * vs[k];
    return der;
  }

  double InterpolatedVolume::getSizeXMm() const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    return getSizeX() * vs[0];
  }

  double InterpolatedVolume::getSizeYMm() const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    return getSizeY() * vs[1];
  }

  double InterpolatedVolume::getSizeZMm() const
  {
    std::vector<float> vs(4, 1.);
    header().getProperty( "voxel_size", vs );
    return getSizeZ() * vs[2];
  }


  //--------------------------------------------------------------------------
  //   Change interpolated volume
  //--------------------------------------------------------------------------

  unsigned InterpolatedVolume::order() const
  {
    return _order;
  }

  bool InterpolatedVolume::empty() const
  {
    return (bool)_coeff.get();
  }


  void InterpolatedVolume::setRecursiveMode()
  {
    _spline.reset( new BSpline( _order ) );
  }

  void InterpolatedVolume::setFastMode()
  {
    _spline.reset( new FastBSpline( _order ) );
  }

  void InterpolatedVolume::setTabularMode()
  {
    _spline.reset( new TabulBSpline( _order, _order ) );
  }


  //--------------------------------------------------------------------------
  //   Coefficients
  //--------------------------------------------------------------------------

  carto::VolumeRef<double> & InterpolatedVolume::coefficients()
  {
    return _coeff;
  }

  const carto::VolumeRef<double> & InterpolatedVolume::coefficients() const
  {
    return _coeff;
  }


  //--------------------------------------------------------------------------
  //   Instantiation
  //--------------------------------------------------------------------------

#define instantiate_methods( T ) \
  template InterpolatedVolume::InterpolatedVolume( const carto::Volume<T> &, unsigned );     \
  template InterpolatedVolume::InterpolatedVolume( const carto::VolumeRef<T> &, unsigned );  \
  template InterpolatedVolume::InterpolatedVolume( const AimsData<T> &, unsigned );          \
  template void InterpolatedVolume::setVolume( const carto::Volume<T> & vol, int );          \
  template void InterpolatedVolume::setVolume( const carto::VolumeRef<T> & vol, int );       \
  template void InterpolatedVolume::setVolume( const AimsData<T> & vol, int );               \
  template void InterpolatedVolume::setCoeff( const carto::Volume<T> & vol, int );           \
  template void InterpolatedVolume::setCoeff( const carto::VolumeRef<T> & vol, int );        \
  template void InterpolatedVolume::setCoeff( const AimsData<T> & vol, int );


  instantiate_methods( int8_t )
  instantiate_methods( uint8_t )
  instantiate_methods( int16_t )
  instantiate_methods( uint16_t )
  instantiate_methods( int32_t )
  instantiate_methods( uint32_t )
  instantiate_methods( int64_t )
  instantiate_methods( uint64_t )
  instantiate_methods( float )
  instantiate_methods( double )


} // namespace aims
