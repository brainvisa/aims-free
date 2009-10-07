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

#include <cartobase/object/object_d.h>
#include <cartobase/object/pythonreader_d.h>
#include <cartobase/object/syntobject.h>
#include <cartobase/exception/parse.h>
#include <cartobase/exception/file.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/stream/attachable_cuifstream.h>
#include <cartobase/wip/io/itemreader.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/datasource/streamdatasource.h>
#include <stdio.h>
#include <vector>
#include <iostream>

using namespace std;
using namespace carto;


GenericObject* PythonReader::propertySetHelper(GenericObject *, const string &, 
                                         PythonReader & r)
{
  string                synt;
  DataSource    & ds = *r.dataSource();
  char      c;

  r.skipWhile( " \t\n\\\r" );
  if( r.eof() )
    io_error::launchErrnoExcept( ds.url() );
  c = ds.getch();
  if( ds.eof() )
    io_error::launchErrnoExcept( ds.url() );
  if ( c != '{' ) {
    cerr << "Attempt to read a dictionary not starting with '{'" << endl;
    return 0;
  }
  r.skipWhile( " \t\n\\\r" );
  if( r.eof() )
    io_error::launchErrnoExcept( ds.url() );
  c = ds.getch();
  if( ds.eof() )
    io_error::launchErrnoExcept( ds.url() );
  if( c == 'u' )
  {
    c = ds.getch();
    if( ds.eof() )
      io_error::launchErrnoExcept( ds.url() );
  }
  // cout << "dictHelper c = " << c << endl;

  // expect attribute name
  switch( c )
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
    case '-':
    case '+':
    case '\'':  // string identifier
    case '"': 
      {
        ds.ungetch( c );
        GenericObject   *obj;

        if( r.readSyntax( synt ) )
          obj = new SyntaxedObject<PropertySet>( synt );
        else
          obj = new ValueObject<PropertySet>;

        try
          {
            r.readDictionary2( *obj );
          }
        catch( exception & e )
          {
            delete obj;
            throw;
          }

        return( obj );
      }
      break;
    case '}':
      return new ValueObject<PropertySet>;
      break;
     }
  throw parse_error( "unrecognized tag start", string() + c, ds.url(), 
                     r.line() );
  return 0;
}


GenericObject* PythonReader::dictHelper( GenericObject *, const string &, 
                                         PythonReader & r )
{
  string                synt;
  DataSource    & ds = *r.dataSource();
  char      c;

  r.skipWhile( " \t\n\\\r" );
  if( r.eof() )
    io_error::launchErrnoExcept( ds.url() );
  c = ds.getch();
  if( ds.eof() )
    io_error::launchErrnoExcept( ds.url() );
  if ( c != '{' ) {
    cerr << "Attempt to read a dictionary not starting with '{'" << endl;
    return 0;
  }
  r.skipWhile( " \t\n\\\r" );
  if( r.eof() )
    io_error::launchErrnoExcept( ds.url() );
  c = ds.getch();
  if( ds.eof() )
    io_error::launchErrnoExcept( ds.url() );
  if( c == 'u' ) // unicode string
  {
    c = ds.getch();
    if( ds.eof() )
      io_error::launchErrnoExcept( ds.url() );
    if( c != '\'' && c != '"' )
      throw parse_error( "unrecognized tag start", string() + 'u' + c,
                         ds.url(), r.line() );
  }
  // cout << "dictHelper c = " << c << endl;

  // expect attribute name
  switch( c )
    {
    case '\'':  // string identifier
    case '"': 
      {
        ds.ungetch( c );
        ValueObject<Dictionary> *obj;

        if( r.readSyntax( synt ) )
          obj = new SyntaxedObject<Dictionary>( synt );
        else
          obj = new ValueObject<Dictionary>;

        try
          {
            r.readDictionary2( *obj );
          }
        catch( exception & e )
          {
            delete obj;
            throw;
          }

        return( obj );
      }
      break;
    case '}':
      return new ValueObject<Dictionary>;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
    case '-':
    case '+':
    case 'i':
    case 'I':
    case 'n':
    case 'N':
      {
        ds.ungetch( c );
        ValueObject<IntDictionary>  *obj;

        obj = new ValueObject<IntDictionary>;

        try
          {
            r.readIntDictionary2( *obj );
          }
        catch( exception & e )
          {
            delete obj;
            throw;
          }

        return( obj );
      }
      break;

    }

  throw parse_error( "unrecognized tag start", string() + c, ds.url(), 
                     r.line() );
  return 0;
}

