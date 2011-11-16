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

#include <aims/roi/maskIterator.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <graph/graph/graph.h>
#include <aims/io/aimsGraphR.h>
#include <aims/graph/graphmanip.h>
#include <exception>
#include <cartobase/type/string_conversion.h>

using namespace std;
using namespace carto;
using namespace aims;

//----------------------------------------------------------------------------
MotionedMaskIterator::
MotionedMaskIterator( const carto::rc_ptr< MaskIterator > &maskIterator,
                      const Motion &motion ) :
  _maskIterator( maskIterator ),
  _motion( motion )
{
  _inverseMotion = _motion.inverse();
}

//----------------------------------------------------------------------------
MotionedMaskIterator::~MotionedMaskIterator()
{
}

//----------------------------------------------------------------------------
const Point3d &MotionedMaskIterator::value() const
{
  throw runtime_error( "value() is not allowed for MotionedMaskIterator (use valueMillimeters() instead)" );
}


//----------------------------------------------------------------------------
const Point3df MotionedMaskIterator::valueMillimeters() const
{
  return _motion.transform( _maskIterator->valueMillimeters() );
}

//----------------------------------------------------------------------------
void MotionedMaskIterator::next()
{
  _maskIterator->next();
}

//----------------------------------------------------------------------------
bool MotionedMaskIterator::isValid() const
{
  return _maskIterator->isValid();
}

//----------------------------------------------------------------------------
void MotionedMaskIterator::restart()
{
  _maskIterator->restart();
}

//----------------------------------------------------------------------------
const Point3df MotionedMaskIterator::voxelSize() const
{
  Point3df vs = _maskIterator->voxelSize();
  return Point3df( ( _motion.transform( Point3df( vs[0], 0, 0 ) - _motion.transform( Point3df( 0, 0, 0 ) ) ) ).norm(),
		   ( _motion.transform( Point3df( 0, vs[1], 0 ) - _motion.transform( Point3df( 0, 0, 0 ) ) ) ).norm(),
		   (_motion.transform( Point3df( 0, 0, vs[2] ) - _motion.transform( Point3df( 0, 0, 0 ) ) ) ).norm() );
}

//----------------------------------------------------------------------------
bool MotionedMaskIterator::contains( const Point3d &p ) const
{
  return contains( Point3df( p[ 0 ], p[ 1 ], p[ 2 ] ) );
}

//----------------------------------------------------------------------------
bool MotionedMaskIterator::contains( const Point3df &p ) const
{
  return _maskIterator->contains( _inverseMotion.transform( p ) );
}


//----------------------------------------------------------------------------
const Point3d MotionedMaskIterator::volumeDimension() const
{
  return _maskIterator->volumeDimension();
}

//----------------------------------------------------------------------------
std::string MotionedMaskIterator::regionName() const
{
  return _maskIterator->regionName();
}






//----------------------------------------------------------------------------
bool NodeFilter::filter( const AttributedObject & ) const
{
  return true;
}


//----------------------------------------------------------------------------
bool LabelNodeFilter::filter( const AttributedObject &object ) const
{
  int roi_label = 0;
  if ( object.getProperty( "roi_label", roi_label ) ) {
    return roi_label == _label;
  }
  return false;
}


//----------------------------------------------------------------------------
bool NameNodeFilter::filter( const AttributedObject &object ) const
{
  string name;
  if ( object.getProperty( "name", name ) ) {
    return name == _name;
  }
  return false;
}


//----------------------------------------------------------------------------
void MaskIteratorOf<Graph>::_getRequiredAttributes()
{
  vector<float> voxel_size;
  if ( ! _roi->getProperty( "voxel_size", voxel_size ) ) {
    throw runtime_error( "Cannot find graph \"voxel_size\" attribute" );
  }
  _voxelSize[0] = voxel_size[0];
  _voxelSize[1] = voxel_size[1];
  _voxelSize[2] = voxel_size[2];
}

