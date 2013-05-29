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

//--- aims ---------------------------------------------------------------------
#ifdef USE_SOMA_IO
  #include <aims/io/gisformatchecker.h>                     // class declaration
#else
  #include <soma-io/plugingis/gisformatchecker.h>
#endif
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasource/datasourcelist.h>
#include <soma-io/datasource/filedatasource.h>    // because we use file sources
#include <soma-io/reader/itemreader.h>                 // to read in the file
#include <soma-io/utilities/asciidatasourcetraits.h>       // to read datasource
#include <soma-io/writer/pythonwriter.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>                                   // header
#include <cartobase/object/property.h>                                 // header
#include <cartobase/stream/fileutil.h>               // to manipulate file names
#include <cartobase/config/verbose.h>                 // to write debug messages
//--- system -------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#define SOMAIO_BYTE_ORDER 0x41424344  //"ABCD" in ascii
//-- debug ---------------------------------------------------------------------
//#define SOMA_IO_DEBUG
//------------------------------------------------------------------------------

using namespace soma;
using namespace carto;
using namespace std;

//==============================================================================
//   U S E F U L
//==============================================================================
namespace
{
  /*  REGISTERED THROUGH PLUGIN
  //--- Registering the checker ------------------------------------------------
  bool initMe()
  {
    vector<string>  exts;
    exts.push_back( "ima" );
    exts.push_back( "dim" );
    DataSourceInfoLoader::registerFormat( "GIS", new GisFormatChecker, exts );

    return true;
  }
  
  bool  _dummy = initMe();
  */
  
  //--- helper for reading attributes ------------------------------------------
  template<typename T> 
  static void inHelper( Object & object, const string & semantic, 
                        DataSource & ds )
  {
    if( !ds.isOpen() )
      return;

    T i = 0;
    ds >> i;
    object->setProperty( semantic, i );
  }
  
  template<typename T> 
  static void inVecHelper( Object & object, const string & semantic, 
                           DataSource & ds )
  {
    vector<T> iv;
    while (1)
      {
        if( !ds.isOpen() )
          return;

        int c = ds.getch();
        while (c != '\n' && c != '\0' && c != EOF 
               && (c == ' ' || c == '\t'))
          c = ds.getch();
        if (c == '\n' || c == '\0' || c == EOF)
          break;
        else
          ds.ungetch( c );
        T i;
        ds >> i;
        iv.push_back(i);
      }
    object->setProperty( semantic, iv );
  }

}

//==============================================================================
//   P R I V A T E   M E T H O D S
//==============================================================================
//--- BUILDING DATASOURCELIST --------------------------------------------------
/* This method builds a DataSourceList from the initial datasource.
 * It tries to find .dim, .ima and .minf files
 * If one or several of those files doesn't exist, it writes the initial
 * DataSource instead. Thus, the list returned contains at least one ds for
 * each of the following keywords : "dim", "ima", "minf", "default", but
 * in the worst case they can all be the initial ds.
 */