PythonReader::PythonReader( const string& filename, const SyntaxSet& rules,
                const HelperSet& helpers )
  : d( new Private( rules,
         rc_ptr<DataSource>( new IStreamDataSource( rc_ptr<istream>(
             new cuifstream( filename.c_str(), ios::in | ios::binary ) ),
         filename ) ) ) )
{
  // cout << "PythonReader on file: " << filename << endl;
  // are files opened?
  if( !d->datasource->isOpen() )
    throw file_error( "Can't open file for reading", d->datasource->url() );
  static_cast<cuifstream &>( static_cast<IStreamDataSource *>
      ( d->datasource.get() )->stream() ).enableUncomment( false );

  init( helpers );
}


PythonReader::PythonReader( const SyntaxSet& rules, const HelperSet& helpers )
  : d( new Private( rules ) )
{
  // cout << "pythonreader with no source\n";
  init( helpers );
}


PythonReader::PythonReader( rc_ptr<DataSource> ds, const SyntaxSet& rules,
                const HelperSet& helpers )
  : d( new Private( rules, ds ) )
{
  init( helpers );
}


void PythonReader::init( const HelperSet& helpers )
{
  d->helpers = helpers;

  if (d->helpers.find( "double" ) == d->helpers.end())
    d->helpers[ "double" ] = &genericHelper<double>;
  if (d->helpers.find( "float" ) == d->helpers.end())
    d->helpers[ "float" ] = &genericHelper<float>;
  if (d->helpers.find( "int" ) == d->helpers.end())
    d->helpers[ "int" ] = &genericHelper<int>;
  if (d->helpers.find( "uint" ) == d->helpers.end())
    d->helpers[ "uint" ] = &genericHelper<unsigned int>;
  if (d->helpers.find( "short" ) == d->helpers.end())
    d->helpers[ "short" ] = &genericHelper<short>;
  if (d->helpers.find( "ushort" ) == d->helpers.end())
    d->helpers[ "ushort" ] = &genericHelper<unsigned short>;
  if (d->helpers.find( "char" ) == d->helpers.end())
    d->helpers[ "char" ] = &genericHelper<char>;
  if (d->helpers.find( "uchar" ) == d->helpers.end())
    d->helpers[ "uchar" ] = &genericHelper<unsigned char>;
  if (d->helpers.find( "bool" ) == d->helpers.end())
    d->helpers[ "bool" ] = &genericHelper<bool>;
  if (d->helpers.find("string") == d->helpers.end())
    d->helpers["string"] = &genericHelper<string>;
  if (d->helpers.find("list") == d->helpers.end())
    d->helpers["list"] = &listHelper;
  if (d->helpers.find("dictionary") == d->helpers.end())
    d->helpers["dictionary"] = &dictHelper;
  if (d->helpers.find("PropertySet") == d->helpers.end())
    d->helpers["PropertySet"] = &propertySetHelper;
  if (d->helpers.find("int_vector") == d->helpers.end())
    d->helpers["int_vector"] = &PythonReader::genericSequenceHelper<vector<int> >;
  if (d->helpers.find("uint_vector") == d->helpers.end())
    d->helpers["uint_vector"] = &PythonReader::genericSequenceHelper<vector<unsigned int> >;
  if (d->helpers.find("float_vector") == d->helpers.end())
    d->helpers["float_vector"] = &PythonReader::genericSequenceHelper<vector<float> >;
  if (d->helpers.find("string_vector") == d->helpers.end())
    d->helpers["string_vector"] = &PythonReader::genericSequenceHelper<vector<string> >;
  if (d->helpers.find("None") == d->helpers.end())
    d->helpers["None"] = &noneHelper;

  // Some helpers compatible with DataTypeCode::dataType()
  if (d->helpers.find( "U8" ) == d->helpers.end())
    d->helpers[ "U8" ] = &genericHelper<uint8_t>;
  if (d->helpers.find( "S8" ) == d->helpers.end())
    d->helpers[ "S8" ] = &genericHelper<int8_t>;
  if (d->helpers.find( "U16" ) == d->helpers.end())
    d->helpers[ "U16" ] = &genericHelper<uint16_t>;
  if (d->helpers.find( "S16" ) == d->helpers.end())
    d->helpers[ "S16" ] = &genericHelper<int16_t>;
  if (d->helpers.find( "U32" ) == d->helpers.end())
    d->helpers[ "U32" ] = &genericHelper<uint32_t>;
  if (d->helpers.find( "S32" ) == d->helpers.end())
    d->helpers[ "S32" ] = &genericHelper<int32_t>;
  if (d->helpers.find( "unsigned integer" ) == d->helpers.end())
    d->helpers[ "unsigned integer" ] = &genericHelper<unsigned int>;
  if (d->helpers.find( "integer" ) == d->helpers.end())
    d->helpers[ "integer" ] = &genericHelper<int>;
  if (d->helpers.find( "FLOAT" ) == d->helpers.end())
    d->helpers[ "FLOAT" ] = &genericHelper<float>;
  if (d->helpers.find( "DOUBLE" ) == d->helpers.end())
    d->helpers[ "DOUBLE" ] = &genericHelper<double>;
  if (d->helpers.find( "boolean" ) == d->helpers.end())
    d->helpers[ "boolean" ] = &genericHelper<bool>;
}


