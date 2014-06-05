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
 *  Header class
 */
#include <cartobase/object/object_d.h>
#include <aims/data/pheader.h>
#include <aims/io/iooptions.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <cartobase/exception/file.h>
#include <cartobase/uuid/uuid.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/writer/pythonwriter.h>

using namespace aims;
using namespace aims::internal;
using namespace carto;
using namespace std;


namespace carto
{
INSTANTIATE_GENERIC_OBJECT_TYPE( PythonHeader );
} // namespace carto


PythonHeader::~PythonHeader()
{
}


PythonHeader::PythonHeader()
  : PythonHeaderType( *new PropertySet, true ), Header()
{
  /*
  cout << "PythonHeader::PythonHeader, this : " << this << endl;
  cout << "props: " << &getValue() << endl;
  */
}


PythonHeader::PythonHeader( const internal::PythonHeaderType & x )
  : PythonHeaderType
  ( *new PropertySet( const_cast<internal::PythonHeaderType &>
                      ( x ).getValue() ), true ), 
    Header()
{
  // warning: Dictionary are all ref-counted: no deep copy is made !
  // maybe we should change this !
  /*
  cout << "PythonHeader::PythonHeader, this : " << this << endl;
  cout << "props: " << &getValue() << endl;
  */
}


PythonHeader::PythonHeader( const PythonHeader & x )
  : 
    RCObject(), 
    Interface(), 
    StringInterface(), 
    ScalarInterface(), 
    SizeInterface(), 
    IterableInterface(), 
    ArrayInterface(), 
    DynArrayInterface(), 
    DictionaryInterface(), 
    IteratorInterface(), 
    DictionaryIteratorInterface(), 
    PythonHeaderType( 
      *new PropertySet( const_cast<PythonHeader &>( x ).getValue() ), 
      true ),
    Header()
{
  // warning: Dictionary are all ref-counted: no deep copy is made !
  // maybe we should change this !
  /*
  cout << "PythonHeader::PythonHeader, this : " << this << endl;
  cout << "props: " << &getValue() << endl;
  */
}


PythonHeader::PythonHeader( Headered & x )
  : PythonHeaderType( x.header(), false ), Header()
{
  // warning: Dictionary are all ref-counted: no deep copy is made !
  // maybe we should change this !
  /*
  cout << "PythonHeader::PythonHeader, this : " << this << endl;
  cout << "props: " << &getValue() << endl;
  */
}


const char* PythonHeader::id() const
{
  static const char	pheader_type[] = "PYTHON_HEADER";
  return pheader_type;
}


carto::SyntaxSet* PythonHeader::syntax()
{
  return &DataSourceInfoLoader::minfSyntax();
}


bool PythonHeader::readMinf( const string &filename )
{
  try
  {
    Reader<GenericObject> r( filename );
    Object options = Object::value( Dictionary() );
    options->setProperty( "syntaxset", rc_ptr<SyntaxSet>( syntax(), true ) );
    bool hasbs = hasProperty( "byte_swapping" );
    int bs = 0;
    if( hasbs )
      hasbs = getProperty( "byte_swapping", bs );
    r.setOptions( options );
    r.read( *this );
    if( hasbs )
      setProperty( "byte_swapping", bs );
    else if( hasProperty( "byte_swapping" ) )
      removeProperty( "byte_swapping" );
    return true;
  }
  catch( file_error & ) {
  }
  catch( io_error & ) {
  }
  catch( exception & e ) {
    cerr << e.what() << endl;
  }
  catch( ... ) {
    cerr << "Unknown exception" << endl;
  }
  return false;
}


bool PythonHeader::writeMinf( const string & filename )
{
  if( hasProperty( "byte_swapping" ) )
    removeProperty( "byte_swapping" );
  if( hasProperty( "possible_data_types" ) )
    removeProperty( "possible_data_types" );
  bool	wm = true;
  IOOptions::ioOptions()->getProperty( "writeMinf", wm );
  if( !wm )
    return true;
  return ((const PythonHeader*) this)->writeMinf( filename );
}


bool PythonHeader::writeMinf( const string & filename ) const
{
  // we need a local modifiable copy
  PythonHeader	ph( *this );
  if( ph.hasProperty( "byte_swapping" ) )
    ph.removeProperty( "byte_swapping" );
  if( ph.hasProperty( "possible_data_types" ) )
    ph.removeProperty( "possible_data_types" );

  // UUID handling: try first to reload an existing .minf with same name
  string		uuid;
  Object options = Object::value( Dictionary() );
  options->setProperty( "syntaxset", rc_ptr<SyntaxSet>( syntax(), true ) );
  if( !getProperty( "uuid", uuid ) )
    try
    {
      Reader<GenericObject> r( filename );
      r.setOptions( options );
      Object oh( r.read() );
      oh->getProperty( "uuid", uuid );
    }
    catch( exception & )
    {
    }
  if( !uuid.empty() )
    ph.setProperty( "uuid", uuid );

  try
  {
    /* Writer doesn't support options yet. Until then, use the low-level
       PythonWriter

    Writer<GenericObject> w( filename );
    w.setOptions( options );
    w.write( ph );
    */

    PythonWriter	pw( filename, *syntax() );
    pw.write( ph );
    return true;
  }
  catch( file_error & )
  {
  }
  catch( exception & e )
  {
    cerr << "writeMinf: " << e.what() << endl;
  }
  return( false );
}


Header* PythonHeader::cloneHeader( bool keepUuid ) const
{
  /*
  cout << "PythonHeader::cloneHeader: " << this << ", " << d->owner 
       << ", props: " << &const_cast<PythonHeader *>(this)->getValue() 
       << endl;
  PythonHeader	*ph = new PythonHeader( *this );
  cout << "PythonHeader::cloneHeader after cloning: " << ph << "," 
       << ph->d->owner << ", props: " << &ph->getValue() << endl;
  return ph;
  */
  PythonHeader* ph = new PythonHeader( *this );
  // remove uuid property to keep it unique
  if( !keepUuid && ph->hasProperty( "uuid" ) )
    ph->removeProperty( "uuid" );

  return ph;
}


void PythonHeader::copy( const PythonHeader & ph, bool keepUuid )
{
  /*
  cout << "PythonHeader::copy this: " << this << ", other: " << &ph << endl;
  cout << "this:" << endl;
  PythonWriter	pw;
  pw.attach( cout );
  pw.write( *this );
  cout << "other:" << endl;
  pw.write( ph );
  */

  if( this != &ph )
    copyProperties( Object::reference( const_cast<PythonHeader &>( ph )
      .value<PropertySet>() ) );
  if( !keepUuid && hasProperty( "uuid" ) )
    removeProperty( "uuid" );
}


PythonHeader & PythonHeader::operator = ( const PythonHeader & x )
{
  clearProperties();
  copy( x );
  return *this;
}
