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

#include <aims/roi/roiIterator.h>
#include <aims/io/process.h>
#include <exception>

using namespace std;
using namespace carto;

namespace aims {

//----------------------------------------------------------------------------
VolumeROILabelMap::VolumeROILabelMap( AimsData< int32_t > &v ) :
  _volume( v )
{
}

//----------------------------------------------------------------------------
int32_t VolumeROILabelMap::at( const Point3df &point ) const
{
  const short x = (short) rint( point[ 0 ] / _volume.sizeX() );
  const short y = (short) rint( point[ 1 ] / _volume.sizeY() );
  const short z = (short) rint( point[ 2 ] / _volume.sizeZ() );
  int32_t label = 0;
  if ( x >= 0 && x < _volume.dimX() &&
       y >= 0 && y < _volume.dimY() &&
       z >= 0 && z < _volume.dimZ() ) {
    label = _volume( x, y, z );
  }
  return label;
}

//----------------------------------------------------------------------------
MotionedVolumeROILabelMap::MotionedVolumeROILabelMap( AimsData< int32_t > &v,
                                                      const Motion &m ) :
  VolumeROILabelMap( v ),
  _motion( m )
{
}

//----------------------------------------------------------------------------
int32_t MotionedVolumeROILabelMap::at( const Point3df &point ) const
{
  return VolumeROILabelMap::at( _motion.transform( point ) );
}


//----------------------------------------------------------------------------
rc_ptr< ROILabelMap > RoiIterator::createLabelMap()
{
  const Point3df &voxelSize = this->voxelSize();
  const Point3d &volumeDimension = this->volumeDimension();

  AimsData<int32_t> volume( volumeDimension[ 0 ],
                            volumeDimension[ 1 ],
                            volumeDimension[ 2 ] );
  volume.setSizeXYZT( voxelSize[ 0 ],
                           voxelSize[ 1 ],
                           voxelSize[ 2 ] );
  volume = 0UL;

  rc_ptr< ROILabelMap > result( new VolumeROILabelMap( volume ) );
  result->roiNames.push_back( "background" );
  for( restart(); isValid(); next() ) {
    result->roiNames.push_back( regionName() );
    const int32_t label = result->roiNames.size() - 1;
    rc_ptr< MaskIterator > pointsIterator = maskIterator();
    while( pointsIterator->isValid() ) {
      volume( pointsIterator->value() ) = label;
      pointsIterator->next();
    }
  }
  return result;
}

//----------------------------------------------------------------------------
MotionedRoiIterator::
MotionedRoiIterator( const carto::rc_ptr< RoiIterator > &roiIterator,
                     const Motion &motion ) :
  _roiIterator( roiIterator ),
  _motion( motion )
{
}

//----------------------------------------------------------------------------
MotionedRoiIterator::~MotionedRoiIterator() {}


//----------------------------------------------------------------------------
bool MotionedRoiIterator::isValid() const
{
  return _roiIterator->isValid();
}


//----------------------------------------------------------------------------
void MotionedRoiIterator::restart()
{
  _roiIterator->restart();
}


//----------------------------------------------------------------------------
size_t  MotionedRoiIterator::count() const
{
  return _roiIterator->count();
}


//----------------------------------------------------------------------------
void  MotionedRoiIterator::next()
{
  _roiIterator->next();
}


//----------------------------------------------------------------------------
rc_ptr< MaskIterator >  MotionedRoiIterator::maskIterator() const
{
  if ( _roiIterator.isNull() )
    throw runtime_error( "Fatal internal error: "
                         "MotionedRoiIterator::_roiIterator is null" );
  rc_ptr< MaskIterator > mi =_roiIterator->maskIterator();
  return rc_ptr< MaskIterator >( new MotionedMaskIterator( mi, _motion ) );
}


//----------------------------------------------------------------------------
const Point3d  MotionedRoiIterator::volumeDimension() const
{
  throw runtime_error( "volumeDimension() is not implemented for "
                       "MotionedRoiIterator" );
}


//----------------------------------------------------------------------------
std::string  MotionedRoiIterator::regionName() const
{
  return _roiIterator->regionName();
}


//----------------------------------------------------------------------------
rc_ptr< ROILabelMap > MotionedRoiIterator::createLabelMap()
{
  const Point3df &voxelSize = _roiIterator->voxelSize();
  const Point3d &volumeDimension = _roiIterator->volumeDimension();

  AimsData<int32_t> volume( volumeDimension[ 0 ],
                            volumeDimension[ 1 ],
                            volumeDimension[ 2 ] );
  volume.setSizeXYZT( voxelSize[ 0 ],
                      voxelSize[ 1 ],
                      voxelSize[ 2 ] );
  volume = 0UL;

  rc_ptr< ROILabelMap > 
    result( new MotionedVolumeROILabelMap( volume, _motion.inverse() ) );
  result->roiNames.push_back( "background" );
  for( restart(); isValid(); next() ) {
    result->roiNames.push_back( regionName() );
    const int32_t label = result->roiNames.size() - 1;
    rc_ptr< MaskIterator > pointsIterator = _roiIterator->maskIterator();
    while( pointsIterator->isValid() ) {
      volume( pointsIterator->value() ) = label;
      pointsIterator->next();
    }
  }
  return result;
}


//----------------------------------------------------------------------------
RoiIteratorOf<Graph>::
RoiIteratorOf( const Graph &roi,
               carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _roi( &roi ),
  _voxelSampler( voxelSampler )
{
  restart();
}

//----------------------------------------------------------------------------
RoiIteratorOf<Graph>::
RoiIteratorOf( const rc_ptr<Graph> &roi,
               carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _roiLife( roi ),
  _voxelSampler( voxelSampler )
{
  _roi = _roiLife.get();
  restart();
}


//----------------------------------------------------------------------------
RoiIteratorOf<Graph>::
RoiIteratorOf( const string &fileName,
               carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _voxelSampler( voxelSampler )
{
  Reader<Graph> gr( fileName );
  _roiLife.reset( gr.read( 1 ) );
  _roi = _roiLife.get();
  restart();
}


//----------------------------------------------------------------------------
RoiIteratorOf<Graph>::~RoiIteratorOf()
{
}

//----------------------------------------------------------------------------
rc_ptr< MaskIterator > RoiIteratorOf<Graph>::maskIterator() const
{
  if ( _roiLife.get() == NULL ) {
    return rc_ptr< MaskIterator >
      ( new MaskIteratorOf<Graph>( *_roi, *_itNames, _voxelSampler ) );
  } else {
    return rc_ptr< MaskIterator >
      ( new MaskIteratorOf<Graph>( _roiLife, *_itNames, _voxelSampler ) );
  }
}


//----------------------------------------------------------------------------
void RoiIteratorOf<Graph>::next()
{
  ++_itNames;
}


//----------------------------------------------------------------------------
bool RoiIteratorOf<Graph>::isValid() const
{
  return _itNames != _names.end();
}


//----------------------------------------------------------------------------
void RoiIteratorOf<Graph>::restart()
{
  _names.clear();
  for( Graph::const_iterator it = _roi->begin();
       it != _roi->end(); ++it ) {
    string name;
    if ( (*it)->getProperty( "name", name ) ||
         (*it)->getProperty( "label", name ) ) {
      _names.insert( name );
    }
  }
  _itNames = _names.begin();
}


//----------------------------------------------------------------------------
size_t  RoiIteratorOf<Graph>::count() const
{
  return _roi->order();
}


//----------------------------------------------------------------------------
std::string RoiIteratorOf<Graph>::regionName() const
{
  return *_itNames;
}


//----------------------------------------------------------------------------
template <class T>
static bool buildFromVolume( Process &p, const string &filename, Finder & );
static bool buildFromGraph( Process &p, const string &filename, Finder & );

class RoiIteratorFactory : public Process
{
  friend rc_ptr< RoiIterator > 
  getRoiIterator( const string &,
                  carto::rc_ptr< VoxelSampler > );

  RoiIteratorFactory();
  RoiIteratorFactory( carto::rc_ptr< VoxelSampler > voxelSampler );
  void _registerBuildFunctions();

  template <class T>
  friend 
  bool buildFromVolume( Process &p, const string &filename,
                        Finder &finder );
  friend bool buildFromGraph( Process &p, const string &filename,
                              Finder &finder );
  
  rc_ptr< RoiIterator > roiIterator;
  rc_ptr< VoxelSampler > voxelSampler;
};



//----------------------------------------------------------------------------
template <class T>
bool buildFromVolume
( Process &p,
  const string &filename,
  Finder & )
{
  RoiIteratorFactory &rif = static_cast< RoiIteratorFactory & >( p );
  rif.roiIterator = 
    rc_ptr< RoiIterator>( new RoiIteratorOf< AimsData<T> >( filename,
                                                          rif.voxelSampler ) );
  return true;
}

//----------------------------------------------------------------------------
bool buildFromGraph
( Process &p,
  const string &filename,
  Finder & )
{
  RoiIteratorFactory &rif = static_cast< RoiIteratorFactory & >( p );
  rif.roiIterator = 
    rc_ptr< RoiIterator>( new RoiIteratorOf< Graph >( filename, 
                                                      rif.voxelSampler ) );
  return true;
}


//----------------------------------------------------------------------------
RoiIteratorFactory::RoiIteratorFactory()
{
  _registerBuildFunctions();
}


//----------------------------------------------------------------------------
RoiIteratorFactory::RoiIteratorFactory( rc_ptr< VoxelSampler > vs ) :
  voxelSampler( vs )
{
  _registerBuildFunctions();
}


//----------------------------------------------------------------------------
void RoiIteratorFactory::_registerBuildFunctions()
{
  registerProcessType( "Volume", "S8",
                       &buildFromVolume< int8_t > );
  registerProcessType( "Volume", "U8",
                       &buildFromVolume< uint8_t > );
  registerProcessType( "Volume", "S16",
                       &buildFromVolume< int16_t > );
  registerProcessType( "Volume", "U16",
                       &buildFromVolume< uint16_t > );
  registerProcessType( "Volume", "S32",
                       &buildFromVolume< int32_t > );
  registerProcessType( "Volume", "U32",
                       &buildFromVolume< uint32_t > );
  registerProcessType( "Volume", "FLOAT",
                       &buildFromVolume< float > );
  registerProcessType( "Volume", "DOUBLE",
                       &buildFromVolume< double > );
  registerProcessType( "Graph", "VOID",
                       &buildFromGraph  );
}




//----------------------------------------------------------------------------
// maskIteratorFactory (to be continued)


//----------------------------------------------------------------------------
rc_ptr< RoiIterator > 
getRoiIterator( const string &fileName,
                carto::rc_ptr< VoxelSampler > voxelSampler )
{
  RoiIteratorFactory factory( voxelSampler );
  if ( ! factory.execute( fileName ) ) {
    throw runtime_error( string( "cannot read ROI " ) + fileName );
  }
  return factory.roiIterator;
}

//---------------------------------------------------------------------------
carto::rc_ptr< RoiIterator > 
getRoiIterator( const Graph &data,
                carto::rc_ptr< VoxelSampler > voxelSampler ) 
{
  return carto::rc_ptr< RoiIterator >
    ( new RoiIteratorOf< Graph >( data, voxelSampler ) );
}

//----------------------------------------------------------------------------
rc_ptr< RoiIterator > getRoiIterator( const string &fileName,
                                      const Motion &motion )
{
    return rc_ptr< RoiIterator >
      ( new MotionedRoiIterator( getRoiIterator( fileName ), motion ) );
}

//---------------------------------------------------------------------------
carto::rc_ptr< RoiIterator > getRoiIterator( const Graph &data,
                                             const Motion &motion )
{
  return carto::
    rc_ptr< RoiIterator >( new MotionedRoiIterator( getRoiIterator( data ), 
                                                    motion ) );
}

//----------------------------------------------------------------------------
rc_ptr< RoiIterator > 
getRoiIterator( const string &fileName,
                carto::rc_ptr< VoxelSampler > voxelSampler,
                const Motion &motion )
{
    return rc_ptr< RoiIterator >
      ( new MotionedRoiIterator( getRoiIterator( fileName, voxelSampler ),
                                 motion ) );
}

//---------------------------------------------------------------------------
carto::rc_ptr< RoiIterator > 
getRoiIterator( const Graph &data,
                carto::rc_ptr< VoxelSampler > voxelSampler,
                 const Motion &motion )
{
  return carto::
    rc_ptr< RoiIterator >
    ( new MotionedRoiIterator( getRoiIterator( data, voxelSampler ), 
                               motion ) );
}

} // namespace aims