PythonReader::~PythonReader()
{
  delete d;
}


void
PythonReader::open(const string& filename)
{
  attach( filename );
}


void
PythonReader::attach( const string& filename )
{
  cuifstream  *cs = new cuifstream( filename.c_str(), ios::in | ios::binary );
  cs->enableUncomment( false );
  d->datasource.reset
    ( new IStreamDataSource( rc_ptr<istream>( cs ), filename ) );

  // are files opened?
  if( !d->datasource->isOpen() )
    throw file_error( filename );
}


void
PythonReader::attach( rc_ptr<DataSource> ds )
{
  d->datasource = ds;
}


void PythonReader::attach( istream & s, int line_num )
{
  //_stream.attach( s, line_num );
  attachable_cuifstream *c = new attachable_cuifstream;
  c->enableUncomment( false );
  c->unsetf( ios::skipws );
  c->attach( s, line_num );
  d->datasource 
    = rc_ptr<DataSource>( new IStreamDataSource( rc_ptr<istream>( c ) ) );
}


void
PythonReader::close()
{
  d->datasource->close();
  d->datasource.reset( 0 );
}


string
PythonReader::name() const
{
  if( !d->datasource )
    return string();
  return d->datasource->url();
}


int
PythonReader::line() const
{
  if( !d->datasource )
    return 0;
  IStreamDataSource *sds 
    = dynamic_cast<IStreamDataSource *>( d->datasource.get() );
  if( !sds )
    return 0;
  cuifstream    *cs = dynamic_cast<cuifstream *>( &sds->stream() );
  if( !cs )
    return 0;
  return cs->line();
}


bool
PythonReader::operator!() const
{
  if( !d->datasource )
    return false;
  return !d->datasource->isOpen();
}


bool
PythonReader::is_open() const
{
  return d->datasource && d->datasource->isOpen();
}


bool
PythonReader::eof() const
{
  return d->eof || !d->datasource || !d->datasource->isOpen();
}


string
PythonReader::readUntil( const string& s )
{
  if( eof() )
    return( "" );

  string token;

  int c = d->datasource->getch();
  while( c != EOF && c != '\0' && s.find_first_of(c) == string::npos )
  {
    token += static_cast<char>(c);
    c = d->datasource->getch();
  }
  if( c == '\0' )
    d->eof = true;
  else if( c != EOF )
    d->datasource->ungetch( c );

  /*if( _stream.eof() )
    throw file_error( string( "EOF found" ) + name() );*/

  return token; 
}


void
PythonReader::skipUntil(const string& s)
{
  if( eof() )
    return;

  int c = d->datasource->getch();
  while( c != EOF && c != '\0' && s.find_first_of(c) == string::npos )
  {
    // uncomment
    if( c == '#' )
      do c = d->datasource->getch();
      while( c != EOF && c!= '\n' );
    c = d->datasource->getch();
  }
  if( c == '\0' )
    d->eof = true;
  else if( c != EOF )
    d->datasource->ungetch( c );
  /*if( _stream.eof() )
    throw file_error( string( "EOF found" ) + name() );*/
}


