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
 *  Finder class
 */
#include <aims/io/finder.h>
#include <aims/io/finderFormats.h>
#include <aims/data/header.h>
#include <aims/io/spmattribconverter.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/type/typetraits.h>
#include <cartobase/wip/io/datasourceinfo.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartodata/io/carto2aimsheadertranslator.h>
#include <map>
#include <set>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


namespace aims
{
  struct Finder_Private
  {
    ~Finder_Private();

    map<string, FinderFormat *>	formats;
    map<string, list<string> >	extensions;
  };

  namespace internal
  {
    //defined in reader_d.cc
    void keep_exception_priority( std::exception & e, int & prio, int & type,
				  std::string & message );
  }
}


Finder_Private::~Finder_Private()
{
  map<string, FinderFormat *>::iterator	im, em = formats.end();
  for( im=formats.begin(); im!=em; ++im )
    delete im->second;
}


auto_ptr<Finder_Private>	Finder::pd( 0 );


void Finder::initPrivate()
{
  if( !pd.get() )
    {
      pd.reset( new Finder_Private );
      vector<string>	ext;
      ext.push_back( "ima" );
      ext.push_back( "dim" );
      registerFormat( "GIS", new FinderGisFormat, ext );
      ext.clear();
      ext.push_back( "img" );
      ext.push_back( "hdr" );
      registerFormat( "SPM", new FinderSpmFormat, ext );
#ifdef VIDA_LIB
      ext.clear();
      ext.push_back( "vimg" );
      ext.push_back( "vinfo" );
      ext.push_back( "vhdr" );
      registerFormat( "VIDA", new FinderVidaFormat, ext );
#endif
#ifdef ECAT_LIB
      ext.clear();
      ext.push_back( "v" );
      ext.push_back( "i" );
      ext.push_back( "p" );
      registerFormat( "ECAT", new FinderEcatFormat, ext );
#endif
#ifdef AIMS_JPEG
      ext.clear();
      ext.push_back( "jpg" );
      registerFormat( "JPEG", new FinderJpegFormat, ext );
#endif
      ext.clear();
      ext.push_back( "mesh" );
      registerFormat( "MESH", new FinderMeshFormat, ext );
      ext.clear();
      ext.push_back( "tri" );
      registerFormat( "TRI", new FinderTriFormat, ext );
      ext.clear();
      ext.push_back( "obj" );
      registerFormat( "MNI OBJ", new FinderMniObjFormat, ext );
      ext.clear();
      ext.push_back( "tex" );
      registerFormat( "TEX", new FinderTexFormat, ext );
      ext.clear();
      ext.push_back( "bck" );
      registerFormat( "BCK", new FinderBckFormat, ext );
      ext.clear();
      ext.push_back( "arg" );
      ext.push_back( "bundles" );
      registerFormat( "ARG", new FinderGraphFormat, ext );
      ext.clear();
      ext.push_back( "trm" );
      registerFormat( "TRM", new FinderTrmFormat, ext );
      ext.clear();
      ext.push_back( "hie" );
      registerFormat( "HIE", new FinderHieFormat, ext );
      // at end: formats without an extension
      ext.clear();
      ext.push_back( "" );
      registerFormat( "GENESIS", new FinderGenesisFormat, ext );
#ifdef DICOM_LIB
      ext.clear();
      ext.push_back( "" );
      registerFormat( "DICOM", new FinderDicomFormat, ext );
#endif
    }
}


Finder::Finder() : _state( Unchecked ), _errorcode( -1 )
{
  initPrivate();
}


Finder::~Finder()
{
}


