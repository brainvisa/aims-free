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
 getLinearInterpolator( const AimsData<uint8_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int8_t> & );
template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<uint16_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int16_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<uint32_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int32_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<float> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<double> & );

template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<uint8_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<int8_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<uint16_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<int16_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<uint32_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<int32_t> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<float> & );
template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const VolumeRef<double> & );

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
