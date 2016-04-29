/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/* datatypecode.h must be included before any Parameterizable object can 
   be instantiated on Aims objects (here, Point3df).
   I didn't get deep into the code and I'm not sure it is better here than in 
   bundles.h. What I am sure of is that without this include, 
   DataTypeCode<Point3df> is instantated with the default template definition,
   which is wrong and causes very strange problems in other parts of the 
   library
   Denis 21/06/2004
*/

#include <aims/mesh/surface.h>
#include <aims/io/datatypecode.h>
#include <aims/fibers/bundles.h>
// FIXME: temporary
#include <aims/fibers/trackvisbundlereader.h>
#include <cartobase/type/byte_order.h>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/object/pythonreader.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/io_g.h>
#include <sstream>
#include <stdexcept>
#include <math.h>
#include <iostream>


using namespace std;
using namespace carto;
using namespace aims;


namespace
{


  //----------------------//
 //  AttributedFileName  //
//----------------------//

// WARNING : This class has nothing to do here. It should be somewhere else
//           in aimsdata (io ?) or cartobase (attributed ?)

class AttributedFileName
{
public:

  inline AttributedFileName();
  inline AttributedFileName( const std::string &,
                             const carto::SyntaxSet & = carto::SyntaxSet() );

  void reset( const std::string &,
              const carto::SyntaxSet & = carto::SyntaxSet() );

  inline const std::string &fileName() const;
  inline const carto::Object &attributes() const;

private:

  std::string _fileName;
  carto::Object _attributes;
};



  //-------------------------//
 //  fileNameAndParameters  //
//-------------------------//

// WARNING : This function has nothing to do here. It should be somewhere else
//           in aimsdata (io ?) or cartobase (attributed ?)

std::string fileNameAndParameters( const std::string &,
                                   carto::Object );



  //----------------------//
 //  AttributedFileName  //
//----------------------//


//-----------------------------------------------------------------------------
inline AttributedFileName::AttributedFileName() :
  _attributes( carto::Object::value( carto::Dictionary() ) )
{
}


//-----------------------------------------------------------------------------
inline AttributedFileName::AttributedFileName( const std::string &fileString,
                                               const carto::SyntaxSet &syntax
                                               ) :
  _attributes( carto::Object::value( carto::Dictionary() ) )
{
  reset( fileString, syntax );
}


//-----------------------------------------------------------------------------
inline const std::string &AttributedFileName::fileName() const
{
  return _fileName;
}


//-----------------------------------------------------------------------------
inline const carto::Object &
AttributedFileName::attributes() const
{
  return _attributes;
}


//-----------------------------------------------------------------------------
void AttributedFileName::reset( const std::string &fileString,
                                const carto::SyntaxSet &syntax )
{
  _fileName = "";
  
  TypedObject< Dictionary > *pattrs = 
    static_cast<TypedObject< Dictionary > *>( _attributes.get() );
  pattrs->getValue().clear();
  if ( ! fileString.empty() ) {
    string::size_type openBraceIndex = fileString.find( '{' );
    if ( openBraceIndex == string::npos ) {
      _fileName = fileString;
    } else {
      _fileName = fileString.substr( 0, openBraceIndex );
      stringstream in( fileString.substr( openBraceIndex + 1, 
                                          fileString.size() - openBraceIndex ),
                       stringstream::in );
      PythonReader reader( syntax );
      reader.attach( in );
      reader.readDictionary2( *pattrs );
    }
  }
}


  //-------------------------//
 //  fileNameAndParameters  //
//-------------------------//


//-----------------------------------------------------------------------------
std::string fileNameAndParameters( const std::string &fileString,
                                   PropertySet &param )
{
  // Create syntax
  SyntaxSet syntaxSet;
  Syntax &syntax = syntaxSet[ "__generic__" ];
  for( PropertySet::iterator it = param.getIterator(); it.isValid();
       it.next() ) {
    syntax[ it.key() ] = it.currentValue()->type();
  }
  // Extract file name and parse parameters values
  AttributedFileName attributedFileName( fileString, syntaxSet );
  Dictionary &pdict = attributedFileName.attributes()
    ->value<Dictionary>();
  for( Dictionary::iterator it = pdict.begin();
       it != pdict.end(); ++it ) {
    param.setProperty( it->first, it->second );
  }
  return attributedFileName.fileName();
}



  //---------------//
 //  sorted_pair  //
//---------------//

template <typename T>
class sorted_pair : public pair<T,T>
{
public:

  inline sorted_pair( const T &a, const T& b );
};

//-----------------------------------------------------------------------------
template <typename T>
inline sorted_pair<T>::sorted_pair( const T &a, const T& b ) {
  if ( a < b ) {
    this->first = a;
    this->second = b;
  } else {
    this->first = b;
    this->second = a;
  }
}

} // namespace ""


namespace aims
{

  //------------------//
 //  BundleProducer  //
//------------------//

//-----------------------------------------------------------------------------
void BundleProducer::addBundleListener( BundleListener &bundleListener )
{
  _bundleListeners.push_back( &bundleListener );
}


//-----------------------------------------------------------------------------
void BundleProducer::startBundle( const BundleInfo &bundleInfo )
{
  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->bundleStarted( *this, bundleInfo );
  }
}


//-----------------------------------------------------------------------------
void BundleProducer::terminateBundle( const BundleInfo &bundleInfo )
{
  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->bundleTerminated( *this, bundleInfo );
  }
}


//-----------------------------------------------------------------------------
void BundleProducer::startFiber( const BundleInfo &bundleInfo, 
                                 const FiberInfo &fiberInfo )
{
  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->fiberStarted( *this, bundleInfo, fiberInfo );
  }
}

//-----------------------------------------------------------------------------
void BundleProducer::terminateFiber( const BundleInfo &bundleInfo, 
                                     const FiberInfo &fiberInfo )
{
  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->fiberTerminated( *this, bundleInfo, fiberInfo );
  }
}

//-----------------------------------------------------------------------------
void BundleProducer::terminateFiber( const BundleInfo &bundleInfo,
                                     const FiberInfo &fiberInfo,
                                     FiberPoint* fiber,
                                     int &fiberSize )
{

  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->fiberTerminated( *this, bundleInfo, fiberInfo, fiber, fiberSize );
  }
}

//-----------------------------------------------------------------------------
void BundleProducer::addFiberPoint( const BundleInfo &bundleInfo, 
                                    const FiberInfo &fiberInfo,
                                    const FiberPoint &fiberPoint )
{
  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->newFiberPoint( *this, bundleInfo, fiberInfo, fiberPoint );
  }
}

//-----------------------------------------------------------------------------
void BundleProducer::noMoreBundle()
{
  for( BundleListenerList::iterator i = _bundleListeners.begin();
       i != _bundleListeners.end(); ++i ) {
    (*i)->noMoreBundle( *this );
  }
}


  //------------------//
 //  BundleListener  //
//------------------//

//-----------------------------------------------------------------------------
BundleListener::~BundleListener()
{
}