string
PythonReader::readWhile(const string& s)
{
  if( eof() )
    return( "" );

  string token;

  int c = d->datasource->getch();
  while (c != EOF && c != '\0' && s.find_first_of(c) != string::npos)
  {
    token += static_cast<char>(c);
    c = d->datasource->getch();
  }
  if( c == '\0' )
    d->eof = true;
  else
    d->datasource->ungetch( c );

  /*if( _stream.eof() )
    throw file_error( string( "EOF found" ) + name() );*/

  return token; 
}


unsigned
PythonReader::skipWhile(const string& s)
{
  unsigned  n = 0;
  if( eof() )
    return n;

  int c = d->datasource->getch();
  ++n;

  // uncomment
  if( c == '#' )
    do c = d->datasource->getch();
    while( c != EOF && c != '\0' && c!= '\n' );

  if( c == '\0' )
  {
    d->eof = true;
    return n;
  }

  while( c != EOF && c != '\0'
         && ( c == '#' || s.find_first_of(c) != string::npos ) )
  {
    // uncomment
    if( c == '#' )
      do c = d->datasource->getch();
      while( c != EOF && c != '\0' && c!= '\n' );
    c = d->datasource->getch();
    ++n;
  }
  if( c == '\0' )
  {
    d->eof = true;
    return n;
  }
  else
  {
    d->datasource->ungetch( c );
    --n;
  }

  /*if( _stream.eof() )
    throw file_error( string( "EOF found" ) + name() );*/
  return n;
}


GenericObject* PythonReader::read()
{
  // cout << "PythonReader::read()\n";
  /*string var =*/ readVariableName();
  return( read( 0, "" ) );
}


string PythonReader::readVariableName()
{
  if( !d->datasource )
    throw file_error( "no IO source", "" );
  if( !is_open() )
    d->datasource->open( DataSource::Read );
  if( !is_open() )
    throw file_error( "file not open", name() );

  skipWhile( " \t\n\\\r" );
  if( eof() )
    throw file_error( "EOF", name() );

  // var name
  // string x = readUntil( "= \t\n\\\r" );
  /* to avoid confusion with other files, we ensure the variable name
  is the hard-coded string 'attributes' for now. */
  static const string hardcoded = "attributes";
  unsigned  i, n = hardcoded.length();
  int c;
  for( i=0; i<n; ++i )
  {
    c = d->datasource->getch();
    if( c != hardcoded[i] )
      throw invalid_format_error( "MINF not starting with 'attributes ='",
                                  name() );
  }

  if( eof() )
    throw file_error( "EOF", name() );
  skipWhile( " \t\n\\\r" );
  if( eof() )
    throw file_error( "EOF", name() );
  c = d->datasource->getch();
  if( c != '=' )
    throw runtime_error( name() + ": Unrecognized syntax, line " 
                         + lineString() + " (expecting 'variable = ...')" );
  return hardcoded;
}


