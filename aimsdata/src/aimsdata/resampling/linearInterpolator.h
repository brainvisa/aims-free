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

/*
 * 3D linear image interpolation
 */

#ifndef AIMS_LINEAR_INTERPOLATOR
#define AIMS_LINEAR_INTERPOLATOR

#include <cassert>

#include <aims/data/data.h>
#include <cartobase/smart/rcptr.h>
#include <vector>

namespace aims {


  //----------------//
 //  Interpolator  //
//----------------//

//-----------------------------------------------------------------------------
class Interpolator : public carto::RCObject
{
public:

  typedef float Coordinate_t;
  typedef Point3df Point_t;
  typedef double Scalar_t;

  virtual ~Interpolator();

  /** Return true if point ( x, y, z ) can be used for interpolation. If
      interpolation is tried on an invalid point, it may crash the program.
  */
  virtual bool isValid( Coordinate_t x, Coordinate_t y, 
                        Coordinate_t z ) const = 0;
  /** Return true if point can be used for interpolation. If
      interpolation is tried on an invalid point, it may crash the program.
  */
  inline bool isValid( const Interpolator::Point_t &point ) const
  {
    return isValid( point[ 0 ], point[ 1 ], point[ 2 ] );
  }

  /** \deprecated Obsolete, use value() method */
  inline Scalar_t operator()( Coordinate_t x, Coordinate_t y,
                                Coordinate_t z ) const
    __attribute__((__deprecated__("use value() method instead")))
  {
    return do_interpolation( x, y, z );
  }
  /** \deprecated Obsolete, use value() method */
  inline Scalar_t operator()( const Interpolator::Point_t &point ) const
    __attribute__((__deprecated__("use value() method instead")))
  {
    return do_interpolation( point[ 0 ], point[ 1 ], point[ 2 ] );
  }
  /** \deprecated Obsolete, use values() method */
  inline void operator()( Coordinate_t x, Coordinate_t y, Coordinate_t z,
                             std::vector<Scalar_t> & v ) const
    __attribute__((__deprecated__("use values() method instead")))
  {
    do_interpolations( x, y, z, v );
  }
  /** \deprecated Obsolete, use values() method */
  inline void operator()( const Interpolator::Point_t &point,
                            std::vector<Scalar_t> &v ) const
    __attribute__((__deprecated__("use values() method instead")))
  {
    do_interpolations( point[ 0 ], point[ 1 ], point[ 2 ], v );
  }

  /** Interpolate to get a value from point ( x, y ,z ). */
  inline Scalar_t value( Coordinate_t x, Coordinate_t y,
                         Coordinate_t z ) const
  {
    return do_interpolation(x, y, z);
  }
  /** Interpolate to get a value from point */
  inline Scalar_t value( const Interpolator::Point_t &point ) const
  {
    return do_interpolation( point[ 0 ], point[ 1 ], point[ 2 ] );
  }
  /** Interpolate to get a series of values from point ( x, y ,z ) */
  inline void values( Coordinate_t x, Coordinate_t y, Coordinate_t z,
                      std::vector<Scalar_t> &v ) const
  {
    do_interpolations(x, y, z, v);
  }
  /** Interpolate to get a series of values from point ( x, y ,z ) */
  inline void values( const Interpolator::Point_t &point,
                      std::vector<Scalar_t> &v ) const
  {
    do_interpolations( point[ 0 ], point[ 1 ], point[ 2 ], v );
  }
  /** Return the header of the image.
  */
  virtual const carto::PropertySet &header() const = 0;
private:
  virtual Scalar_t do_interpolation( Coordinate_t x, Coordinate_t y,
                                     Coordinate_t z ) const = 0;
  virtual void do_interpolations( Coordinate_t x,
                                  Coordinate_t y,
                                  Coordinate_t z,
                                  std::vector<Scalar_t> &v ) const = 0;
};


  //--------------------------------//
 //  linear interpolator factories //
//--------------------------------//
//-----------------------------------------------------------------------------
carto::rc_ptr< Interpolator > getLinearInterpolator( const std::string & );
template <typename T>
carto::rc_ptr< Interpolator > getLinearInterpolator( const AimsData<T> & );
template <typename T>
carto::rc_ptr< Interpolator > getLinearInterpolator(
  const carto::VolumeRef<T> & );
template <typename T>
carto::rc_ptr< Interpolator > getLinearInterpolator(
  const carto::rc_ptr<carto::Volume<T> > & );