void Finder::registerFormat( const string & fmtid, FinderFormat* format,
			     const vector<std::string> & extensions,
                             const string & before )
{
  initPrivate();

  FinderFormat	*oldr = finderFormat( fmtid );
  delete oldr;
  pd->formats[ fmtid ] = format;

  vector<string>::const_iterator	ie, ee = extensions.end();
  list<string>::iterator  ie2, ee2;
  for( ie=extensions.begin(); ie!=ee; ++ie )
  {
    list<string>  & ext = pd->extensions[ *ie ];
    if( before.empty() )
      ext.push_back( fmtid );
    else
    {
      for( ie2=ext.begin(), ee2=ext.end(); ie2!=ee2; ++ie2 )
        if( *ie2 == before )
          break;
      ext.insert( ie2, fmtid );
    }
  }
}


FinderFormat* Finder::finderFormat( const string & format )
{
  initPrivate();

  map<string, FinderFormat *>::const_iterator	i
    = pd->formats.find( format );
  if( i == pd->formats.end() )
    return( 0 );
  return( (*i).second );
}


bool Finder::check( const string& filename )
{
#ifdef AIMS_DEBUG_IO
  cout << "Finder::check( " << filename << " )\n";
#endif
  static bool plugs = false;
  if( !plugs )
    {
      plugs = true;
      PluginLoader::load();
    }

  initPrivate();
  _state = Unchecked;

  // try using DataSourceInfo first (new system 2005)
  DataSourceInfo	dsi;
  FileDataSource	ds( filename );
  Object		h = dsi.check( ds );
  bool dsok = !h.isNone();
  if( dsok )
    {
      _state = Ok;

      Carto2AimsHeaderTranslator	t;
      t.translate( h );

      string	x;
      h->getProperty( "object_type", x );
      setObjectType( x );
      x.clear();
      h->getProperty( "data_type", x );
      setDataType( x );
      vector<string>	vt;
      vt.push_back( x );
      h->getProperty( "possible_data_types", vt );
      setPossibleDataTypes( vt );
      x.clear();
      h->getProperty( "file_type", x );
      setFormat( x );

      PythonHeader	*ph = new PythonHeader;
      ph->copyProperties( h );
      setHeader( ph );

      // cout << "DataSourceInfo worked\n";
      if( filename.substr( filename.length() - 4, 4 ) != ".gii" )
        // bidouille to let the gifti reader work (it's both gifti and XML)
        return true;
    }

  _errorcode = -1;
  _errormsg = "";

  //	find filename extension
  string                bname = FileUtil::basename( filename );
  string::size_type	pos = bname.find( '.' );
  string::size_type     dlen = filename.length() - bname.length();
  string		ext;
  int			excp = 0;

  if( pos != string::npos )
    ext = filename.substr( dlen+pos+1, filename.length() - pos - 1 );

#ifdef AIMS_DEBUG_IO
  cout << "ext : " << ext << endl;
#endif
  //	Check compatible formats
  set<string>			tried;
  FinderFormat			*reader;
  set<string>::iterator	notyet = tried.end();

  map<string, list<string> >::const_iterator iext
      = pd->extensions.find( ext ),
  eext = pd->extensions.end();

  while( iext == eext && (pos=bname.find( '.', pos+1 ))!=string::npos )
    {
      ext = filename.substr( dlen+pos+1, filename.length() - pos - 1 );
      iext = pd->extensions.find( ext );
    }
  list<string>::const_iterator	ie, ee;
  if( iext != eext )
  {
    // try every matching format until one works
    for( ie=iext->second.begin(), ee=iext->second.end(); ie!=ee; ++ie )
      if( tried.find( *ie ) == notyet )
        {
#ifdef AIMS_DEBUG_IO
          cout << "trying " << *ie << "...\n";
#endif
          reader = finderFormat( *ie );
          if( reader )
            {
              try
                {
                  if( reader->check( filename, *this ) )
                    {
                      _state = Ok;
                      postProcessHeader( _header );
                      return( true );
                    }
                }
              catch( exception & e )
                {
                  io_error::keepExceptionPriority( e, excp, _errorcode,
                                                  _errormsg );
                }
              tried.insert( *ie );
            }
        }
  }

#ifdef AIMS_DEBUG_IO
  cout << "not found yet... pass2...\n";
#endif
  if( !ext.empty() )
    {
      // not found or none works: try readers with no extension
      iext = pd->extensions.find( "" );

      if( iext != eext )
        for( ie=iext->second.begin(), ee=iext->second.end(); ie!=ee; ++ie )
          if( tried.find( *ie ) == notyet )
            {
#ifdef AIMS_DEBUG_IO
              cout << "pass2, trying " << *ie << "...\n";
#endif
              reader = finderFormat( *ie );
              if( reader )
                {
                  try
                    {
                      if( reader->check( filename, *this ) )
                        {
                          _state = Ok;
                          postProcessHeader( _header );
                          return( true );
                        }
                    }
                  catch( exception & e )
                    {
                      io_error::keepExceptionPriority( e, excp, _errorcode,
                                                      _errormsg );
                    }
                  tried.insert( *ie );
                }
            }
    }

#ifdef AIMS_DEBUG_IO
  cout << "not found yet... pass3...\n";
#endif
  // still not found ? well, try EVERY format this time...
  for( iext=pd->extensions.begin(); iext!=eext; ++iext )
    for( ie=iext->second.begin(), ee=iext->second.end(); ie!=ee; ++ie )
      if( tried.find( *ie ) == notyet )
      {
        reader = finderFormat( *ie );
        if( reader )
          {
#ifdef AIMS_DEBUG_IO
            cout << "pass3, trying " << *ie << "...\n";
#endif
            try
              {
                if( reader->check( filename, *this ) )
                  {
                    _state = Ok;
                    postProcessHeader( _header );
                    return( true );
                  }
              }
            catch( exception & e )
              {
                io_error::keepExceptionPriority( e, excp, _errorcode,
                                                _errormsg );
              }
            tried.insert( *ie );
          }
      }

#ifdef AIMS_DEBUG_IO
  cout << "not found at all, giving up\n";
#endif
  // still not succeeded, it's hopeless...

  if( dsok )
    return true; // well, the datasource was OK (exception for .gii )

  _state = Error;
  if( _errorcode < 0 )
    {
      _errorcode = 0;
      _errormsg = filename + " : could not identify format";
    }
  //launchException();
  return( false );
}


