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
#include <cartobase/datasource/datasource.h>
#include <cartobase/object/pythonreader.h>

using namespace std;
using namespace carto;

struct PythonReader::Private
{
  Private( const SyntaxSet & r, rc_ptr<DataSource> ds )
  : rules( r ), datasource( ds ), eof( false )
  {}

  Private( const SyntaxSet & r )
  : rules( r ), datasource(), eof( false )
  {}

  SyntaxSet           rules;
  rc_ptr<DataSource>  datasource;
  HelperSet           helpers;
  bool                eof;
};


namespace
{

  template<typename T>
  GenericObject*
  genericHelper( GenericObject*, const string &, PythonReader & r )
  {
    r.skipWhile( " \t\n\\\r" );
    DataSource & ds = *r.dataSource();
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    T   x;
    *r.dataSource() >> x;
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    return( new ValueObject<T>( x ) );
  }

  template<>
  GenericObject*
  genericHelper<char>( GenericObject*, const string &, PythonReader & r )
  {
    r.skipWhile( " \t\n\\\r" );
    DataSource & ds = *r.dataSource();
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    int x;
    *r.dataSource() >> x;
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    //cout << "number: " << x << endl;
    return( new ValueObject<char>( char( x ) ) );
  }


  template<>
  GenericObject*
  genericHelper<unsigned char>( GenericObject*, const string &, 
                                PythonReader & r )
  {
    r.skipWhile( " \t\n\\\r" );
    DataSource & ds = *r.dataSource();
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    unsigned    x;
    *r.dataSource() >> x;
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    //cout << "number: " << x << endl;
    return( new ValueObject<unsigned char>( (unsigned char) x ) );
  }


  template<>
  GenericObject* 
  genericHelper<string>( GenericObject*, const string &, PythonReader & r )
  {
    string  x;
    char    quote;
    DataSource  & ds = *r.dataSource();
    if( !ds.isOpen() )
      throw eof_error( string( "EOF: line" ) + r.lineString(), r.name() );

    r.readWhile( " \t\n\\\r" );
    if( ds.eof() )
      io_error::launchErrnoExcept( ds.url() );
    quote = ds.getch();
    if( ds.eof() )
      io_error::launchErrnoExcept( ds.url() );
    if( quote == 'u' ) // unicode string
    {
      quote = ds.getch();
      if( ds.eof() )
        io_error::launchErrnoExcept( ds.url() );
    }
    if( quote != '"' && quote != '\'' )
    {
      cerr << "string reader: NOT a string: " << quote << endl;
      return 0; // not a string
    }
    unsigned    n = 0;
    x = r.readString( quote, n );
    if( ds.eof() )
      io_error::launchErrnoExcept( ds.url() );

    //cout << "string: " << x << endl;
    return( new ValueObject<string>( x ) );
  }

  GenericObject* listHelper( GenericObject*, const string &, PythonReader & r )
  {
    // cout << "list\n";
    bool        end = false;
    string  id, type;
    DataSource  & ds = *r.dataSource();

    if( !ds.isOpen() )
      return 0;
    r.skipWhile( " \t\n\\\r" );
    if( r.eof() )
      io_error::launchErrnoExcept( ds.url() );
    char    c = ds.getch(), mark;
    if( ds.eof() )
      io_error::launchErrnoExcept( ds.url() );
    if( c == '(' )
      mark = ')';
    else if( c == '[' )
      mark = ']';
    else
      {
        // cout << "char: " << c << endl;
        throw runtime_error( string( "PythonReader: Not a list/tuple: " ) 
                             + r.name() + ", line " + r.lineString() );
      }

    ValueObject<ObjectVector> *obj = new ValueObject<ObjectVector>;

    try
      {
        do
          {
            r.skipWhile( " \t\n\\\r" );
            if( r.eof() )
              io_error::launchErrnoExcept( ds.url() );
            c = ds.getch();
            if( ds.eof() )
              io_error::launchErrnoExcept( ds.url() );
            if( c == ',' )  // separator
              {
                r.skipWhile( " \t\n\\\r" );
                if( r.eof() )
                  io_error::launchErrnoExcept( ds.url() );
                c = ds.getch();
                if( ds.eof() )
                  io_error::launchErrnoExcept( ds.url() );
              }
            if( c == mark )
              {
                end = true;
                break;  // end
              }
            ds.ungetch( c );
            obj->getValue().push_back( r.read( obj, "" ) );
          }
        while( ds.isOpen() );

        return( obj );
      }
    catch( exception & e )
      {
        delete obj;
        throw;
      }
  }


  GenericObject* noneHelper( GenericObject*, const string &, PythonReader & )
  {
    return 0;
  }


} // namespace (internal linkage)

template<typename T>
GenericObject* PythonReader::genericSequenceHelper( GenericObject*, const string &,
                                      PythonReader & r )
{
  bool        end = false;
  string  id, type;
  DataSource  & ds = *r.dataSource();

  if( !ds.isOpen() )
    return( 0 );
  r.skipWhile( " \t\n\\\r" );
  if( r.eof() )
    io_error::launchErrnoExcept( ds.url() );
  char    c = ds.getch(), mark;
  if( r.eof() )
    io_error::launchErrnoExcept( ds.url() );
  if( c == '(' )
    mark = ')';
  else if( c == '[' )
    mark = ']';
  else
    {
      //cout << "char: " << c << endl;
      throw runtime_error( string( "PythonReader: Not a list/tuple: " )
                            + r.name() + ", line " + r.lineString() );
    }

  ValueObject<T> *obj = new ValueObject<T>;
  GenericObject       *de;
  ValueObject<typename T::value_type> *dev;

  try
    {
      do
        {
          r.skipWhile( " \t\n\\\r" );
          if( r.eof() )
            io_error::launchErrnoExcept( ds.url() );
          c = ds.getch();
          if( ds.eof() )
            io_error::launchErrnoExcept( ds.url() );
          if( c == ',' )  // separator
            {
              r.skipWhile( " \t\n\\\r" );
              if( r.eof() )
                io_error::launchErrnoExcept( ds.url() );
              c = ds.getch();
              if( ds.eof() )
                io_error::launchErrnoExcept( ds.url() );
            }
          if( c == mark )
            {
              end = true;
              break;  // end
            }
          ds.ungetch( c );
          de = genericHelper<typename T::value_type>( obj, "", r );
          dev
            = dynamic_cast<ValueObject<typename T::value_type>*>( de );
          if( dev )
            obj->getValue().push_back( dev->getValue() );
          else
            {
              delete de;
              throw runtime_error( string( "PythonReader: wrong type "
                                            "inserted in homogen sequence: "
                                            )
                                    + r.name() + ", line " + r.lineString() );
            }
          delete de;
        }
      while( ds.isOpen() );

      return( obj );
    }
  catch( exception & e )
    {
      delete obj;
      throw;
    }
}

template<typename T>
GenericObject* PythonReader::genericDictHelper( GenericObject *, const string &,
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

  ValueObject<T>   *obj = new ValueObject<T>;

  try
  {
    r.readDictionary2( *obj );
  }
  catch( exception & e )
  {
    delete obj;
    throw;
  }

  return obj;
}