  //--------------------------//
 //   LinearInterpolator<T>  //
//--------------------------//

class LinearInterpolatorFactory;

//-----------------------------------------------------------------------------
template <typename T>
class LinearInterpolator : public Interpolator
{

public:
  LinearInterpolator( const carto::VolumeRef<T> & image );
  virtual ~LinearInterpolator();

  bool isValid( Coordinate_t x, Coordinate_t y, Coordinate_t z ) const;

  virtual const carto::PropertySet &header() const;

private:
  const carto::VolumeRef<T> _image;

  Scalar_t do_interpolation( Coordinate_t x, Coordinate_t y,
                             Coordinate_t z ) const;
  void do_interpolations( Coordinate_t x, Coordinate_t y, Coordinate_t z,
                          std::vector<Scalar_t> & ) const;

  inline static
  void _interpolationCoefficients( Coordinate_t xx,
                                   int &x,
                                   int &X,
                                   Coordinate_t &ax,
                                   int dx, float sx );

  inline static
  Scalar_t linint(Scalar_t a, Scalar_t b, Scalar_t x) { return a + x*(b-a); }


  int _dimX, _dimY, _dimZ;
  float _invsizeX, _invsizeY, _invsizeZ;
};

//-----------------------------------------------------------------------------
extern template class LinearInterpolator<uint8_t>;
extern template class LinearInterpolator<int8_t>;
extern template class LinearInterpolator<uint16_t>;
extern template class LinearInterpolator<int16_t>;
extern template class LinearInterpolator<uint32_t>;
extern template class LinearInterpolator<int32_t>;
extern template class LinearInterpolator<float>;
extern template class LinearInterpolator<double>;


  //--------------------------------//
 //  linear interpolator factories //
//--------------------------------//

//-----------------------------------------------------------------------------
template <typename T>
inline
carto::rc_ptr< Interpolator > getLinearInterpolator( const AimsData<T> &image )
{
  return carto::rc_ptr< Interpolator >( new LinearInterpolator<T>( image ) );
}


//-----------------------------------------------------------------------------
template <typename T>
inline
carto::rc_ptr< Interpolator > getLinearInterpolator(
  const carto::VolumeRef<T> &image )
{
  return carto::rc_ptr< Interpolator >( new LinearInterpolator<T>( image ) );
}


//-----------------------------------------------------------------------------
template <typename T>
inline
carto::rc_ptr< Interpolator > getLinearInterpolator(
  const carto::rc_ptr<carto::Volume<T> > &image )
{
  return carto::rc_ptr< Interpolator >( new LinearInterpolator<T>( image ) );
}


//-----------------------------------------------------------------------------
extern template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<uint8_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int8_t> & );
extern template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<uint16_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int16_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<short> & );
extern template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<uint32_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int32_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<float> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<double> & );

extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<uint8_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<int8_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<uint16_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<int16_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<short> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<uint32_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<int32_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<float> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::VolumeRef<double> & );

//-----------------------------------------------------------------------------
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<uint8_t> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<int8_t> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<uint16_t> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<int16_t> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<short> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<uint32_t> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<int32_t> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<float> > & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const carto::rc_ptr<carto::Volume<double> > & );


  //--------------------------//
 //   LinearInterpolator<T>  //
//--------------------------//

//-----------------------------------------------------------------------------
template <typename T>
inline
LinearInterpolator<T>::LinearInterpolator(
    const carto::VolumeRef<T> & image ) :
  _image( image )
{
  _dimX = _image->getSizeX();
  _dimY = _image->getSizeY();
  _dimZ = _image->getSizeZ();

  std::vector<float> vs( 3, 1. );
  try
  {
    carto::Object o = _image->header().getProperty( "voxel_size" );
    vs[0] = o->getArrayItem( 0 )->getScalar();
    vs[1] = o->getArrayItem( 1 )->getScalar();
    vs[2] = o->getArrayItem( 2 )->getScalar();
  }
  catch( ... )
  {
  }

  assert( vs[0] > 0 );
  _invsizeX = float( 1.0 / vs[0] );
  assert( vs[1] > 0 );
  _invsizeY = float( 1.0 / vs[1] );
  assert( vs[2] > 0 );
  _invsizeZ = float( 1.0 / vs[2] );
}


//-----------------------------------------------------------------------------
template <typename T>
inline
LinearInterpolator<T>::~LinearInterpolator()
{
}