GenericObject* PythonReader::read( GenericObject* object, 
                                   const string& semantic )
{
  // cout << "PythonReader::read " << object << ", \"" << semantic << "\"\n";
  if( !d->datasource )
    throw file_error( "no IO source", "" );
  if( !is_open() )
    d->datasource->open( DataSource::Read );
  if( !is_open() )
    throw file_error( "file not open", name() );

  d->eof = false;
  if( eof() )
    throw file_error( string( "EOF: line " ) + lineString(), name() );

  // cout << "source OK\n";

  string    synt;
  bool      genfallback = false;
  if( object )
  {
    SyntaxedInterface *si = object->getInterface<SyntaxedInterface>();
    if( si && si->hasSyntax() )
      synt = si->getSyntax();
    if( synt.empty() )
      synt = object->type();
  }
  // cout << "read syntax: " << synt << ", semantic: " << semantic << endl;

  //    try to use syntax
  /*
  SyntaxSet::const_iterator i, f;
  for (i = d->rules.begin(), f = d->rules.end(); i != f; ++i)
    std::cout << (*i).first << std::endl;*/
  if( !semantic.empty() )
    {
      // if this semantic attribute does have associated I/O properties...
      SyntaxSet::const_iterator syntax = d->rules.find( synt );
      if( syntax == d->rules.end() && !genfallback )
        {
          syntax = d->rules.find( "__generic__" );
          genfallback = true;
        }
      if( syntax != d->rules.end() )
        {
          synt = syntax->first; // update according to real syntax
          Syntax::const_iterator property = syntax->second.find( semantic );
          if( property == syntax->second.end() && !genfallback )
            {
              genfallback = true;
              SyntaxSet::const_iterator syntax2 = d->rules.find( "__generic__" );
              if( syntax2 != d->rules.end() )
                {
                  syntax = syntax2;
                  property = syntax->second.find( semantic );
                }
            }
          if( semantic != "__syntax__" && property == syntax->second.end() )
          {
            property = syntax->second.find( "__fallback__" );
          }
          if( property != syntax->second.end() )
            {
              // cout << "semantic found\n";
              // make sure this attribute is read for the first time
              /* this should be checked in dictionaries only
                 if( object.hasAttribute(semantic) )
                 {
                 char buffer[10];
                 sprintf( buffer, "%d", _stream.line() );
                 throw runtime_error( string( "Attribute already read for "
                           "attribute " ) + semantic
                 + " in " + name() + ", line " 
                 + buffer );
                 }
              */
              // find the associated I/O function
              HelperSet::const_iterator h =
                d->helpers.find( property->second.type );
              if( h != d->helpers.end() )
                // call the associated I/O function
                return( (h->second)( object, semantic, *this ) );
            }
        }
    }

  // not in syntax: try to determine type from contents
  // cout << "not in syntax or no helper\n";

  skipWhile( " \t\n\\\r" );
  int   c = d->datasource->getch();
  if( c == 'u' ) // unicode string
  {
    c = d->datasource->getch();
    if( c != '\'' && c != '"' )
    {
      d->datasource->ungetch( c );
      d->datasource->ungetch( c );
      throw runtime_error( name() + ": Unrecognized syntax, line "
          + lineString() );
    }
  }
  d->datasource->ungetch( c );

  string    type;

  switch( c )
    {
    case '{':   // dictionary
      type = "dictionary";
      break;
    case '[':   // list
    case '(':   // tuple
      type = "list";
      break;
    case '\"':  // string
    case '\'':
      type = "string";
      break;
    case '0':   // number
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
    case '-':
    case '+':
    case 'i': // inf
    case 'I': // Inf/INF
    case 'n': // nan
      type = "double";
      break;
    case 'N':
      {
        const string    none = "None";
        unsigned    count = 0;
        while( is_open() && !eof() 
               && ( c = d->datasource->getch() ) == none[count] )
          ++count;
        if( count != none.length() )
          {
            if( count == 1 && ( c == 'a' || c == 'A' ) )
              {
                type = "double"; // try NaN
                d->datasource->ungetch( c );
                break;
              }
            d->datasource->ungetch( c );
            type = "";
            break;
          }
        type = "None";
      }
      break;
    default:
      type = "";
      break;
    }

  // cout << "PythonReader::read " << semantic << ", type = " << type << endl;
  if( type.empty() )
    throw runtime_error( name() + ": Unrecognized syntax, line "
                         + lineString() );

  // now try to find an appropriate helper function

  HelperSet::const_iterator h = d->helpers.find( type );
  if( h != d->helpers.end() )
    {
      // make new rule (should we really do that ?)
      /*
      if( !semantic.empty() && synt != "__generic__" )
        {
          Semantic & rule = d->rules[ synt ][ semantic ];
          rule.type = type;
          rule.needed = false;
        }
      */
      // call the associated I/O function
      return( (h->second)( object, semantic, *this ) );
    }

  // failed
  char buffer[10];
  sprintf( buffer, "%d", line() );
  throw runtime_error( string( "Can't read attribute " ) + semantic 
                       + " of type " + type + " in " + name() 
                       + ", line " + buffer );
}


string PythonReader::lineString() const
{
  ostringstream ss;
  ss << line();
  return ss.str();
}


void PythonReader::read( Object & obj )
{
  if( obj.get() )
    read( *obj );
  else
    {
      GenericObject *go = read();
      obj = go;
    }
}


void PythonReader::read( GenericObject & obj )
{
  /* --- too restrictive test (PropertySet) ---
  ValueObject<Dictionary> 
    *dic = dynamic_cast<ValueObject<Dictionary> *>( &obj );
  if( !dic )
    throw runtime_error( string( "Can't read allocated object of type " ) 
             + obj.type() );
  */

  // better test in the future but always true right now
  /*
  if( !obj.getInterface<DictionaryInterface>() )
    throw runtime_error( string( "Dictionary can't be read as an object of " 
                                 "type " ) + obj.type() );
  */

  /*string var =*/ readVariableName();
  readDictionary( obj );
}


