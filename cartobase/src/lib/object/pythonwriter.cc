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

#include <cartobase/object/pythonwriter_d.h>
#include <cartobase/object/object_d.h>
#include <cartobase/object/property.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/wip/io/asciidatasourcetraits.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/datasource/streamdatasource.h>
// debug
//#include <iostream>

using namespace std;
using namespace carto;


PythonWriter::PythonWriter( const std::string& filename, 
			    const SyntaxSet& rules, const HelperSet& helpers )
  : _rules(rules), 
    _datasource( new FileDataSource( filename, 0, DataSource::Write ) ), 
    _singleLine( false ), _catchFunction( 0 )
{
  init( helpers );
  _datasource->open( DataSource::Write );
}


PythonWriter::PythonWriter( const SyntaxSet& rules, const HelperSet& helpers )
  : _rules(rules), _datasource(), _singleLine( false ), _catchFunction( 0 )
{
  init( helpers );
}


PythonWriter::PythonWriter( rc_ptr<DataSource> ds, 
			    const SyntaxSet& rules, const HelperSet& helpers )
  : _rules(rules), _datasource( ds ), _singleLine( false ), _catchFunction( 0 )
{
  init( helpers );
}


PythonWriter::~PythonWriter()
{
  close();
}


void PythonWriter::init( const HelperSet & helpers )
{
  _helpers = helpers;

  if( _helpers.find( "double" ) == _helpers.end() )
    _helpers[ "double" ] = _helpers[ "DOUBLE" ] = &genericHelper<double>;
  {
    ValueObject<double>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<double>;
  }
  if( _helpers.find( "float" ) == _helpers.end() )
    _helpers[ "float" ] = _helpers[ "FLOAT" ] = &genericHelper<float>;
  {
    ValueObject<float>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<float>;
  }
  if( _helpers.find( "int" ) == _helpers.end() )
    _helpers[ "int" ] = _helpers[ "S32" ] = &genericHelper<int>;
  {
    ValueObject<int>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<int>;
  }
  if( _helpers.find( "uint" ) == _helpers.end() )
    _helpers[ "uint" ] = _helpers[ "U32" ] = &genericHelper<unsigned int>;
  {
    ValueObject<unsigned int>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned int>;
  }
  if( _helpers.find( "short" ) == _helpers.end() )
    _helpers[ "short" ] = _helpers[ "S16" ] = &genericHelper<short>;
  {
    ValueObject<short>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<short>;
  }
  if( _helpers.find( "ushort" ) == _helpers.end() )
    _helpers[ "ushort" ] = _helpers[ "U16" ] = &genericHelper<unsigned short>;
  {
    ValueObject<unsigned short>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned short>;
  }
  if( _helpers.find( "char" ) == _helpers.end() )
    _helpers[ "char" ] = _helpers[ "S8" ] = &genericHelper<char>;
  {
    ValueObject<char>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<char>;
  }
  if( _helpers.find( "uchar" ) == _helpers.end() )
    _helpers[ "uchar" ] = _helpers[ "U8" ] = &genericHelper<unsigned char>;
  {
    ValueObject<unsigned char>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<unsigned char>;
  }
  if( _helpers.find( "bool" ) == _helpers.end() )
    _helpers[ "bool" ] = &genericHelper<bool>;
  {
    ValueObject<bool>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<bool>;
  }
  if( _helpers.find( "string" ) == _helpers.end() )
    _helpers[ "string" ] = &genericHelper<string>;
  {
    ValueObject<string>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<string>;
  }
  if( _helpers.find( "dictionary" ) == _helpers.end() )
    _helpers[ "dictionary" ] = &dictHelper<string>;
  {
    ValueObject<Dictionary>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<string>;
  }
  {
    ValueObject<PropertySet>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<string>;
  }
  {
    ValueObject<map<int, Object> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<int>;
  }
  {
    ValueObject<map<float, Object> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &dictHelper<float>;
  }
  {
    ValueObject<map<Object, Object> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &rcDictHelper;
  }
  if( _helpers.find( "list" ) == _helpers.end() )
    _helpers[ "list" ] = &listHelper;
  {
    ValueObject<ObjectVector>	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &listHelper;
  }
  if( _helpers.find( "int_vector" ) == _helpers.end() )
    _helpers[ "int_vector" ] = &PythonWriter::genericSequenceHelper<vector<int> >;
  {
    ValueObject<vector<int> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &PythonWriter::genericSequenceHelper<vector<int> >;
  }
  if( _helpers.find( "uint_vector" ) == _helpers.end() )
    _helpers[ "uint_vector" ] = &PythonWriter::genericSequenceHelper<vector<unsigned int> >;
  {
    ValueObject<vector<unsigned int> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &PythonWriter::genericSequenceHelper<vector<unsigned int> >;
  }

  if( _helpers.find( "float_vector" ) == _helpers.end() )
    _helpers[ "float_vector" ] = &PythonWriter::genericSequenceHelper<vector<float> >;
  {
    ValueObject<vector<float> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &PythonWriter::genericSequenceHelper<vector<float> >;
  }
  if( _helpers.find( "string_vector" ) == _helpers.end() )
    _helpers[ "string_vector" ] = &PythonWriter::genericSequenceHelper<vector<string> >;
  {
    ValueObject<vector<string> >	x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &PythonWriter::genericSequenceHelper<vector<string> >;
  }
  // ### remove after everything has been moved to intN_t/uintN_t  
  if( _helpers.find( "long" ) == _helpers.end() )
    _helpers[ "long" ] = _helpers[ "LONG" ] = &genericHelper<long>;
  {
    ValueObject<long>	   x;
    if( _helpers.find( x.type() ) == _helpers.end() )
      _helpers[ x.type() ] = &genericHelper<long>;
  }
}