//-----------------------------------------------------------------------------
template <typename T>
inline
bool LinearInterpolator<T>::isValid( Interpolator::Coordinate_t x,
                                     Interpolator::Coordinate_t y, 
                                     Interpolator::Coordinate_t z ) const
{
  std::vector<float> vs( 3, 1. );
  try
  {
    carto::Object o = _image->header().getProperty( "voxel_size" );
    vs[0] = o->getArrayItem( 0 )->getScalar();
    vs[1] = o->getArrayItem( 1 )->getScalar();
    vs[2] = o->getArrayItem( 2 )->getScalar();
  }
  catch( ... )
  {
  }

  const Interpolator::Coordinate_t hx = - vs[0] / 2;
  const Interpolator::Coordinate_t hy = - vs[1] / 2;
  const Interpolator::Coordinate_t hz = - vs[2] / 2;
  return x >= hx && y >= hy && z >= hz &&
    x < _dimX * vs[0] + hx &&
    y < _dimY * vs[1] + hy &&
    z < _dimZ * vs[2] + hz;
}


//-----------------------------------------------------------------------------
template <typename T>
inline
void LinearInterpolator<T>::
  _interpolationCoefficients( Interpolator::Coordinate_t xx,
                              int & x,
                              int & X,
                              Interpolator::Coordinate_t & ax,
                              int dx, float isx)
{
  const Interpolator::Coordinate_t px = xx * isx;
  
  x = int(px);
  X = x+1;
  if ( px < 0 )
  {
    x = X = 0;
    ax = 1;
  }
  else if (X >= dx)
  {
    X = x = dx - 1;
    ax = 0;
  }
  else
  {
    ax = px - x;
    assert(ax >= 0);
    assert(ax <= 1);
  }
}


//-----------------------------------------------------------------------------
template <typename T>
Interpolator::Scalar_t
LinearInterpolator<T>::do_interpolation( Interpolator::Coordinate_t xx,
                                         Interpolator::Coordinate_t yy,
                                         Interpolator::Coordinate_t zz ) const
{
  int x, X, y, Y, z, Z;
  Interpolator::Coordinate_t ax, ay, az;
  _interpolationCoefficients(xx, x, X, ax, _dimX, _invsizeX);
  _interpolationCoefficients(yy, y, Y, ay, _dimY, _invsizeY);
  _interpolationCoefficients(zz, z, Z, az, _dimZ, _invsizeZ);

  return
      linint(
             linint(
                    linint(_image( x, y, z ), _image( X, y, z ), ax),
                    linint(_image( x, Y, z ), _image( X, Y, z ), ax), ay),
             linint(
                    linint(_image( x, y, Z ), _image( X, y, Z ), ax),
                    linint(_image( x, Y, Z ), _image( X, Y, Z ), ax), ay), az);
}


//-----------------------------------------------------------------------------
template <typename T>
void LinearInterpolator<T>::
  do_interpolations( Interpolator::Coordinate_t xx,
                     Interpolator::Coordinate_t yy,
                     Interpolator::Coordinate_t zz,
                     std::vector< Interpolator::Scalar_t > &values ) const
{
  values.resize( _image->getSizeT() );
  int x, X, y, Y, z, Z;
  Interpolator::Coordinate_t ax, ay, az;
  _interpolationCoefficients(xx, x, X, ax, _dimX, _invsizeX);
  _interpolationCoefficients(yy, y, Y, ay, _dimY, _invsizeY);
  _interpolationCoefficients(zz, z, Z, az, _dimZ, _invsizeZ);

  for( int t = 0; t < _image->getSizeT(); ++t ) {
    values[ t ] = 
      linint(
             linint(
                    linint(_image( x, y, z, t ), _image( X, y, z, t ), ax),
                    linint(_image( x, Y, z, t ), _image( X, Y, z, t ), ax),
                    ay),
             linint(
                    linint(_image( x, y, Z, t ), _image( X, y, Z, t ), ax),
                    linint(_image( x, Y, Z, t ), _image( X, Y, Z, t ), ax),
                    ay), az);
  }
}


//-----------------------------------------------------------------------------
template <typename T>
inline
const carto::PropertySet & LinearInterpolator<T>::header() const
{
  return _image->header();
}

} // namespace aims

#endif // ifdef AIMS_LINEAR_INTERPOLATOR