//-----------------------------------------------------------------------------
void BundleListener::bundleStarted( const BundleProducer &, 
                                    const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleListener::bundleTerminated( const BundleProducer &, 
                                      const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleListener::fiberStarted( const BundleProducer &, const BundleInfo &,
                                   const FiberInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleListener::fiberTerminated( const BundleProducer &, const BundleInfo &,
                                      const FiberInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleListener::fiberTerminated( const BundleProducer &, const BundleInfo &,
                                      const FiberInfo & ,
                                      FiberPoint * , int & )
{
}

//-----------------------------------------------------------------------------
void BundleListener::newFiberPoint( const BundleProducer &, const BundleInfo &,
                                    const FiberInfo &, const FiberPoint & )
{
}


//-----------------------------------------------------------------------------
void BundleListener::noMoreBundle( const BundleProducer & )
{
}


  //----------------//
 //  BundleWriter  //
//----------------//

//-----------------------------------------------------------------------------
BundleWriter::BundleWriter() :
  _ascii( false )
{
  _header = Object::value( Dictionary() );
  _header->setProperty( "format", string( "bundles_1.0" ) );
  _header->setProperty( "byte_order", string( byteOrderString() ) );
  addBuiltinProperty( "ascii", _ascii );
  _header->setProperty( "data_file_name", string( "*.bundlesdata" ) );
  _header->setProperty( "space_dimension", 3 );
  _header->setProperty( "bundles", ObjectVector() );
  _writtenFibersCount = 0;
}


//-----------------------------------------------------------------------------
BundleWriter::~BundleWriter()
{
  _dataFile << std::flush;
}


//-----------------------------------------------------------------------------
void BundleWriter::setFileString( const string &fileString ) {
  _fileName = fileNameAndParameters( fileString, *this );
  if ( _fileName.size() > 8 &&
       _fileName.substr( _fileName.size() - 8 ) == ".bundles" ) {
    _fileName = _fileName.substr( 0, _fileName.size() - 8 );
  }
  _header->setProperty( "binary", _ascii ? 0 : 1 );
  _dataFile.open( ( _fileName + ".bundlesdata" ).c_str(), 
                  ios::out | ios::binary );
  checkStreams();  
}


//-----------------------------------------------------------------------------
void BundleWriter::bundleStarted( const BundleProducer &,
                                  const BundleInfo & )
{
  _firstFiberInBundle = _writtenFibersCount;
}


//-----------------------------------------------------------------------------
void BundleWriter::bundleTerminated( const BundleProducer &,
                                     const BundleInfo &bundleInfo )
{
  if ( _firstFiberInBundle < _writtenFibersCount ) {
    ObjectVector &hbundles 
      = _header->getProperty( "bundles" )->value<ObjectVector>();
    hbundles.push_back( Object::value( bundleInfo.name() ) );
    hbundles.push_back( Object::value( _firstFiberInBundle ) );
  }
}


//-----------------------------------------------------------------------------
void BundleWriter::fiberStarted( const BundleProducer &, 
                                 const BundleInfo &,
                                 const FiberInfo & )
{
  _fiberPoints.clear();
}


//-----------------------------------------------------------------------------
void BundleWriter::fiberTerminated( const BundleProducer &,
                                    const BundleInfo &,
                                    const FiberInfo & )
{
  checkStreams();
  if ( _fiberPoints.size() > 1 ) {
    ++_writtenFibersCount;
    int size = _fiberPoints.size();
    if ( _ascii ) {
      _dataFile << size << " ";
    } else {
      _dataFile.write( reinterpret_cast<char *>( &size ), sizeof(size) );
    }
    for( FiberPoints::iterator iPoint = _fiberPoints.begin();
         iPoint != _fiberPoints.end(); ++iPoint ) {
      checkStreams();
      FiberPoint &point = *iPoint;
      if ( _ascii ) {
        _dataFile << point[ 0 ] << " " << point[ 1 ] << " " << point[ 2 ]
                  << ";";
      } else {
        _dataFile.write( reinterpret_cast<char *>( &point[ 0 ] ),
                         sizeof( point[ 0 ] ) );
        _dataFile.write( reinterpret_cast<char *>( &point[ 1 ] ) ,
                         sizeof( point[ 1 ] ) );
        _dataFile.write( reinterpret_cast<char *>( &point[ 2 ] ),
                         sizeof( point[ 2 ] ) );
      }
    }
    if ( _ascii ) _dataFile << endl;
  }
  checkStreams();
}


//-----------------------------------------------------------------------------
void BundleWriter::newFiberPoint( const BundleProducer &,
                                  const BundleInfo &,
                                  const FiberInfo &,
                                  const FiberPoint &point )
{
  _fiberPoints.push_back( point );
}


//-----------------------------------------------------------------------------
void BundleWriter::noMoreBundle( const BundleProducer & )
{
  _dataFile << std::flush;
  _dataFile.close();
  checkStreams();
  _header->setProperty( "curves_count", _writtenFibersCount );
  PythonWriter out( _fileName + ".bundles" );
  out.write( *_header );
}

//-----------------------------------------------------------------------------
void BundleWriter::checkStreams()
{
  if ( ! _dataFile.good() ) throw runtime_error( "Error while writing data file" );
}

//-----------------------------------------------------------------------------
void BundleWriter::addFiber( const BundleProducer&, const BundleInfo &,const FiberInfo & ,  FiberPoint *fiber, int &fiberSize)
{

  checkStreams();

  if ( fiberSize > 0 ) {
    ++_writtenFibersCount;
    if ( _ascii ) {
      _dataFile << fiberSize << " ";
    } else {
      _dataFile.write( reinterpret_cast< char * >( &fiberSize ),
                       sizeof( fiberSize ) );
    }

    checkStreams();

    if ( _ascii )
    {
      for( int i = 0; i < fiberSize; i++ )
      {
        _dataFile << fiber[ i ][ 0 ] << " " << fiber[ i ][ 1 ]
                  << " " << fiber[ i ][ 2 ];
      }
    }
    else
    {
      _dataFile.write( reinterpret_cast< char * >( &fiber[ 0 ] ),
                        sizeof( FiberPoint ) *fiberSize );
    }
  delete []fiber;
  if ( _ascii ) _dataFile << endl;
  }
  checkStreams();

}



  //---------------------//
 //  BundleReader  //
//---------------------//

//-----------------------------------------------------------------------------
BundleReader::BundleReader( const string &fileName )
  : BundleProducer(), BundleListener()
{
  _fileName = fileName;
}


//-----------------------------------------------------------------------------
BundleReader::~BundleReader()
{
}


void BundleReader::bundleStarted( const BundleProducer &,
                                  const BundleInfo & bi )
{
  startBundle( bi );
}


void BundleReader::bundleTerminated( const BundleProducer &,
                                     const BundleInfo & bi )
{
  terminateBundle( bi );
}


void BundleReader::fiberStarted( const BundleProducer &, const BundleInfo & bi,
                    const FiberInfo & fi )
{
  startFiber( bi, fi );
}


void BundleReader::fiberTerminated( const BundleProducer &,
                                    const BundleInfo & bi,
                                    const FiberInfo & fi )
{
  terminateFiber( bi, fi );
}


void BundleReader::newFiberPoint( const BundleProducer &,
                                  const BundleInfo & bi,
                                  const FiberInfo & fi, const FiberPoint & fp )
{
  addFiberPoint( bi, fi, fp );
}


void BundleReader::noMoreBundle( const BundleProducer & )
{
  BundleProducer::noMoreBundle();
}


//-----------------------------------------------------------------------------
void BundleReader::read()
{
  rc_ptr<BundleFormatReader> low_reader;

  if( _fileName.length() >= 4
      && _fileName.substr( _fileName.size() - 4 ) == ".trk" )
    low_reader.reset( new TrackvisBundleReader( _fileName ) );
  else
    low_reader.reset( new ConnectomistBundlesReader( _fileName ) );

  low_reader->addBundleListener( *this );
  low_reader->read();
}


//-----------------------------------------------------------------------------
Object BundleReader::readHeader()
{
  rc_ptr<BundleFormatReader> low_reader;

  if( _fileName.length() >= 4
      && _fileName.substr( _fileName.size() - 4 ) == ".trk" )
    low_reader.reset( new TrackvisBundleReader( _fileName ) );
  else
    low_reader.reset( new ConnectomistBundlesReader( _fileName ) );

  return low_reader->readHeader();
}

  //---------------------//
 //  BundleFormaReader  //
//---------------------//

BundleFormatReader::BundleFormatReader()
  : BundleProducer()
{
}

BundleFormatReader::~BundleFormatReader()
{
}


Object BundleFormatReader::readHeader()
{
  return Object();
}


  //----------------------------------//
 //  ConnectomistBundlesFormaReader  //
//----------------------------------//
//-----------------------------------------------------------------------------
ConnectomistBundlesReader::ConnectomistBundlesReader( const string &fileName )
  : BundleFormatReader()
{
  if ( fileName.size() > 8 &&
       fileName.substr( fileName.size() - 8 ) == ".bundles" ) {
    _fileName = fileName.substr( 0, fileName.size() - 8 );
  } else {
    _fileName = fileName;
  }
}


//-----------------------------------------------------------------------------
ConnectomistBundlesReader::~ConnectomistBundlesReader()
{
}


//-----------------------------------------------------------------------------
void ConnectomistBundlesReader::read()
{
  if( _fileName.length() >= 4
    && _fileName.substr( _fileName.size() - 4 ) == ".trk" )
  {
    TrackvisBundleReader tbr( _fileName );
    tbr.read();

    return;
  }

  // Read header
  Object header;
  header = Object::value( Dictionary() );
  PythonReader out( _fileName + ".bundles" );
  out.read( *header );

  // Check format
  const string format = header->getProperty( "format" )->value<string>();
  if ( format != "bundles_1.0" ) {
    throw runtime_error( string( "invalid format: " ) + format );
  }

  // Check space dimension
  if ( header->hasProperty( "space_dimension" ) &&
       header->getProperty( "space_dimension" )->value<double>() != 3 ) {
    throw runtime_error( 
           string( "only tridimensional bundle are inmplemented" ) );
  }

  // Check data file name
  string dataFileName 
    = header->getProperty( "data_file_name" )->value<string>();
  if ( dataFileName != "*.bundlesdata" ) {
    throw runtime_error( "data_file_name value must be \"*.bundlesdata\"" );
  }

  bool binary = true;
  ByteSwapper byteSwapper( byteOrder() );
  if ( header->hasProperty( "binary" ) &&
       header->getProperty( "binary" )->value<double>() == 0 ) {
    binary = false;
  }
  if ( binary ) {
    int byteOrder = stringToByteOrder( header->getProperty( "byte_order" )->
                                       value<string>() );
    byteSwapper = ByteSwapper( byteOrder );
  }

  int fiberCount;
  try {
    fiberCount = (int) header->getProperty( "curves_count" )->value<double>();
  }
  catch(...) {
    fiberCount = (int) header->getProperty( "fibers_count" )->value<double>();
  }

  ifstream dataFile( ( _fileName + ".bundlesdata" ).c_str(),
                     ( binary ? ios::in | ios::binary : ios::in ) );

  Object bundlesObj = header->getProperty( "bundles" );
  ObjectVector &bundles = bundlesObj->value<ObjectVector>();
  // it should not be a problem to read empty bundles.
  if ( bundles.empty() )
  {
    noMoreBundle();
    return;
    // throw runtime_error( "Bundles required in format bundles_1.0" );
  }

  int bundleCount = 0;
  ObjectVector::const_iterator it = bundles.begin();
  string bundleName = (*it)->value<string>();
  BundleInfo currentBundle( bundleCount++, bundleName );
  if ( ++it == bundles.end() || (*it)->value<double>() != 0 ) {
    throw runtime_error( string( "invalid range for bundle " ) + bundleName +
			 " (should be 0)" ); 
  }
  ++it;
  int fiberRead = 0;
  do {
    startBundle( currentBundle );
    bool noMoreBundle;
    int stopOnFiber;
    if ( it == bundles.end() ) {
      noMoreBundle = true;
      stopOnFiber = fiberCount;
    } else {
      noMoreBundle = false;
      bundleName = (*it)->value<string>();
      ++it;
      stopOnFiber = static_cast<int>( (*it)->value<double>() );
      ++it;
    }

    while( dataFile.good() && fiberRead < stopOnFiber ) {
      FiberInfo fiberInfo( fiberRead );
      startFiber( currentBundle, fiberInfo );
      int fiberSize;
      if ( binary ) {
        byteSwapper.read( dataFile, fiberSize );
      } else {
        dataFile >> fiberSize;
      }
      while( dataFile.good() && fiberSize > 0 ) {
        FiberPoint point;
        if ( binary ) {
          byteSwapper.read( dataFile, point[ 0 ] );
          byteSwapper.read( dataFile, point[ 1 ] );
          byteSwapper.read( dataFile, point[ 2 ] );
        } else {
          dataFile >> point[ 0 ] >> point[ 1 ] >> point[ 2 ];
          char c;
          dataFile.get( c );
          while( dataFile.good() && c != ';' && c != '\n' ) {
            dataFile.get( c );
            if ( c != ' ' ) {
              throw runtime_error( "syntax error in bundles data file" );
            }
          }
        }
        addFiberPoint( currentBundle, fiberInfo, point );
        --fiberSize;
      }
      terminateFiber( currentBundle, fiberInfo );
      ++fiberRead;
    }

    terminateBundle( currentBundle );
    currentBundle = BundleInfo( bundleCount++, bundleName );
  } while( dataFile.good() && fiberRead < fiberCount );
  noMoreBundle();
}


//-----------------------------------------------------------------------------
Object ConnectomistBundlesReader::readHeader()
{
  Object header;
  header = Object::value( Dictionary() );
  PythonReader out( _fileName + ".bundles" );
  out.read( *header );

  vector<float> vs( 3, 1. );
  // get resolution, if available
  try
  {
    Object rx = header->getProperty( "resolutionX" );
    vs[0] = float( rx->getScalar() );
  }
  catch( exception & )
  {
  }
  try
  {
    Object ry = header->getProperty( "resolutionY" );
    vs[1] = float( ry->getScalar() );
  }
  catch( exception & )
  {
  }
  try
  {
    Object rz = header->getProperty( "resolutionZ" );
    vs[2] = float( rz->getScalar() );
  }
  catch( exception & )
  {
  }
  header->setProperty( "voxel_size", vs );

  return header;
}


  //-----------------//
 //  BundleToGraph  //
//-----------------//

//-----------------------------------------------------------------------------
BundleToGraph::BundleToGraph() :
  _meshResult( NULL ), _numPointsInMeshSection( 3 ),
  _normalsAsFiberStartPos( true ), _relatveFiberStartPos( 0, 0, 0 ),
  _autoFiberStartPos( true )
{
  _minimumLength = 0;
  addBuiltinProperty( "minimum_length", _minimumLength );
  _width = 0;
  addBuiltinProperty(  "width", _width );
  addBuiltinProperty(  "num_points_in_mesh_section", _numPointsInMeshSection );
  addBuiltinProperty(  "normals_as_fiber_start_pos", _normalsAsFiberStartPos );
  addBuiltinProperty(  "relative_fiber_start_pos", _relatveFiberStartPos );
  addBuiltinProperty(  "auto_fiber_start_pos", _autoFiberStartPos );
  // callback
  getSignalPropertyChanged().connect(
    ::sigc::mem_fun( *this, &BundleToGraph::parameterChangedSlot ) );
  parametersValueChanged();
}

//-----------------------------------------------------------------------------
BundleToGraph::BundleToGraph( Graph &graph ) :
  _meshResult( &graph ), _numPointsInMeshSection( 3 ),
  _normalsAsFiberStartPos( true ), _relatveFiberStartPos( 0, 0, 0 ),
  _autoFiberStartPos( true )
{
  _minimumLength = 0;
  addBuiltinProperty( "minimum_length", _minimumLength );
  _width = 0;
  addBuiltinProperty(  "width", _width );
  addBuiltinProperty(  "num_points_in_mesh_section", _numPointsInMeshSection );
  addBuiltinProperty(  "normals_as_fiber_start_pos", _normalsAsFiberStartPos );
  addBuiltinProperty(  "relative_fiber_start_pos", _relatveFiberStartPos );
  // callback
  getSignalPropertyChanged().connect(
    ::sigc::mem_fun( *this, &BundleToGraph::parameterChangedSlot ) );
  parametersValueChanged();
}


//-----------------------------------------------------------------------------
void BundleToGraph::parameterChangedSlot( const Object &,
                                          const std::string& propertyName,
                                          const Object& )
{
  if( propertyName == "width"
      || propertyName == "num_points_in_mesh_section" )
    parametersValueChanged();
  else if( propertyName == "relative_fiber_start_pos" )
    _autoFiberStartPos = false;
}

//-----------------------------------------------------------------------------
void BundleToGraph::parametersValueChanged()
{
  if ( _meshResult == NULL ) {
    _rcptrMeshResult.reset( new Graph( "RoiArg" ) );
    _meshResult = _rcptrMeshResult.get();
  }

  // Initialize graph
  if( !_meshResult->hasProperty( "voxel_size" ) )
  {
    vector<float> vs( 3 );
    vs[0] = 1;
    vs[1] = 1;
    vs[2] = 1;
    _meshResult->setProperty( "voxel_size", vs );
  }
  _meshResult->setProperty( "filename_base", string( "*" ) );
  rc_ptr< map< string, map< string, GraphElementCode > > > 
    objmap( new map< string, map< string, GraphElementCode > > );
  GraphElementCode &gec = (*objmap)["roi"]["Tmtktri"];
  gec.id = "Tmtktri";
  gec.attribute = "aims_Tmtktri";
  gec.storageType = GraphElementCode::Global;
  if ( _width > 0 )
    gec.objectType = "Mesh";
  else
    gec.objectType = "Segments";
  gec.dataType = "VOID";
  gec.syntax = "roi";
  gec.local_file_attribute = "Tmtktri_filename";
  gec.global_index_attribute = "Tmtktri_label";
  gec.global_filename = "fibers.mesh";
  gec.global_attribute = gec.attribute;
  _meshResult->setProperty( "aims_objects_table", objmap );

  // Create Mesh section
  _section.clear();
  int i;
  for( i=0; i<_numPointsInMeshSection; ++i )
    _section.push_back( Point2df(
      _width * cos( float(i) / _numPointsInMeshSection * M_PI * 2 ),
      _width * sin( float(i) / _numPointsInMeshSection * M_PI * 2 ) ) );
}


//-----------------------------------------------------------------------------
BundleToGraph::~BundleToGraph()
{
}


//-----------------------------------------------------------------------------
void BundleToGraph::bundleStarted( const BundleProducer &,
                                   const BundleInfo &bundle )
{
  _currentVertex = _meshResult->addVertex( "roi" );
  _current_fibers_count = 0;
  _currentVertex->setProperty( "name", bundle.name() );
  if ( _width > 0 ) {
    _currentMesh = rc_ptr<AimsSurfaceTriangle>( new AimsSurfaceTriangle );
    _currentLines.reset( NULL );
  } else {
    _currentLines = rc_ptr< AimsTimeSurface<2,Void> >
      ( new AimsTimeSurface<2,Void> );
    _currentMesh.reset( NULL );
  }
}


//-----------------------------------------------------------------------------
void BundleToGraph::bundleTerminated( const BundleProducer &,
                                      const BundleInfo & )
{
  shuffleBundle();
  if ( _currentMesh.isNull() ) {
    _currentVertex->setProperty( "aims_Tmtktri", _currentLines );
  } else {
    _currentVertex->setProperty( "aims_Tmtktri", _currentMesh );
  }
  _currentVertex->setProperty( "fibers_count", (int) _current_fibers_count );
}


namespace
{

  // http://stackoverflow.com/questions/6127503/shuffle-array-in-c
  template <typename T>
  void shuffle( T *array, size_t n )
  {
      if (n > 1)
      {
          size_t i;
          for (i = n - 1; i > 0; i--)
          {
              size_t j = (size_t) (rand() / (RAND_MAX / (i+1) + 1));
              T t = array[j];
              array[j] = array[i];
              array[i] = t;
          }
      }
  }


  template <int D> void shuffleFiberPolygons(
    Vertex *vertex, rc_ptr<AimsTimeSurface<D, Void> > mesh )
  {
    Object poly_counts_o;
    try
    {
      poly_counts_o = vertex->getProperty( "poly_counts" );
    }
    catch( ... )
    {
      // no polygons: empty fiber, do nothing
      return;
    }
    vector<size_t> & poly_counts
      = poly_counts_o->value<vector<size_t> >();

    size_t i, j, n = poly_counts.size();

    vector<size_t> order( n );
    size_t *p = &order[0];
    // fill linear order
    for( i=0; i<n; ++i )
      *p++ = i;
    // shuffle order
    shuffle( &order[0], n );

    vector<AimsVector<uint, D> > & poly = mesh->polygon();
    vector<AimsVector<uint, D> > old_poly = poly;
    size_t start, npoly, current_index = 0;

    for( i=0; i<n; ++i )
    {
      j = order[i];
      if( j == 0 )
        start = 0;
      else
        start = poly_counts[j-1];
      npoly = poly_counts[j] - start;
      memcpy( &poly[current_index], &old_poly[start],
              npoly * sizeof( AimsVector<uint, D> ) );
      current_index += npoly;
    }

    vertex->removeProperty( "poly_counts" );
  }

}


void BundleToGraph::shuffleBundle()
{
  if ( _currentLines.isNull() )
    shuffleFiberPolygons( _currentVertex, _currentMesh );
  else
    shuffleFiberPolygons( _currentVertex, _currentLines );
}


//-----------------------------------------------------------------------------
void BundleToGraph::fiberStarted( const BundleProducer &, 
                                  const BundleInfo &,
                                  const FiberInfo & )
{
  _currentCurve.clear();
}


//-----------------------------------------------------------------------------
void BundleToGraph::fiberTerminated( const BundleProducer &,
                                     const BundleInfo &,
                                     const FiberInfo & )
{
  // At least two points required
  if ( _currentCurve.size() < 2 ) return;

  ++_current_fibers_count;

  Object poly_counts_o;
  try
  {
    poly_counts_o = _currentVertex->getProperty( "poly_counts" );
  }
  catch( ... )
  {
    poly_counts_o = Object::value( vector<size_t>() );
    _currentVertex->setProperty( "poly_counts", poly_counts_o );
  }
  vector<size_t> & poly_counts
    = poly_counts_o->value<vector<size_t> >();

  if ( _currentLines.isNull() )
  {
    // Compute mesh composed of triangles
    const float length = _currentCurve.length();
    if ( length >= _minimumLength )
    {
      uint firstVertex = _currentMesh->vertex().size();
      for( unsigned pointIndex = 0;  pointIndex < _currentCurve.size();
           ++pointIndex )
      {
        // Get current point
        const Polyline::Point_t &p = _currentCurve[ pointIndex ];
        // Check bounding boxes
        if ( _boundingbox_min.empty() )
        {
          _boundingbox_min.resize( 3 );
          _boundingbox_max.resize( 3 );
          for( int i = 0; i < 3; ++i )
          {
            _boundingbox_min[ i ] = int( ::floor( p[ i ] ) );
            _boundingbox_max[ i ] = int( ::ceil( p[ i ] ) );
          }
        }
        else
        {
          for( int i = 0; i < 3; ++i )
          {
            if ( _boundingbox_min[ i ] > p[ i ] ) 
              _boundingbox_min[ i ] = int( ::floor( p[ i ] ) );
            if ( _boundingbox_max[ i ] < p[ i ] ) 
              _boundingbox_max[ i ] = int( ::ceil( p[ i ] ) );
          }
        }

        // Build local referential
        int previousIndex = max( int(pointIndex)-1, 0 );
        int nextIndex = min( int(pointIndex)+1, int(_currentCurve.size()-1) );
        const PolylinePolynomialInterpolator::Tangent_t lz = 
          ( _currentCurve[ nextIndex ] - _currentCurve[ previousIndex ] );
        PolylinePolynomialInterpolator::Tangent_t lx;
        if ( lz[ 0 ] ) lx = crossed( lz, PolylinePolynomialInterpolator::
                                     Tangent_t( 0, 1, 0 ) );
        else lx = crossed( lz, PolylinePolynomialInterpolator::
                           Tangent_t( 1, 0, 0 ) );
        lx.normalize();
        float max = fabs( lx[0] );
        unsigned coord = 0;
        for( unsigned j = 1; j < 3; ++j )
        {
          if ( fabs( lx[j] ) > max )
          {
            max = fabs( lx[ j ] );
            coord = j;
          }
        }
        if ( lx[ coord ] < 0 ) lx = -lx;
        const PolylinePolynomialInterpolator::Tangent_t ly = 
          crossed( lz, lx ).normalize();

        // Add section vertices
        for( unsigned j = 0; j < _section.size(); ++j )
        {
          Point3df pointNormal = lx * _section[ j ][ 0 ] 
            + ly * _section[ j ][ 1 ];
          _currentMesh->vertex().push_back( p + pointNormal );
          _currentMesh->normal().push_back( pointNormal.normalize() );
        }
        // Add triangles
        if ( ( _currentMesh->vertex().size() - firstVertex ) > 
             _section.size() )
        {
          for( unsigned i = 0; i < _section.size(); ++i )
          {
            const unsigned jp = _currentMesh->vertex().size() - 
              _section.size() + i;
            const unsigned j = jp - _section.size();
            const unsigned kp = _currentMesh->vertex().size() - 
              _section.size() + ( i + 1 ) % _section.size();
            const unsigned k = kp - _section.size();
            _currentMesh->polygon()
              .push_back( AimsVector<uint,3>( k, jp, j ) );
            _currentMesh->polygon()
              .push_back( AimsVector<uint,3>( k, kp, jp ) );
          }
        }
      }
      poly_counts.push_back( _currentMesh->polygon().size() );
    }

  }
  else
  {
    // Compute "mesh" composed of segments
    const float length = _currentCurve.length();
    Point3df norm = _currentCurve[0];
    if ( length >= _minimumLength )
    {
      for( unsigned pointIndex = 0; pointIndex < _currentCurve.size();
           ++pointIndex )
      {
        // Get current point
        const Point3df p = _currentCurve[ pointIndex ];

        // Check bounding boxes
        if ( _boundingbox_min.empty() )
        {
          _boundingbox_min.resize( 3 );
          _boundingbox_max.resize( 3 );
          for( int i = 0; i < 3; ++i )
          {
            _boundingbox_min[ i ] = int( ::floor( p[ i ] ) );
            _boundingbox_max[ i ] = int( ::ceil( p[ i ] ) );
          }
        }
        else
        {
          for( int i = 0; i < 3; ++i )
          {
            if ( _boundingbox_min[ i ] > p[ i ] ) 
              _boundingbox_min[ i ] = int( ::floor( p[ i ] ) );
            if ( _boundingbox_max[ i ] < p[ i ] ) 
              _boundingbox_max[ i ] = int( ::ceil( p[ i ] ) );
          }
        }
        _currentLines->vertex().push_back( p );
        if( _normalsAsFiberStartPos )
          _currentLines->normal().push_back( norm );
        if ( pointIndex > 0 )
        {
          _currentLines->polygon()
            .push_back( AimsVector<uint,2>( _currentLines->vertex().size()-2, 
                                            _currentLines->vertex().size()-1));
        }
      }
      Object mat = Object::value( Dictionary() );
      _currentLines->header().setProperty( "material", mat );
      if( !_normalsAsFiberStartPos )
        mat->setProperty( "normal_is_direction", 1 );
      mat->setProperty( "shader_color_normals", 1 );

      poly_counts.push_back( _currentLines->polygon().size() );
    }

  }
}


//-----------------------------------------------------------------------------
void BundleToGraph::newFiberPoint( const BundleProducer &,
                                   const BundleInfo &, const FiberInfo &, 
                                   const FiberPoint &point )
{
  _currentCurve.push_back( point );
}


//-----------------------------------------------------------------------------
void BundleToGraph::noMoreBundle( const BundleProducer & )
{
  // Set bounding box attributes
  if ( _boundingbox_min.empty() ) {
    _boundingbox_min.resize( 3 );
    _boundingbox_max.resize( 3 );
    for( int i = 0; i < 3; ++i ) {
      _boundingbox_min[ i ] = 0;
      _boundingbox_max[ i ] = 0;
    }
  }
  if( _normalsAsFiberStartPos && _width == 0 )
  {
    if( _autoFiberStartPos )
    {
      Point3df bmax = Point3df( _boundingbox_max[0],
                                _boundingbox_max[1],
                                _boundingbox_max[2] );
      Point3df bmin = Point3df( _boundingbox_min[0],
                                _boundingbox_min[1],
                                _boundingbox_min[2] );
      _relatveFiberStartPos = bmin + (bmax - bmin ) / 2;
    }
    Graph::iterator iv, ev = _meshResult->end();
    int i, n;
    for( iv=_meshResult->begin(); iv!=ev; ++iv )
    {
      rc_ptr<AimsTimeSurface<2, Void> > mesh;
      if( (*iv)->getProperty( "aims_Tmtktri", mesh ) )
      {
        vector<Point3df> & normals = mesh->normal();
        n = normals.size();
        for( int i=0; i<n; ++i )
        {
          normals[i] -= _relatveFiberStartPos;
          normals[i].normalize();
        }
      }
    }
  }
  vector<float> vs;
  _meshResult->getProperty( "voxel_size", vs );
  vector<int> bbmin( 3 );
  vector<int> bbmax( 3 );
  bbmin[0] = int( floor( _boundingbox_min[0] / vs[0] ) );
  bbmin[1] = int( floor( _boundingbox_min[1] / vs[1] ) );
  bbmin[2] = int( floor( _boundingbox_min[2] / vs[2] ) );
  bbmax[0] = int( ceil( _boundingbox_max[0] / vs[0] ) );
  bbmax[1] = int( ceil( _boundingbox_max[1] / vs[1] ) );
  bbmax[2] = int( ceil( _boundingbox_max[2] / vs[2] ) );
  _meshResult->setProperty( "boundingbox_min", bbmin );
  _meshResult->setProperty( "boundingbox_max", bbmax );
  Object mat = Object::value( Dictionary() );
  _meshResult->setProperty( "material", mat );
  if( _width == 0 )
  {
    if( !_normalsAsFiberStartPos )
      mat->setProperty( "normal_is_direction", 1 );
    mat->setProperty( "shader_color_normals", 1 );
    mat->setProperty( "use_shader", 1 );
    mat->setProperty( "lighting", "no_lighting" );
  }
}


  //-----------------------//
 //  BundleToGraphWriter  //
//-----------------------//

//-----------------------------------------------------------------------------
void BundleToGraphWriter::setFileString( const std::string &fileString )
{
  _fileName = fileNameAndParameters( fileString, *this );
  if ( _fileName.size() > 4 &&
       _fileName.substr( _fileName.size() - 4 ) == ".arg" ) {
    _fileName = _fileName.substr( 0, _fileName.size() - 4 );
  }
  parametersValueChanged();
}


//-----------------------------------------------------------------------------
void BundleToGraphWriter::noMoreBundle( const BundleProducer &producer )
{
  BundleToGraph::noMoreBundle( producer );
  Writer< Graph > writer( _fileName + ".arg" );
  writer.write( *getGraph() );
}


  //------------------//
 //  BundleROISplit  //
//------------------//

//-----------------------------------------------------------------------------
BundleROISplit::BundleROISplit( carto::rc_ptr< RoiIterator > roiIterator,
                                bool keepOriginalBundle ) :
  _keepOriginalBundle( keepOriginalBundle )
{
  _currentPoints.reserve( 100 );

  _roiLabelMap = roiIterator->createLabelMap();
}


//-----------------------------------------------------------------------------
BundleROISplit::~BundleROISplit()
{
}


//-----------------------------------------------------------------------------
void BundleROISplit::bundleStarted( const BundleProducer &, 
                                    const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleROISplit::bundleTerminated( const BundleProducer &,
                                       const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleROISplit::fiberStarted( const BundleProducer &,
                                   const BundleInfo &,
                                   const FiberInfo & )
{
  _currentPoints.clear();
}


//-----------------------------------------------------------------------------
void BundleROISplit::fiberTerminated( const BundleProducer &, 
                                      const BundleInfo &bundle,
                                      const FiberInfo & )
{
  if ( _keepOriginalBundle ) {
    _result[ bundle.name() ][ sorted_pair<int>( _lastLabel, _currentLabel ) ]
      .push_back( _currentPoints );
  } else {
    _result[ "" ][ sorted_pair<int>( _lastLabel, _currentLabel ) ]
      .push_back( _currentPoints );
  }
}


//-----------------------------------------------------------------------------
void BundleROISplit::newFiberPoint( const BundleProducer &, 
                                    const BundleInfo &bundle,
                                    const FiberInfo &, 
                                    const FiberPoint &point )
{
  const int32_t label = _roiLabelMap->at( point );
  
  string bundleName;
  if ( _keepOriginalBundle ) {
    bundleName = bundle.name();
  }
  if ( _currentPoints.empty() ) {
    _lastLabel =_currentLabel = label;
    _currentPoints.push_back( point );
  } else {
    if ( _currentLabel == label ) {
      _currentPoints.push_back( point );
    } else {
      if ( _lastLabel == 0 ) {
        _currentPoints.push_back( point );
        _result[ bundleName ][ sorted_pair<int>( _lastLabel, label ) ]
          .push_back( _currentPoints );
        _currentPoints.clear();
        _currentPoints.push_back( point );
        _lastLabel = _currentLabel = label;
      } else if ( label == 0 ) {
        _result[ bundleName ][ sorted_pair<int>( _lastLabel, _lastLabel ) ]
          .push_back( _currentPoints );
        const FiberPoint lastPoint = _currentPoints.back();
        _currentPoints.clear();
        _currentPoints.push_back( lastPoint );
        _currentPoints.push_back( point );
        _currentLabel = 0;
      } else {
        if ( _currentLabel == 0 ) {
          _currentPoints.push_back( point );
          _result[ bundleName ][ sorted_pair<int>( _lastLabel, label ) ]
            .push_back( _currentPoints );
          _currentPoints.clear();
          _currentPoints.push_back( point );
        } else {
          _result[ bundleName ][ sorted_pair<int>
                                    ( _lastLabel, _currentLabel ) ]
            .push_back( _currentPoints );
          const FiberPoint lastPoint = _currentPoints.back();
          _currentPoints.clear();
          _currentPoints.push_back( lastPoint );
          _currentPoints.push_back( point );
          _result[ bundleName ][ sorted_pair<int>( _lastLabel, label ) ]
            .push_back( _currentPoints );
          _currentPoints.clear();
          _currentPoints.push_back( point );
        }
        _lastLabel = _currentLabel = label;
      }
    }
  }
}


//-----------------------------------------------------------------------------
void BundleROISplit::noMoreBundle( const BundleProducer & )
{
  int bundleId = 0;
  for( map< string, map< pair<int,int>, Fibers > >::iterator
         itob = _result.begin(); itob != _result.end(); ++itob ) {
    for( map< pair<int,int>, Fibers >::iterator itb = itob->second.begin();
       itb != itob->second.end(); ++itb ) {
      string bundleName;
      if ( ! itob->first.empty() ) {
        bundleName = itob->first + ":";
      }
      bundleName += _roiLabelMap->roiNames[ itb->first.first ] + "." +
        _roiLabelMap->roiNames[ itb->first.second ];
      BundleInfo bundleInfo( bundleId++, bundleName );
      startBundle( bundleInfo );
      int fiberId = 0;
      for( Fibers::iterator itf = itb->second.begin(); 
           itf != itb->second.end(); ++itf ) {
        if ( itf->size() >= 2 ) {
          FiberInfo fiberInfo( fiberId++ );
          startFiber( bundleInfo, fiberInfo );
          for( FiberPoints::iterator itp = itf->begin(); itp != itf->end(); 
               ++itp ) {
            addFiberPoint( bundleInfo, fiberInfo, *itp );
          }
          terminateFiber( bundleInfo, fiberInfo );
        }
      }
      terminateBundle( bundleInfo );
      bundleId++;
    }
  }
  BundleProducer::noMoreBundle();
}



  //-------------------//
 //  BundleROISelect  //
//-------------------//

//-----------------------------------------------------------------------------
BundleROISelect::BundleROISelect( carto::rc_ptr< RoiIterator > roiIterator,
                                  const string &rulesFileName,
                                  float defaultROIMinimumOverlap ) :
  _defaultROIMinimumOverlap( defaultROIMinimumOverlap )
{
  _currentPoints.reserve( 100 );

  _roiLabelMap = roiIterator->createLabelMap();

  _crossedROI.resize( _roiLabelMap->roiNames.size() );

  ifstream in( rulesFileName.c_str() );
  while( in.good() ) {
    char c = in.get();
    while ( isspace( c ) ) c = in.get();
    if ( in.good() ) {
      SelectionRule newRule;
      newRule.mustBeCrossed.resize( _roiLabelMap->roiNames.size() );
      for( unsigned i = 0; i < _roiLabelMap->roiNames.size(); ++i ) {
        newRule.mustBeCrossed[ i ] = false;
      }
      if ( c == '"' ) {
        c = in.get();
        while ( c != '"' ) {
          newRule.name += c;
          if ( ! in.good() ) break;
        }
      } else {
        in.unget();
        in >> newRule.name;
      }
      if ( newRule.name == "-" ) newRule.name.clear();
      in >> newRule.min >> newRule.max;
      while( true ) {
        c = in.get();
        while ( c != '\n' && isspace( c ) ) c = in.get();
        if ( c == '\n' || ! in.good() ) break;
        string roiName;
        if ( c == '"' ) {
          c = in.get();
          while ( c != '"' ) {
            roiName += c;
            if ( ! in.good() ) break;
          }
        } else {
          in.unget();
          in >> roiName;
        }
        bool found = false;
        for( unsigned i = 0; i < _roiLabelMap->roiNames.size(); ++i ) {
          if ( _roiLabelMap->roiNames[ i ] == roiName ) {
            newRule.mustBeCrossed[ i ] = true;
            found = true;
            break;
          }
        }
        if ( ! found ) {
          cerr << "WARNING: in bundle selection file, \"" << roiName << "\" is not a valid region name for selected ROI." << endl;
        }
      }
      _selectionRules.push_back( newRule );
    }
  }
}


//-----------------------------------------------------------------------------
BundleROISelect::~BundleROISelect()
{
}


//-----------------------------------------------------------------------------
void BundleROISelect::bundleStarted( const BundleProducer &, 
                                     const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleROISelect::bundleTerminated( const BundleProducer &,
                                        const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void BundleROISelect::fiberStarted( const BundleProducer &,
                                    const BundleInfo &,
                                    const FiberInfo & )
{
  _currentPoints.clear();
  for( vector< uint32_t >::iterator it = _crossedROI.begin();
       it != _crossedROI.end(); ++it ) {
    *it = 0;
  }
}


//-----------------------------------------------------------------------------
void BundleROISelect::fiberTerminated( const BundleProducer &, 
                                       const BundleInfo &bundle,
                                       const FiberInfo & )
{
  // Counting regions crossed
  cartoCondDbgMsg( 2, "BundleROISelect: check fiber" );
  unsigned crossedCount = 0;
  for( unsigned i = 0; i < _roiLabelMap->roiNames.size(); ++i ) {
    if ( float( _crossedROI[ i ] ) / _currentPoints.size() > 
         _defaultROIMinimumOverlap ) {
      ++crossedCount;
      cartoCondDbgMsg( 2, string( "BundleROISelect:   crossing " ) + _roiLabelMap->roiNames[ i ] );
    }
  }

  for( vector< SelectionRule >::const_iterator itRule =
         _selectionRules.begin(); itRule != _selectionRules.end(); ++itRule ) {
    // Check number of region crossed
    if ( crossedCount < itRule->min || 
         ( itRule->max != 0 && crossedCount > itRule->max ) ) {
      continue;
    }

    // Check regions that must be crossed
    bool selected = true;
    for( unsigned i = 0; i < itRule->mustBeCrossed.size(); ++i ) {
      if ( itRule->mustBeCrossed[ i ] && 
           float( _crossedROI[ i ] ) / _currentPoints.size() <= 
         _defaultROIMinimumOverlap ) {
        selected = false;
        break;
      }
    }
    
    // Add fascicle to result
    if ( selected ) {
      if ( ! itRule->name.empty() ) {
        string finalName = itRule->name;
        for( size_t star = finalName.find( "*" ); star != string::npos;
             star = finalName.find( "*" ) ) {
          finalName.replace( star, 1, bundle.name() );
        }
        for( size_t star = finalName.find( "?" ); star != string::npos;
             star = finalName.find( "?" ) ) {
          string namesOfRegionsCrossed = "";
          for( unsigned i = 0; i < _roiLabelMap->roiNames.size(); ++i ) {
            if ( float( _crossedROI[ i ] )  / _currentPoints.size() > 
                 _defaultROIMinimumOverlap ) {
              if ( namesOfRegionsCrossed.empty() ) {
                namesOfRegionsCrossed =  _roiLabelMap->roiNames[ i ];
              } else {
                namesOfRegionsCrossed += ".";
               namesOfRegionsCrossed +=  _roiLabelMap->roiNames[ i ];
              }
            }
          }          
          finalName.replace( star, 1, namesOfRegionsCrossed );
        }
        _result[ finalName ].push_back( _currentPoints );
        cartoCondDbgMsg( 2, string( "BundleROISelect: added to " ) + finalName );
      }
      break;
    }
  }
}


//-----------------------------------------------------------------------------
void BundleROISelect::newFiberPoint( const BundleProducer &, 
                                     const BundleInfo &,
                                     const FiberInfo &, 
                                     const FiberPoint &point )
{
  const int32_t label = _roiLabelMap->at( point );
  _crossedROI[ label ] += 1;
  _currentPoints.push_back( point );
}


//-----------------------------------------------------------------------------
void BundleROISelect::noMoreBundle( const BundleProducer & )
{
  for( std::map< string, vector< FiberPoints > >::const_iterator 
         it = _result.begin(); it != _result.end(); ++it ) {
    BundleInfo bundle( it->first );
    startBundle( bundle );
    for( vector< FiberPoints >::const_iterator it2 =  it->second.begin();
         it2 != it->second.end(); ++it2 ) {
      FiberInfo fiber;
      startFiber( bundle, fiber );
      for( FiberPoints::const_iterator it3 =  it2->begin();
           it3 != it2->end(); ++it3 ) {
        addFiberPoint( bundle, fiber, *it3 );
      }
      terminateFiber( bundle, fiber );
    }
    terminateBundle( bundle );
  }
  BundleProducer::noMoreBundle();
}



  //----------------//
 //  BundleMotion  //
//----------------//

//-----------------------------------------------------------------------------
BundleMotion::BundleMotion( const Motion &motion ) :
  _motion( motion )
{
}


//-----------------------------------------------------------------------------
BundleMotion::BundleMotion( const string &fileName )
{
  MotionReader reader( fileName );
  reader >> _motion;
}


//-----------------------------------------------------------------------------
BundleMotion::~BundleMotion()
{
}


//-----------------------------------------------------------------------------
void BundleMotion::bundleStarted( const BundleProducer &, 
                                  const BundleInfo &bundle )
{
  startBundle( bundle );
}


//-----------------------------------------------------------------------------
void BundleMotion::bundleTerminated( const BundleProducer &,
                                     const BundleInfo &bundle )
{
  terminateBundle( bundle );
}


//-----------------------------------------------------------------------------
void BundleMotion::fiberStarted( const BundleProducer &,
                                 const BundleInfo &bundle,
                                 const FiberInfo &fiber )
{
  startFiber( bundle, fiber );
}


//-----------------------------------------------------------------------------
void BundleMotion::fiberTerminated( const BundleProducer &, 
                                    const BundleInfo &bundle,
                                    const FiberInfo &fiber )
{
  terminateFiber( bundle, fiber );
}


//-----------------------------------------------------------------------------
void BundleMotion::newFiberPoint( const BundleProducer &, 
                                  const BundleInfo &bundle,
                                  const FiberInfo &fiber, 
                                  const FiberPoint &point )
{
  addFiberPoint( bundle, fiber, _motion.transform( point ) );
}


//-----------------------------------------------------------------------------
void BundleMotion::noMoreBundle( const BundleProducer & )
{
  BundleProducer::noMoreBundle();
}



  //------------------//
 //  CurveSelection  //
//------------------//

//-----------------------------------------------------------------------------
CurveSelection::CurveSelection( const double &minimumLength ) :
  _minimumLength( minimumLength )
{
}


//-----------------------------------------------------------------------------
CurveSelection::~CurveSelection()
{
}


//-----------------------------------------------------------------------------
void CurveSelection::bundleStarted( const BundleProducer &, 
				    const BundleInfo &bundle )
{
  startBundle( bundle );
}


//-----------------------------------------------------------------------------
void CurveSelection::bundleTerminated( const BundleProducer &,
				       const BundleInfo &bundle )
{
  terminateBundle( bundle );
}


//-----------------------------------------------------------------------------
void CurveSelection::fiberStarted( const BundleProducer &,
				   const BundleInfo &,
				   const FiberInfo & )
{
  _currentLength = 0;
  _currentPoints.clear();
}


//-----------------------------------------------------------------------------
void CurveSelection::fiberTerminated( const BundleProducer &, 
                                    const BundleInfo &bundle,
                                    const FiberInfo &fiber )
{
  if ( _currentLength >= _minimumLength ) {
    terminateFiber( bundle, fiber );
  }
}


//-----------------------------------------------------------------------------
void CurveSelection::newFiberPoint( const BundleProducer &, 
                                  const BundleInfo &bundle,
                                  const FiberInfo &fiber, 
                                  const FiberPoint &point )
{
  if ( _currentLength >= _minimumLength ) {
    addFiberPoint( bundle, fiber,  point );
  } else {
    if ( _currentPoints.empty() ) {
      _currentPoints.push_back( point );
    } else {
      _currentLength += ( point - _currentPoints.back() ).norm();
      if ( _currentLength >= _minimumLength ) {
	startFiber( bundle, fiber );
	for( FiberPoints::const_iterator it = _currentPoints.begin();
	     it != _currentPoints.end(); ++it ) {
	  addFiberPoint( bundle, fiber,  *it );
	}
	addFiberPoint( bundle, fiber,  point );
      } else {
	_currentPoints.push_back( point );
      }
    }
  }
}


//-----------------------------------------------------------------------------
void CurveSelection::noMoreBundle( const BundleProducer & )
{
  BundleProducer::noMoreBundle();
}


  //--------------------//
 //  CurveOrientation  //
//--------------------//

//-----------------------------------------------------------------------------
CurveOrientation::CurveOrientation() :
  _count( 0 )
{
}


//-----------------------------------------------------------------------------
CurveOrientation::~CurveOrientation()
{
}


//-----------------------------------------------------------------------------
void CurveOrientation::bundleStarted( const BundleProducer &, 
				      const BundleInfo &bundle )
{
  _beginSum = FiberPoint( 0,0,0 );
  startBundle( bundle );
}


//-----------------------------------------------------------------------------
void CurveOrientation::bundleTerminated( const BundleProducer &,
					 const BundleInfo &bundle )
{
  terminateBundle( bundle );
}


//-----------------------------------------------------------------------------
void CurveOrientation::fiberStarted( const BundleProducer &,
				     const BundleInfo &bundle,
				     const FiberInfo &fiber )
{
  _currentPoints.clear();
  startFiber( bundle, fiber );
}


//-----------------------------------------------------------------------------
void CurveOrientation::fiberTerminated( const BundleProducer &, 
					const BundleInfo &bundle,
					const FiberInfo &fiber )
{
  if ( ! _currentPoints.empty() ) {
    bool forward = true;
    if ( _count ) {
      const FiberPoint beginMean = _beginSum / _count;
      if ( ( beginMean -  *_currentPoints.begin() ).norm2() >
           ( beginMean -  *_currentPoints.rbegin() ).norm2() ) {
        forward = false;
        _beginSum += *_currentPoints.rbegin();
      } else {
        _beginSum += *_currentPoints.begin();
      }
      ++_count;
    } else {
      _beginSum = *_currentPoints.begin();
      ++_count;
    }
    
    if ( forward ) {
      for( FiberPoints::const_iterator it = _currentPoints.begin(); 
           it != _currentPoints.end(); ++it ) {
        addFiberPoint( bundle, fiber,  *it );
      }
    } else {
      for( FiberPoints::reverse_iterator it = _currentPoints.rbegin(); 
           it != _currentPoints.rend(); ++it ) {
        addFiberPoint( bundle, fiber,  *it );
      }
    }
  }
  terminateFiber( bundle, fiber );
}


//-----------------------------------------------------------------------------
void CurveOrientation::newFiberPoint( const BundleProducer &, 
                                  const BundleInfo &,
                                  const FiberInfo &, 
                                  const FiberPoint &point )
{
  _currentPoints.push_back( point );
}


//-----------------------------------------------------------------------------
void CurveOrientation::noMoreBundle( const BundleProducer & )
{
  BundleProducer::noMoreBundle();
}




} // namespace aims
