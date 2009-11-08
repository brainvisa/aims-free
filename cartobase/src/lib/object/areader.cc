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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/object/areader.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/parse.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/object/syntobject.h>
#include <cartobase/object/pythonreader.h>
//#include <cartobase/io/minfXML2.h> // not ready yet
#include <cartobase/wip/io/asciidatasourcetraits.h>
#include <cartobase/datasource/streamdatasource.h>
#include <cartobase/stream/attachable_cuifstream.h>
#include <vector>
#include <assert.h>

using namespace std;
using namespace carto;


//=============================================================================
//	F U N C T I O N S
//=============================================================================

namespace
{

  void
  intHelper(GenericObject& object, const string& semantic, DataSource & is)
  {
    if( is.eof() )
      return;

    if( !StreamUtil::skip( is ) )
      return;

    int i = 0;
    if( !AsciiDataSourceTraits<int>::read( is, i ) )
      return;
    object.setProperty( semantic, i );
  }


  void
  floatHelper(GenericObject& object, const string& semantic, DataSource & is)
  {
    if( is.eof() )
      return;

    if( !StreamUtil::skip( is ) )
      return;

    float f = 0;
    if( !AsciiDataSourceTraits<float>::read( is, f ) )
      return;
    object.setProperty(semantic, f);
  }


  void
  stringHelper(GenericObject& object, const string& semantic, DataSource & is)
  {
    if( is.eof() )
      return;

    if( !StreamUtil::skip( is ) )
      return;

    string s;
    StreamUtil::getline( is, s );
    string::size_type i1 = s.find_first_not_of(" \t");
    string::size_type i2 = s.find_last_not_of(" \t\n\r");
    s = s.substr(i1, i2-i1+1);
    object.setProperty(semantic, s);
  }


  void
  int_vectorHelper( GenericObject& object, const string& semantic, 
                    DataSource & is )
  {
    vector<int> iv;
    while (1)
      {
        if( is.eof() )
          return;

        if( !StreamUtil::skip( is ) )
          return;

        int i;
        if( !AsciiDataSourceTraits<int>::read( is, i ) )
          break;
        iv.push_back(i);
      }
    object.setProperty(semantic, iv);
  }


  void
  float_vectorHelper( GenericObject& object, const string& semantic, 
                      DataSource & is )
  {
    vector<float> fv;
    while (1)
      {
        if( !is )
          return;

        if( !StreamUtil::skip( is, " \t" ) )
          return;

        float f;
        if( !AsciiDataSourceTraits<float>::read( is, f ) )
          break;
        fv.push_back(f);
      }
    object.setProperty(semantic, fv);
  }


  void
  genericobjectHelper( GenericObject& object, const string& semantic, 
                       DataSource & is )
  {
    PythonReader	pr;
    rc_ptr<DataSource>	ds( &is );
    pr.attach( ds );
    try
      { 
        GenericObject	*o = pr.read( &object, semantic );
        ds.release();
        if( o )
          object.setProperty( semantic, Object( o ) );
      }
    catch( exception & e )
      {
        ds.release();
        throw;
      }
  }

}

//=============================================================================
//	M E T H O D S
//=============================================================================

AttributedReader::AttributedReader( const string& filename,
                                    const SyntaxSet& rules,
                                    const HelperSet& helpers )
  : _rules(rules),
    _datasource( new IStreamDataSource
                 ( rc_ptr<istream>( new cuifstream( filename.c_str() ) ) ) ),
    _helpers(helpers)
{
  // are files opened?
  if( !_datasource->isOpen() )
    _datasource->open( DataSource::Read );
  if (!*_datasource)
    io_error::launchErrnoExcept( filename );

  init();
}


AttributedReader::AttributedReader( rc_ptr<DataSource> ds,
                                    const SyntaxSet& rules,
                                    const HelperSet& helpers )
  : _rules(rules),
    _datasource( ds ),
    _helpers(helpers)
{
  // are files opened?
  if( !_datasource->isOpen() )
    _datasource->open( DataSource::Read );
  if (!*_datasource)
    io_error::launchErrnoExcept( ds->url() );

  init();
}


AttributedReader::AttributedReader( const SyntaxSet& rules,
                                    const HelperSet& helpers )
  : _rules(rules),
    _helpers(helpers)
{
  init();
}