void GisFormatChecker::_buildDSList( DataSourceList & dsl ) const
{
  DataSource* pds = dsl.dataSource( "default", 0 ).get();
  string dimname, imaname, minfname;
  
  dimname = imaname = minfname = pds->url();
  if( dimname.empty() ) {
    // we suppose ds is a dim file and a imafile
    dsl.addDataSource( "dim", rc_ptr<DataSource>( pds ) );
    dsl.addDataSource( "ima", rc_ptr<DataSource>( pds ) );
  } else {
    string ext = FileUtil::extension( dimname );
    string basename = FileUtil::removeExtension( dimname );
    
    if( ext == "ima" ) {
      dimname = basename + ".dim";
    } else if( ext == "dim" ) {
      imaname = basename + ".ima";
    } else {
      ext.clear();
    }
    
    if( ext.empty() ) {
      basename = dimname;
      dimname += ".dim";
      imaname += ".ima";
    }
    minfname = imaname + ".minf";
    
    if( dimname == pds->url() ) {
      // if dimname is original url
      dsl.addDataSource( "dim", rc_ptr<DataSource>( pds ) );
    } else {
      if( FileUtil::fileStat( dimname ).find( '+' ) != string::npos ) {
        // if dim file exists
        dsl.addDataSource( "dim", rc_ptr<DataSource>
            ( new FileDataSource( dimname ) ) );
      } else {
        // we suppose original url is dim
        dsl.addDataSource( "dim", rc_ptr<DataSource>( pds ) );
      }
    }
    if( imaname == pds->url() ) {
      // if imaname is original url
      dsl.addDataSource( "ima", rc_ptr<DataSource>( pds ) );
    } else {
      if( FileUtil::fileStat( imaname ).find( '+' ) != string::npos ) {
        // if ima file exists
        dsl.addDataSource( "ima", rc_ptr<DataSource>
            ( new FileDataSource( imaname ) ) );
      } else {
        // we suppose original url is ima
        dsl.addDataSource( "ima", rc_ptr<DataSource>( pds ) );
      }
    }
  }
  #ifdef SOMA_IO_DEBUG
    cout << "GISFORMATCHECKER:: dim: " << dsl.dataSource( "dim", 0 )->url() << endl;
  #endif
  #ifdef SOMA_IO_DEBUG
    cout << "GISFORMATCHECKER:: ima: " << dsl.dataSource( "ima", 0 )->url() << endl;
  #endif
  
  //// Minf DataSource
  if( FileUtil::fileStat( minfname ).find( '+' ) != string::npos ) {
    // if minf file exist
    dsl.addDataSource( "minf", rc_ptr<DataSource>
        ( new FileDataSource( minfname ) ) );
  } else {
    // we suppose original url is minf
    dsl.addDataSource( "minf", rc_ptr<DataSource>( pds ) );
  }
  
  #ifdef SOMA_IO_DEBUG
    cout << "GISFORMATCHECKER:: minf: " << dsl.dataSource( "minf", 0 )->url() << endl;
  #endif
}

//--- BUILDING HEADER ----------------------------------------------------------
/* This method builds a header from a .dim DataSource.
 * The argument is given by check(...) and is supposed to be a .dim file.
 * However, since the DSList is constructed even if no .dim file is found,
 * it can be an absolutely different format. In this case, the method should 
 * launch an exception at one point which should be caught by 
 * DataSourceInfoLoader (?), thus knowing this checker cannot read the entry file.
 */