const Header* Finder::header() const
{
  return static_cast<const PythonHeader *>( _header.get() );
}


void Finder::setHeader( Header* hdr )
{
  if( !dynamic_cast<PythonHeader *>( hdr) )
    throw runtime_error( "Finder::setHeader: non-generic Header is not "
                         "supported anymore.");
  _header.reset( static_cast<PythonHeader *>( hdr ) );
}


void Finder::setHeader( Object hdr )
{
  if( !dynamic_cast<PythonHeader *>( hdr.get() ) )
    throw runtime_error( "Finder::setHeader: Object is not a Header" );
  _header = hdr;
}


void Finder::launchException()
{
  /* cout << "Finder::launchException\n";
  cout << "code: " << _errorcode << ", state: " << (int) _state
  << ", msg: " << _errormsg << endl; */
  if( _state != Error || _errorcode < 0 )
    return;	// no error (?)
  string	defmsg;
  if( _errormsg.empty() )
    defmsg = "could not identify format";
  io_error::launchExcept( _errorcode, _errormsg, defmsg );
}


void Finder::postProcessHeader( carto::Object hdr )
{
  initPrivate();
  spmAttributesConverter( *hdr );
}


set<string> Finder::extensions( const string & format )
{
  initPrivate();
  set<string> exts;
  map<string, list<string> >::const_iterator ie, ee = pd->extensions.end();
  list<string>::const_iterator ifo, efo;
  for( ie=pd->extensions.begin(); ie!=ee; ++ie )
    for( ifo=ie->second.begin(), efo=ie->second.end(); ifo!=efo; ++ifo )
      if( *ifo == format )
      {
        exts.insert( ie->first );
        break;
      }
  return exts;
}