string PythonReader::readString( char sep, unsigned & n )
{
  DataSource    & ds = *d->datasource;
  char      x;
  bool      esc = false;
  int           cnum = 0, numch = 0;
  string    str;

  do
  {
    if( eof() )
      throw file_error( string( "EOF, line " ) + lineString(), name() );
    x = ds.getch();
    if( x == EOF || x == '\0' )
      throw file_error( string( "EOF, line " ) + lineString(), name() );
    ++n;
    if( numch )
    {
      if( x >= '0' && x <= '9' )
      {
        cnum = cnum << 4 | ( x - '0' );
        ++numch;
      }
      else if( x >= 'A' && x <= 'F' )
      {
        cnum = ( cnum << 4 ) | ( x - 'A' + 10 );
        ++numch;
      }
      else if( x >= 'a' && x <= 'f' )
      {
        cnum = ( cnum << 4 ) | ( x - 'a' + 10 );
        ++numch;
      }
      else
        numch = 3;
      if( numch >= 3 )
      {
        numch = 0;
        esc = false;
        str += (char) cnum;
      }
    }
    else if( esc )
    {
      esc = false;
      switch( x )
      {
        case 'n':
          str += '\n';
          break;
        case 't':
          str += '\t';
          break;
        case 'r':
          str += '\r';
          break;
        case '\\':
          str += '\\';
          break;
        case '\'':
          str += '\'';
          break;
        case '\"':
          str += '\"';
          break;
        case 'x':
          numch = 1;
          cnum = 0;
          break;
        default:
          // unrecognized escape char
          str += '\\';
          str += x;
      }
      x = 0;
    }
    else if( x == '\\' )
      esc = true;
    else if( x != sep )
      str += x;
  }
  while( x != sep || esc );
  if( numch > 0 )
    str += (char) cnum;

  return str;
}

bool PythonReader::readSyntax( string & synt )
{
  //cout << "readSyntax\n";
  DataSource    & ds = *d->datasource;

  if( !ds.isOpen() )
    throw file_error( string( "EOF, line " ) + lineString(), name() );
  unsigned  n = 0;
  bool      ok = true;

  skipWhile( " \t\n\\\r" );
  //return false; // ungetch() doesn't work for several chars

  char c = ds.getch();
  ++n;
  if( c == 'u' )
  {
    c = ds.getch();
    ++n;
  }
  if( c != '\'' && c != '"' )
    ok = false;
  else
    {
      string    att = readString( c, n );
      // cout << "readSyntax: read: " << att << endl;
      if( att == "__syntax__" )
        {
          n += skipWhile( " \t\n\\\r" );
          c = ds.getch();
          ++n;
          if( c == ':' )
            {
              n += skipWhile( " \t\n\\\r" );
              c = ds.getch();
              ++n;
              if( c != '\'' && c != '"' )
                ok = false;
              else
                synt = readString( c, n );
            }
          else
            ok = false;
        }
      else
        {
          ok = false;
        }
    }

  if( ok )
    return true;
  //cout << "unget " << n << " last chars\n";
  //cout << "pos before: " << ds.at() << endl;
  ds.at( ds.at() - (int) n );
  //cout << "pos after: " << ds.at() << endl;
  return false;
}


void PythonReader::readDictionary( GenericObject & obj )
{
  // cout << "readDictionary\n";
  DataSource    & ds = *d->datasource;

  if( !ds.isOpen() )
    throw file_error( string( "EOF, line " ) + lineString(), name() );
  skipWhile( " \t\n\\\r" );
  if( eof() )
    throw file_error( string( "EOF, line " ) + lineString(), name() );
  char  c = ds.getch();
  if( c != '{' )
    throw runtime_error( string( "PythonReader: Not a dictionary: " )
                         + name() + ", line " + lineString() );
  readDictionary2( obj );
}