Object GisFormatChecker::_buildHeader( DataSource* hds ) const
{
  FileDataSource* fds = dynamic_cast<FileDataSource *>( hds );
  string          fname = hds->url();
  
  if( !hds->isOpen() )
    hds->open( DataSource::Read );
  if( !hds->isOpen() )
    io_error::launchErrnoExcept( fname );
  
  DefaultAsciiItemReader<int>     iir;
  DefaultAsciiItemReader<string>  sir;
  DefaultAsciiItemReader<float>   fir;

  string         token, type, byteord, binar;
  int            sizex = 1, sizey = 1, sizez = 1, sizet = 1;
  Object         hdr = Object::value( PropertySet() );  // header
  int            c;
  vector<float>  vs(4, 1.);

  // reading sizex, sizey, sizez, sizet ////////////////////////////////////////
  if( !StreamUtil::skip( *hds, " \t\n\r" ) )
    throw wrong_format_error( fname );
  if( iir.read( *hds, &sizex ) != 1 )
    throw wrong_format_error( fname );
  if( !StreamUtil::skip( *hds, " \t" ) )
    throw wrong_format_error( fname );
  c = hds->getch();
  hds->ungetch( c );
  if( c != '\n' && c != '\r' ) {
    if( iir.read( *hds, &sizey ) != 1 )
      throw wrong_format_error( fname );
    if( !StreamUtil::skip( *hds, " \t" ) )
      throw wrong_format_error( fname );
    c = hds->getch();
    hds->ungetch( c );
    if( c != '\n' && c != '\r' ) {
      if( iir.read( *hds, &sizez ) != 1 )
        throw wrong_format_error( fname );
      if( !StreamUtil::skip( *hds, " \t" ) )
        throw wrong_format_error( fname );
      c = hds->getch();
      hds->ungetch( c );
      if( c != '\n' && c != '\r' ) {
        if( iir.read( *hds, &sizet ) != 1 )
          throw wrong_format_error( fname );
      }
    }
  }

  /* cout << "size: " << sizex << ", " << sizey << ", " << sizez << ", " 
     << sizet << endl; */

  const Syntax  &sx = DataSourceInfoLoader::minfSyntax()[ "__generic__" ];

  // reading tokens ////////////////////////////////////////////////////////////
  while( hds->isOpen() ) {
    if( !StreamUtil::skip( *hds, " \t\n\r" ) )
      break;
    c = hds->getch();
    if( c != '-' ) {
      hds->ungetch( c );
      break;
    }
    if( sir.read( *hds, &token ) != 1 ) {
      if( token.empty() )
        break;
      else
        throw wrong_format_error( fname );
    }
    if( !StreamUtil::skip( *hds, " \t" ) )
      throw wrong_format_error( fname );
    
    if( token == "type" )
      sir.read( *hds, &type );
    else if (token == "dx" )
      fir.read( *hds, &vs[0] );
    else if (token == "dy" )
      fir.read( *hds, &vs[1] );
    else if (token == "dz" )
      fir.read( *hds, &vs[2] );
    else if (token == "dt" )
      fir.read( *hds, &vs[3] );
    else if (token == "bo" )
      sir.read( *hds, &byteord );
    else if (token == "om" )
      sir.read( *hds, &binar );
    else if ( !token.empty() ) {
      Syntax::const_iterator  ix = sx.find( token );
      if( ix == sx.end() ) {
        string  attval;
        StreamUtil::getline( *hds, attval );
        hdr->setProperty( token, attval );
      } else {
        typedef void (*Helper)( Object &, const string &, DataSource & );
        typedef map<string, Helper> HelperSet;
        
        static HelperSet      hs;
        if( hs.empty() ) {
          hs[ "int" ] = &inHelper<int>;
          hs[ "float" ] = &inHelper<float>;
          hs[ "bool" ] = &inHelper<bool>;
          hs[ "string" ] = &inHelper<string>;
          hs[ "int_vector" ] = &inVecHelper<int>;
          hs[ "float_vector" ] = &inVecHelper<float>;
          //hs[ "string_vector" ] = &inVecHelper<string>;
        }
        
        HelperSet::const_iterator ih = hs.find( ix->second.type );
        if( ih == hs.end() ) {
          string  attval;
          StreamUtil::getline( *hds, attval );
          hdr->setProperty( token, attval );
        } 
        else
          ih->second( hdr, token, *hds );
      }
    }
    else
      break;
  }
  //if( hds->isOpen() )
  //  hds->close();
  
  if( type.empty() ) {
    #ifdef SOMA_IO_DEBUG
      cout << "GISFORMATCHECKER:: Not a GIS header: " << fname << endl;
    #endif
    throw format_mismatch_error( "Not a GIS header", fname );
  }

  if( fds && fds->at() != fds->size() - 1 ) {
    #ifdef SOMA_IO_DEBUG
      cout << "GISFORMATCHECKER:: GIS header not entirely read - garbage at end? " 
           << fname << endl;
    #endif
    throw format_mismatch_error( "GIS header not entirely read - garbage at " 
                                 "end ?", fname );
  }

  /*
  vector<int> sz(4);
  sz[0] = sizex;
  sz[1] = sizey;
  sz[2] = sizez;
  sz[3] = sizet;
  hdr->setProperty( "volume_dimension", sz );
  */
  hdr->setProperty( "sizeX", sizex );
  hdr->setProperty( "sizeY", sizey );
  hdr->setProperty( "sizeZ", sizez );
  hdr->setProperty( "sizeT", sizet );
  hdr->setProperty( "format", string( "GIS" ) );
  hdr->setProperty( "voxel_size", vs );
  if( !type.empty() )
    hdr->setProperty( "object_type", string( "Volume of " ) + type );
  bool  ascii = ( binar == "ascii" );
  hdr->setProperty( "ascii", (int) ascii );
  if( !ascii )
    if( byteord.length() >= 4 ) {
      uint32_t  bo = *( (uint32_t*) byteord.c_str() );
      hdr->setProperty( "byte_swapping", (int) ( bo != SOMAIO_BYTE_ORDER ) );
    }
  
  return hdr;
}