void PythonWriter::writeString( DataSource & ds, string x )
{
  unsigned	i, n = x.length();
  for( i=0; i!=n; ++i )
    if( x[i] < 32 ) // or negative (>= 0x80)
    {
      unsigned char x1 = ((unsigned char) x[i]) >> 4;
      if( x1 >= 10 )
        x1 = 'a' + x1 -10;
      else
        x1 = '0' + x1;
      char x2 = x[i] & 0xf;
      if( x2 >= 10 )
        x2 = 'a' + x2 -10;
      else
        x2 = '0' + x2;
      string rep = "\\x";
      rep += x1;
      rep += x2;
      x.replace( i, 1, rep );
      i += 3;
      n += 3;
    }
    else
      switch( (unsigned char) x[i] )
      {
      case '\'':
      case '\\':
        x.insert( i, "\\" );
        ++n;
        ++i;
        break;
      case '\n':
        x.replace( i, 1, "\\n" );
        ++n;
        ++i;
        break;
      default:
        break;
      }
  ds.putch( '\'' );
  AsciiDataSourceTraits<string>::write( ds, x );
  ds.putch( '\'' );
}


void
PythonWriter::open( const string& filename )
{
  _datasource = rc_ptr<DataSource>
    ( new FileDataSource( filename, 0, DataSource::Write ) );
  _datasource->open( DataSource::Write );

  // are files opened?
  if( !_datasource->isOpen() )
    io_error::launchErrnoExcept( _datasource->url() );
}


void
PythonWriter::close()
{
  _datasource.reset( 0 );
}


void PythonWriter::attach( ostream & s )
{
  _datasource.reset( new OStreamDataSource( s ) );
}


string
PythonWriter::name() const
{
  return _datasource->url();
}


bool
PythonWriter::operator!() const
{
  return !_datasource || !_datasource->isOpen();
}


bool
PythonWriter::is_open() const
{
  return _datasource && _datasource->isOpen();
}


void PythonWriter::write( const Object & object, bool writeInternals, 
			  bool writevariable )
{
  write( *object, writeInternals, writevariable );
}


