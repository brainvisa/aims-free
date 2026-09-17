// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 * 3D linear image interpolation
 */
#include <aims/resampling/linearInterpolator.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>

using namespace std;
using namespace carto;

namespace aims {


  //----------------//
 //  Interpolator  //
//----------------//

//-----------------------------------------------------------------------------
Interpolator::~Interpolator() {}


  //--------------------------------//
 //  linear interpolator factories //
//--------------------------------//

//-----------------------------------------------------------------------------
class LinearInterpolatorFactory : public Process
{
  friend rc_ptr< Interpolator > getLinearInterpolator( const string & );

  LinearInterpolatorFactory();
  template <typename T>
  static bool createInterpolator( Process & p, const string & filename, 
                                  Finder & finder );

  rc_ptr< Interpolator > interpolator;
};


//-----------------------------------------------------------------------------
LinearInterpolatorFactory:: LinearInterpolatorFactory()
{
  registerProcessType( "Volume", "S8", &createInterpolator<int8_t> );
  registerProcessType( "Volume", "U8", &createInterpolator<uint8_t> );
  registerProcessType( "Volume", "S16", &createInterpolator<int16_t> );
  registerProcessType( "Volume", "U16", &createInterpolator<uint16_t> );
  registerProcessType( "Volume", "S32", &createInterpolator<int32_t> );
  registerProcessType( "Volume", "U32", &createInterpolator<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &createInterpolator<float> );
  registerProcessType( "Volume", "DOUBLE", &createInterpolator<double> );
}


//-----------------------------------------------------------------------------
template <typename T>
bool LinearInterpolatorFactory::createInterpolator( Process & p,
                                                    const string & filename, 
                                                    Finder & finder )
{
  VolumeRef<T> image;
  Reader< VolumeRef<T> > r( filename );
  r.setMode( MAP_RO );
  string format = finder.format();
  if( ! r.read( image, 0, &format ) ) {
    return false;
  }
  LinearInterpolatorFactory &factory = 
    static_cast< LinearInterpolatorFactory &>( p );
  factory.interpolator = 
    rc_ptr< Interpolator >( new LinearInterpolator<T>( image ) );
  return true;
}


//-----------------------------------------------------------------------------
rc_ptr< Interpolator > getLinearInterpolator( const string &fileName )
{
  LinearInterpolatorFactory factory;
  factory.execute( fileName );
  return factory.interpolator;
}


//-----------------------------------------------------------------------------
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<uint8_t> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<int8_t> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<uint16_t> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<int16_t> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<uint32_t> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<int32_t> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<float> > & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const rc_ptr<Volume<double> > & );


  //--------------------------//
 //   LinearInterpolator<T>  //
//--------------------------//

//-----------------------------------------------------------------------------
template class LinearInterpolator<uint8_t>;
template class LinearInterpolator<int8_t>;
template class LinearInterpolator<uint16_t>;
template class LinearInterpolator<int16_t>;
template class LinearInterpolator<uint32_t>;
template class LinearInterpolator<int32_t>;
template class LinearInterpolator<float>;
template class LinearInterpolator<double>;


} // namespace aims