//==============================================================================
//   P U B L I C   M E T H O D S
//==============================================================================/

GisFormatChecker::~GisFormatChecker()
{
}

DataSourceInfo GisFormatChecker::check( DataSourceInfo dsi, 
                                        DataSourceInfoLoader & /* f */,
                                        Object /* options */) const
{
  bool doread = dsi.header().isNone() ;
  bool dolist = dsi.list().nbTypes() == 1 ;
  bool docapa = !dsi.capabilities().isInit();
  //--- test header format -----------------------------------------------------
  if( !doread )
    if( !dsi.header()->hasProperty( "format" ) 
        || dsi.header()->getProperty( "format" )->getString() != "GIS" )
      throw wrong_format_error( "Not a GIS header", 
                                dsi.list().dataSource( "default", 0 )->url() );
  
  //--- build datasourcelist ---------------------------------------------------
  if( dolist ) {
    #ifdef SOMA_IO_DEBUG
      cout << "GISFORMATCHECKER:: Building list..." << endl;
    #endif
    _buildDSList( dsi.list() );
  }
  //--- build header -----------------------------------------------------------
  if( doread ) {
    #ifdef SOMA_IO_DEBUG
      cout << "GISFORMATCHECKER:: Reading header..." << endl;
    #endif
    DataSource* hds = dsi.list().dataSource( "dim", 0 ).get();
    dsi.header() = _buildHeader( hds );
    
    #ifdef SOMA_IO_DEBUG
      cout << "GISFORMATCHECKER:: Reading minf..." << endl;
    #endif
    string obtype = dsi.header()->getProperty( "object_type" )->getString();
    DataSource* minfds = dsi.list().dataSource( "minf", 0 ).get();
    DataSourceInfoLoader::readMinf( *minfds, dsi.header() );
    dsi.header()->setProperty( "object_type", obtype );
    
  }
  //--- write capabilities -----------------------------------------------------
  if( docapa ) {
    #ifdef SOMA_IO_DEBUG
      cout << "GISFORMATCHECKER:: Writing capabilities..." << endl;
    #endif
    dsi.capabilities().setMemoryMapping( false );
    try {
      if( !(bool) dsi.header()->getProperty( "byte_swapping" )->getScalar() 
          && !(bool) dsi.header()->getProperty( "ascii" )->getScalar() )
        dsi.capabilities().setMemoryMapping( true );
    } catch( ... ) {
    }
    dsi.capabilities().setDataSource( dsi.list().dataSource( "ima", 0 ) );
    dsi.capabilities().setThreadSafe( false ); /* TODO */
    dsi.capabilities().setOrdered( true );
    dsi.capabilities().setSeekVoxel( true );
    dsi.capabilities().setSeekLine( true );
    dsi.capabilities().setSeekSlice( true );
    dsi.capabilities().setSeekVolume( true );
  }
  //----------------------------------------------------------------------------
  #ifdef SOMA_IO_DEBUG
    cout << "GISFORMATCHECKER:: Checking done" << endl;
  #endif
  return dsi;
}