void 
PythonReader::readDictionary2( GenericObject & obj )
{
  // cout << "readDictionary2 on " << obj.type() << endl;
  DataSource    & ds = *d->datasource;
  bool      end = false, sep;
  string    id, type;
  unsigned  n = 0;
  char      c;
  GenericObject *de;

  if( eof() )
    throw file_error( string( "EOF, line " ) + lineString(), name() );

  do
    {
      skipWhile( " \t\n\\\r" );
      if( eof() )
        throw file_error( string( "EOF, line " ) + lineString(), name() );
      c = ds.getch();
      if( c == ',' )    // separator
        {
          skipWhile( " \t\n\\\r" );
          if( eof() )
            throw file_error( string( "EOF, line " ) + lineString(), name() );
          c = ds.getch();
        }
      // cout << "char: " << c << endl;

      sep = false;
      // expect attribute name
      switch( c )
        {
        case 'u':
          c = ds.getch();
          if( c != '\'' && c != '"' )
          {
            cerr << ds.url() << ": unrecognized tag start: " << c << endl;
            end = true;
            break;
          }
        case '\'':  // string identifier
        case '"':
          id = readString( c, n );
          if( eof() )
            throw file_error( string( "EOF, line " ) + lineString(), name() );
          if( id.empty() )
            {
              cerr << "empty identifier\n";
              break;
            }
          // cout << "ID: " << id << endl;
          break;
        case '}':
          end = true;
          break;
        default:
          cerr << ds.url() << ": unrecognized tag start: " << c << endl;
          end = true;
          break;
        }

      if( end || eof() )
        break;

      // expect ':'
      if( !sep )
        {
          skipWhile( " \t\n\\\r" );
          if( eof() )
            throw file_error( string( "EOF, line " ) + lineString(), name() );
          c = ds.getch();
          if( c != ':' )
            {
              cerr << "expecting ':', got '" << c << "', line " 
                   << lineString() << ", id: " << id << endl;
              break;
            }
        }

      // cout << "Dictionary: read id " << id << endl;
      de = read( &obj, id );
      if( de )
        {
          // cout << "value type: " << de->type() << endl;
          SyntaxedInterface *si = obj.getInterface<SyntaxedInterface>();
          if( si && si->hasSyntax() && id == "__syntax__" )
            si->setSyntax( de->value<string>() );
          else
            obj.setProperty( id, Object( de ) );
        }
    }
  while( !end && !eof() );
}


void 
PythonReader::readIntDictionary2( TypedObject<IntDictionary> & obj )
{
  // cout << "readIntDictionary2\n";
  DataSource    & ds = *d->datasource;
  bool      end = false, sep;
  int   id;
  char      c;
  GenericObject *de;

  if( !ds.isOpen() )
    throw file_error( string( "EOF, line " ) + lineString(), name() );

  do
    {
      skipWhile( " \t\n\\\r" );
      c = ds.getch();
      if( c == ',' )    // separator
        {
          skipWhile( " \t\n\\\r" );
          c = ds.getch();
        }

      sep = false;
      // expect attribute name
      switch( c )
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
        case '.':
        case '+':
        case 'i':
        case 'I':
        case 'n':
        case 'N':
          ds.ungetch( c );
          ds >> id;
          break;
        case '}':
          end = true;
          break;
        default:
          cerr << "unrecognized tag start: " << c << endl;
          end = true;
          break;
        }

      if( end || !ds.isOpen() )
        break;

      // expect ':'
      if( !sep )
        {
          skipWhile( " \t\n\\\r" );
          c = ds.getch();
          if( c != ':' )
            {
              cerr << "expecting ':', got '" << c << "', line " 
                   << lineString() << ", id: " << id << endl;
              break;
            }
        }

      //cout << "Dictionary: read id " << id << endl;
      de = read( &obj, "" );
      if( de )
        {
          obj.getValue()[ id ] = de;
        }
    }
  while( !end && ds.isOpen() );

}


rc_ptr<DataSource> PythonReader::dataSource()
{
  return d->datasource;
}


const PythonReader::HelperSet & PythonReader::helpers() const
{
   return d->helpers;
}


PythonReader::HelperSet & PythonReader::helpers()
{
   return d->helpers;
}


const SyntaxSet & PythonReader::syntaxes() const
{
   return d->rules;
}


SyntaxSet & PythonReader::syntaxes()
{
   return d->rules;
}


INSTANTIATE_GENERIC_OBJECT_TYPE( PythonReader::Helper )
INSTANTIATE_GENERIC_OBJECT_TYPE( PythonReader::HelperSet )