//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const Graph &roi,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _roi( &roi ),
  _nodeFilter( new NodeFilter ),
  _voxelSampler( voxelSampler )
{
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const Graph &roi, int label,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _roi( &roi ),
  _nodeFilter( new LabelNodeFilter( label ) ),
  _voxelSampler( voxelSampler )
{
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const Graph &roi, const string &label,
                carto::rc_ptr< VoxelSampler > voxelSampler ):
  _roi( &roi ),
  _nodeFilter( new NameNodeFilter( label ) ),
  _voxelSampler( voxelSampler )
{
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const rc_ptr<Graph> &roi,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _roiLife( roi ),
  _nodeFilter( new NodeFilter ),
  _voxelSampler( voxelSampler )
{
  _roi = _roiLife.get();
  _getRequiredAttributes();
  restart();
}

//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const rc_ptr<Graph> &roi, int label,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _roiLife( roi ),
  _nodeFilter( new LabelNodeFilter( label ) ),
  _voxelSampler( voxelSampler )
{
  _roi = _roiLife.get();
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const rc_ptr<Graph> &roi, const string &label,
                carto::rc_ptr< VoxelSampler > voxelSampler ):
  _roiLife( roi ),
  _nodeFilter( new NameNodeFilter( label ) ),
  _voxelSampler( voxelSampler )
{
  _roi = _roiLife.get();
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const string &fileName,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _nodeFilter( new NodeFilter ),
  _voxelSampler( voxelSampler )
{
  Reader<Graph> gr( fileName );
  _roiLife.reset( gr.read( 1 ) );
  _roi = _roiLife.get();
  _getRequiredAttributes();
  restart();
}

//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const string &fileName, int label,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _nodeFilter( new LabelNodeFilter( label ) ),
  _voxelSampler( voxelSampler )
{
  Reader<Graph> gr( fileName );
  _roiLife.reset( gr.read( 1 ) );
  _roi = _roiLife.get();
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::
MaskIteratorOf( const string &fileName, const string &label,
                carto::rc_ptr< VoxelSampler > voxelSampler ) :
  _nodeFilter( new NameNodeFilter( label ) ),
  _voxelSampler( voxelSampler )
{
  Reader<Graph> gr( fileName );
  _roiLife.reset( gr.read( 1 ) );
  _roi = _roiLife.get();
  _getRequiredAttributes();
  restart();
}


//----------------------------------------------------------------------------
MaskIteratorOf<Graph>::~MaskIteratorOf()
{
}

//----------------------------------------------------------------------------
const Point3d &MaskIteratorOf<Graph>::value() const
{
  return _itPoints->first;
}

//----------------------------------------------------------------------------
const Point3df MaskIteratorOf<Graph>::valueMillimeters() const
{
  if ( _voxelSampler.isNull() ) {
    return  Point3df(  _itPoints->first[0] * _voxelSize[0],
                       _itPoints->first[1] * _voxelSize[1],
                       _itPoints->first[2] * _voxelSize[2] );
  } else {
    const Point3df &voxelSample = _voxelSampler->value();
    return  Point3df(  ( _itPoints->first[0] + voxelSample[ 0 ] ) * 
                       _voxelSize[0],
                       ( _itPoints->first[1] + voxelSample[ 1 ] ) * 
                       _voxelSize[1],
                       ( _itPoints->first[2] + voxelSample[ 2 ] ) * 
                       _voxelSize[2] );
  }
}

//----------------------------------------------------------------------------
void MaskIteratorOf<Graph>::
_findBucketAttributeNames( const Graph &graph,
			   const Vertex &node,
			   list<string> &attributeNames )
{
  // Get name of node's attributes containing a Bucket
  attributeNames.clear();
  rc_ptr<GraphElementTable> elementTable;
  if ( graph.getProperty( "aims_objects_table", elementTable ) ) {
    GraphElementTable::const_iterator itElementTable = 
      elementTable->find( node.getSyntax() );
    if ( itElementTable != elementTable->end() ) {
      for( map<string,GraphElementCode>::const_iterator it = 
	     itElementTable->second.begin();
	   it != itElementTable->second.end();
	   ++it ) {
	if ( it->second.objectType == "Bucket" ) {
	  attributeNames.push_back( it->second.attribute );
	}
      }
    }
  } else {
    throw runtime_error( "Cannot find \"aims_objects_table\" attribute in graph" );
  }
}


//----------------------------------------------------------------------------
void MaskIteratorOf<Graph>::next()
{
  // Next sample in voxel
  if ( ! _voxelSampler.isNull() ) {
    _voxelSampler->next();
    if ( _voxelSampler->isValid() ) return;
    _voxelSampler->restart();
  }

  // Next point
  ++_itPoints;
  if ( _itPoints == _bucket->end() ) {
    // Next bucket
    do {
      ++_itAttributeNames;
      if ( _itAttributeNames == _attributeNames.end() ) {
        // Next node
        do {
          ++_itRoi;
          if ( _itRoi == _roi->end() ) {
            return;
          }
          if ( ! _nodeFilter->filter( **_itRoi ) ) continue;
          _findBucketAttributeNames( *_roi, **_itRoi, _attributeNames );
          if ( _attributeNames.empty() ) continue;
          _itAttributeNames = _attributeNames.begin();
          break;
        } while( true );
      }
      // Get bucket from attribute name
      if ( (*_itRoi)->getProperty( *_itAttributeNames, _bucketMap ) ) {
        if ( _bucketMap->empty() ) continue;
        _bucket = & _bucketMap->begin()->second;
        if ( _bucket->empty() ) continue;
      } else {
        continue;
      }
      break;
    } while( true );
    _itPoints = _bucket->begin();
  }
}

//----------------------------------------------------------------------------
bool MaskIteratorOf<Graph>::isValid() const
{
  return _itRoi != _roi->end();
  
}

//----------------------------------------------------------------------------
void MaskIteratorOf<Graph>::restart()
{
  if ( ! _voxelSampler.isNull() ) {
    _voxelSampler->restart();
  }
  for( _itRoi = _roi->begin();
       _itRoi != _roi->end();
       ++_itRoi ) {
    if ( ! _nodeFilter->filter( **_itRoi ) ) continue;
    _findBucketAttributeNames( *_roi, **_itRoi, _attributeNames );
    if ( _attributeNames.empty() ) continue;
    for( _itAttributeNames = _attributeNames.begin();
         _itAttributeNames != _attributeNames.end();
         ++_itAttributeNames ) {
      // Get bucket from attribute name
      if ( (*_itRoi)->getProperty( *_itAttributeNames, _bucketMap ) ) {
        if ( _bucketMap->empty() ) continue;
        _bucket = & _bucketMap->begin()->second;
        if ( _bucket->empty() ) continue;
        _itPoints = _bucket->begin();
        return;
      }
    }
  }
}


//----------------------------------------------------------------------------
bool MaskIteratorOf<Graph>::contains( const Point3d &p ) const
{
  std::list<std::string> attributeNames;
  for(  Graph::const_iterator itRoi = _roi->begin();
        itRoi != _roi->end();
        ++itRoi ) {
    if ( ! _nodeFilter->filter( **itRoi ) ) continue;
    _findBucketAttributeNames( *_roi, **itRoi, attributeNames );
    if ( attributeNames.empty() ) continue;
    for( list<string>::const_iterator itAttributeNames = 
           attributeNames.begin();
         itAttributeNames != attributeNames.end();
         ++itAttributeNames ) {
      // Get bucket from attribute name
      rc_ptr< BucketMap<Void> > bucketMap;
      if ( (*itRoi)->getProperty( *itAttributeNames, bucketMap ) ) {
        if ( bucketMap->empty() ) continue;
        return bucketMap->begin()->second.find( p ) != 
          bucketMap->begin()->second.end();
      }
    }
  }
  return false;
}


//----------------------------------------------------------------------------
bool MaskIteratorOf<Graph>::contains( const Point3df &p ) 
  const
{
  const Point3d pixel( (short) rint( p[ 0 ] / _voxelSize[ 0 ] ),
                       (short) rint( p[ 1 ] / _voxelSize[ 1 ] ),
                       (short) rint( p[ 2 ] / _voxelSize[ 2 ] ) );
  return contains( pixel );
}

//----------------------------------------------------------------------------
const Point3d MaskIteratorOf<Graph>::volumeDimension() const
{
  vector<int> boundingbox_max;
  if ( ! _roi->getProperty( "boundingbox_max", boundingbox_max ) ) {
    throw runtime_error( "Cannot find graph \"boundingbox_max\" attribute" );
  }
  return Point3d( boundingbox_max[ 0 ] + 1,
                  boundingbox_max[ 1 ] + 1,
                  boundingbox_max[ 2 ] + 1 );
}

//----------------------------------------------------------------------------
string MaskIteratorOf<Graph>::regionName() const
{
  string result;
  if ( ! (*_itRoi)->getProperty( "label", result ) &&
       ! (*_itRoi)->getProperty( "name", result ) ) {
    result = "unknown";
  }
  return result;
}

//----------------------------------------------------------------------------

template <class T>
static bool buildFromVolume( Process &p, const string &filename, Finder & );
static bool buildFromGraph( Process &p, const string &filename, Finder & );

class MaskIteratorFactory : public Process
{
  friend rc_ptr< MaskIterator > aims::getMaskIterator( const string & );
  friend rc_ptr< MaskIterator > 
  aims::getMaskIterator( const string &,
                         carto::rc_ptr< VoxelSampler > );

  MaskIteratorFactory();
  MaskIteratorFactory( carto::rc_ptr< VoxelSampler > voxelSampler );
  void _registerBuildFunctions();

  template <class T>
  friend 
  bool buildFromVolume( Process &p, const string &filename,
			Finder &finder );
  friend bool buildFromGraph( Process &p, const string &filename,
			      Finder &finder );
  
  rc_ptr< MaskIterator > maskIterator;
  rc_ptr< VoxelSampler > voxelSampler;
};



//----------------------------------------------------------------------------
template <class T>
bool buildFromVolume
( Process &p,
  const string &filename,
  Finder & )
{
  MaskIteratorFactory &mif = static_cast< MaskIteratorFactory & >( p );
  mif.maskIterator = 
    rc_ptr< MaskIterator>( new MaskIteratorOf< AimsData<T> >( filename,
                                                              mif.
                                                              voxelSampler ) );
  return true;
}

//----------------------------------------------------------------------------
bool buildFromGraph
( Process &p,
  const string &filename,
  Finder & )
{
  MaskIteratorFactory &mif = static_cast< MaskIteratorFactory & >( p );
  mif.maskIterator = 
    rc_ptr< MaskIterator>( new MaskIteratorOf< Graph >( filename,
                                                        mif.voxelSampler ) );
  return true;
}


//----------------------------------------------------------------------------
MaskIteratorFactory::MaskIteratorFactory()
{
 _registerBuildFunctions();
}

//----------------------------------------------------------------------------
MaskIteratorFactory::
MaskIteratorFactory( rc_ptr< VoxelSampler > vs ) :
  voxelSampler( vs )
{
 _registerBuildFunctions();
}

//----------------------------------------------------------------------------
void MaskIteratorFactory::_registerBuildFunctions()
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
rc_ptr< MaskIterator > aims::getMaskIterator( const string &fileName )
{
  MaskIteratorFactory factory;
  if ( ! factory.execute( fileName ) ) {
    throw runtime_error( string( "cannot read mask " ) + fileName );
  }
  return factory.maskIterator;
}

//----------------------------------------------------------------------------
rc_ptr< MaskIterator > 
aims::getMaskIterator( const string &fileName,
                       carto::rc_ptr< VoxelSampler > voxelSampler )
{
  MaskIteratorFactory factory( voxelSampler );
  if ( ! factory.execute( fileName ) ) {
    throw runtime_error( string( "cannot read mask " ) + fileName );
  }
  return factory.maskIterator;
}

//----------------------------------------------------------------------------
rc_ptr< MaskIterator > aims::getMaskIterator( const string &fileName,
                                              const Motion &motion )
{
  return rc_ptr< MaskIterator >
    ( new MotionedMaskIterator( getMaskIterator( fileName ), motion ) );
}


//----------------------------------------------------------------------------
rc_ptr< MaskIterator >
aims::getMaskIterator( const string &fileName,
                       carto::rc_ptr< VoxelSampler > voxelSampler,
                       const Motion &motion )
{
  return rc_ptr< MaskIterator >
    ( new MotionedMaskIterator( getMaskIterator( fileName, voxelSampler ),
                                motion ) );
}