void AttributedReader::init()
{
  if (_helpers.find("int") == _helpers.end())
    _helpers["int"] = &intHelper;
  if (_helpers.find("float") == _helpers.end())
    _helpers["float"] = &floatHelper;
  if (_helpers.find("string") == _helpers.end())
    _helpers["string"] = &stringHelper;
  if (_helpers.find("int_vector") == _helpers.end())
    _helpers["int_vector"] = &int_vectorHelper;
  if (_helpers.find("float_vector") == _helpers.end())
    _helpers["float_vector"] = &float_vectorHelper;
  if (_helpers.find("dictionary") == _helpers.end())
    _helpers["dictionary"] = &genericobjectHelper;
  if (_helpers.find("list") == _helpers.end())
    _helpers["list"] = &genericobjectHelper;
}


AttributedReader::~AttributedReader()
{
}


void
AttributedReader::open( const string& filename )
{
  close();
  _datasource.reset( new IStreamDataSource
                     ( rc_ptr<istream>( new cuifstream
                                        ( filename.c_str() ) ) ) );
  _datasource->open( DataSource::Read );

  // are files opened?
  if( !*_datasource )
    io_error::launchErrnoExcept( filename );
}


void AttributedReader::attach( rc_ptr<DataSource> ds )
{
  _datasource = ds;
}


void AttributedReader::attach( istream & s, int line_num )
{
  close();
  attachable_cuifstream	*ac = new attachable_cuifstream;
  ac->attach( s, line_num );
  _datasource.reset( new IStreamDataSource( rc_ptr<istream>( ac ) ) );
}


void
AttributedReader::close()
{
  if( _datasource )
    _datasource->close();
  _datasource.reset( 0 );
}


string
AttributedReader::name() const
{
  if( !_datasource )
    return string();
  return _datasource->url();
}


int
AttributedReader::line() const
{
  if( !_datasource )
    return -1;
  IStreamDataSource 
    *sds = dynamic_cast<IStreamDataSource *>( _datasource.get() );
  if( !sds )
    return -1;
  cuifstream	&cs = dynamic_cast<cuifstream &>( sds->stream() );
  if( !&cs )
    return -1;
  return cs.line();
}


bool
AttributedReader::operator!() const
{
  return !_datasource || !*_datasource;
}


bool
AttributedReader::is_open() const
{
  return _datasource && _datasource->isOpen();
}


bool
AttributedReader::eof() const
{
  return !_datasource || _datasource->eof();
}


void
AttributedReader::readAttribute(GenericObject& object,
                                const string& semantic)
{
  if( !_datasource || !*_datasource )
    return;
  //throw file_error( string( "EOF found" ) + _filename );

  string	token;
  string	synt = "";

  SyntaxedInterface *si = object.getInterface<SyntaxedInterface>();
  if( si && si->hasSyntax() )
    synt = si->getSyntax();


  // if this semantic attribute does have associated I/O properties...
  SyntaxSet::const_iterator syntax = _rules.find(synt);
  if( syntax == _rules.end() )
    {
      stringstream buffer;
      buffer << "syntax not found: " << synt 
             << ", line: " << line();
      throw syntax_check_error( buffer.str(), _datasource->url() );
    }
  map<string,Semantic>::const_iterator property =
    syntax->second.find(semantic);
  map<string,Semantic>::const_iterator property2;

  if (property == syntax->second.end())
    property2 = syntax->second.find("__fallback__");
  else
    property2 = property;
  if (property2 != syntax->second.end())
    {
      // make sure this attribute is read for the first time
      if (object.hasProperty(semantic))
        {
          bool pset = false;
          DictionaryInterface	*di 
            = object.getInterface<DictionaryInterface>();
          if( di && dynamic_cast<PropertySet *>( di ) )
            /* don't test for duplicate atts in PropertySet because 
               builtin props are always already present. 
               FIXME: needs a better check, only on builtins
            */
            pset = true;
          if( !pset )
            {
              stringstream buffer;
              buffer << "Several attributes of same name: " << semantic 
                     << ", line: " << line();
              throw syntax_check_error( buffer.str(), _datasource->url() );
            }
        }
      // find the associated I/O function
      HelperSet::const_iterator h =
        _helpers.find(property2->second.type);
      if( h == _helpers.end() )
        {
          stringstream buffer;
          buffer << "Can't read type " << property2->second.type 
                 << " for attribute " << semantic << ", line: " 
                 << line();
          throw syntax_check_error( buffer.str(), _datasource->url() );
        }
      // call the associated I/O function
      (h->second)( object, semantic, *_datasource );
    }
  else
    {
      string	l;
      StreamUtil::getline( *_datasource, l );
    }

}


void
AttributedReader::readAttribute(GenericObject* object,
                                const string& semantic)
{
	assert(object != 0);
	readAttribute(*object, semantic);
}