void PythonWriter::write( const GenericObject & object, bool writeInternals, 
			  bool writevariable )
{
  if( !_datasource )
    throw file_error( "no IO source\n" );
  if( !_datasource->isOpen() )
    _datasource->open( DataSource::Write );
  if( writevariable )
    AsciiDataSourceTraits<string>::write( *_datasource, "attributes = " );
  write( object, 1, "", "", writeInternals );
  if( !_singleLine )
    _datasource->putch( '\n' );
}


bool PythonWriter::isInternal( const string & syntax, const string & semantic )
{
  if( !semantic.empty() )
    {
      SyntaxSet::const_iterator	is;
      if( syntax.empty() )
	is = _rules.find( "__generic__" );
      else
	is = _rules.find( syntax );
      if( is != _rules.end() )
	{
	  Syntax::const_iterator	ie = is->second.find( semantic );
	  if( ie != is->second.end() 
	      && ie->second.internal )
	    return( true );
	}
    }
  return( false );
}


void PythonWriter::write( const GenericObject & object, int indent, 
			  const string & syntax, const string & semantic, 
			  bool writeInternals )
{
  try
  {
    if( !is_open() )
      throw stream_fail_error( "cannot write", name() );

    if( &object == 0 )	// "None" object
      {
        AsciiDataSourceTraits<string>::write( *dataSource(), "None" );
        if( !is_open() )
          io_error::launchErrnoExcept();
        return;
      }

    string	type = object.type();
    if( type == "object" )
      {
        // Object: de-reference it
        write( *object.value<Object>(), indent, syntax, semantic, 
              writeInternals );
        return;
      }
    // cout << "type: " << type << endl;

    //	try to find type in syntax if semantic / syntaxic atts are provided
    if( !semantic.empty() )
      {
        SyntaxSet::const_iterator	is;
        if( syntax.empty() )
          is = _rules.find( "__generic__" );
        else
          is = _rules.find( syntax );
        if( is != _rules.end() )
          {
            Syntax::const_iterator	ie = is->second.find( semantic );
            if( ie != is->second.end() )
              {
                /* cout << "syntax found, syntax: " << syntax << ", semantic: "
                  << semantic << " -> " << type << endl; */
                type = ie->second.type;
              }
          }
      }

    //	try to find type in helpers
    HelperSet::const_iterator	ih = _helpers.find( type );
    if( ih != _helpers.end() )
      {
        (ih->second)( object, *this, indent, writeInternals );
        if( !is_open() )
          io_error::launchErrnoExcept();
        return;	// OK
      }

    else if( object.isScalar() )
      type = "double";
    if( object.isString() )
      type = "string";
    else if( object.isDictionary() )
      type = "dictionary";
    else if( object.isIterable() )
      type = "list";
    // cout << "type fallback for " << object.type() << ": " << type << endl;

    ih = _helpers.find( type );
    if( ih != _helpers.end() )
      {
        (ih->second)( object, *this, indent, writeInternals );
        if( !is_open() )
          io_error::launchErrnoExcept();
        return;	// OK
      }

    // no helper found
    throw runtime_error( string( "Can't write data of type " ) + type
                        + " in " + name() );
  }
  catch( exception & e )
  {
    if( _catchFunction )
      _catchFunction( *this, e, object );
    else
      throw;
  }
}


void PythonWriter::setCatchFunction( CatchFunction f )
{
  _catchFunction = f;
}

const PythonWriter::HelperSet & PythonWriter::helpers() const
{
   return _helpers;
}


PythonWriter::HelperSet & PythonWriter::helpers()
{
   return _helpers;
}


const SyntaxSet & PythonWriter::syntaxes() const
{
   return _rules;
}


SyntaxSet & PythonWriter::syntaxes()
{
   return _rules;
}


INSTANTIATE_GENERIC_OBJECT_TYPE( PythonWriter::Helper )
INSTANTIATE_GENERIC_OBJECT_TYPE( PythonWriter::HelperSet )
