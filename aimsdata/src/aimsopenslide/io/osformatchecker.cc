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
#include <aims/io/osformatchecker.h>                       // class declaration
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
#include <openslide.h>
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
//   P R I V A T E   M E T H O D S
//==============================================================================
//--- BUILDING DATASOURCELIST --------------------------------------------------
void OSFormatChecker::_buildDSList( DataSourceList & dsl ) const
{
  DataSource* pds = dsl.dataSource( "default", 0 ).get();
  string imaname, minfname;
  
  dsl.addDataSource( "ima", rc_ptr<DataSource>( pds ) );
  
  imaname = pds->url();
  if( !imaname.empty() ) {
    minfname = imaname + ".minf";
  }
  
  //// Minf DataSource
  if( !minfname.empty() && 
      FileUtil::fileStat( minfname ).find( '+' ) != string::npos ) {
    dsl.addDataSource( "minf", rc_ptr<DataSource>
        ( new FileDataSource( minfname ) ) );
  }
  
  #ifdef SOMA_IO_DEBUG
    cout << "OSFORMATCHECKER:: ima: " << dsl.dataSource( "ima", 0 )->url() << endl;
  #endif
  #ifdef SOMA_IO_DEBUG
    if( !dsl.empty( "minf" ) )
      cout << "OSFORMATCHECKER:: minf: " << dsl.dataSource( "minf", 0 )->url() << endl;
  #endif
}

//--- BUILDING HEADER ----------------------------------------------------------
Object OSFormatChecker::_buildHeader( DataSource* hds, Object options ) const
{
  string  fname = hds->url();
  
  openslide_t *osimage;
  if( !( osimage = openslide_open( fname.c_str() ) ) ) {
    #ifdef SOMA_IO_DEBUG
      cout << "OSFORMATCHECKER:: Not a OpenSlide header: " << fname << endl;
    #endif
    throw format_mismatch_error( "Not a OpenSlide header", fname );
  }
  
  Object  hdr = Object::value( PropertySet() );  // header
  int32_t resolution = 0;
  try {
    if( options.get() )
      resolution = options->getProperty( "resolution_level" )->getScalar();
  } catch( ... ) {
  }
  int32_t i;
  
  // resolutions sizes
  int32_t rcount = openslide_get_level_count( osimage );
  vector<vector<int64_t> > rsizes( rcount, vector<int64_t>( 4, 1 ) );
  for( i=0; i<rcount; i++ )
    openslide_get_level_dimensions( osimage, i, &rsizes[i][0], &rsizes[i][1] );
  
  // chosen resolution's downsampling
  float ds = rsizes[0][0]/rsizes[resolution][0];
  
  // chosen resolution's voxel size
  vector<float>  vs(4, 1.);
  string mppx( openslide_get_property_value( osimage, "openslide.mpp-x" ) );
  string mppy( openslide_get_property_value( osimage, "openslide.mpp-y" ) );
  vs[0] = atof( mppx.c_str() )/1000/ds;
  vs[1] = atof( mppy.c_str() )/1000/ds;
  
  openslide_close( osimage );
  
  string type = "RGBA";
  hdr->setProperty( "sizeX", rsizes[resolution][0] );
  hdr->setProperty( "sizeY", rsizes[resolution][1] );
  hdr->setProperty( "sizeZ", rsizes[resolution][2] );
  hdr->setProperty( "sizeT", rsizes[resolution][3] );
  hdr->setProperty( "resolutions_dimension", rsizes );
  hdr->setProperty( "format", string( "OpenSlide" ) );
  hdr->setProperty( "voxel_size", vs );
  hdr->setProperty( "object_type", string( "Volume of " ) + type );
  //hdr->setProperty( "ascii", 0 );
  //hdr->setProperty( "byte_swapping", 0 );
  
  return hdr;
}

//==============================================================================
//   P U B L I C   M E T H O D S
//==============================================================================/

OSFormatChecker::~OSFormatChecker()
{
}

DataSourceInfo OSFormatChecker::check( DataSourceInfo dsi, 
                                       DataSourceInfoLoader & /* f */,
                                       Object options ) const
{
  bool doread = dsi.header().isNone() ;
  bool dolist = dsi.list().nbTypes() == 1 ;
  bool docapa = !dsi.capabilities().isInit();
  //--- test header format -----------------------------------------------------
  if( !doread )
    if( !dsi.header()->hasProperty( "format" ) 
        || dsi.header()->getProperty( "format" )->getString() != "OpenSlide" )
      throw wrong_format_error( "Not a OpenSlide header", 
                                dsi.list().dataSource( "default", 0 )->url() );
  
  //--- build datasourcelist ---------------------------------------------------
  if( dolist ) {
    #ifdef SOMA_IO_DEBUG
      cout << "OSFORMATCHECKER:: Building list..." << endl;
    #endif
    _buildDSList( dsi.list() );
  }
  //--- build header -----------------------------------------------------------
  if( doread ) {
    #ifdef SOMA_IO_DEBUG
      cout << "OSFORMATCHECKER:: Reading header..." << endl;
    #endif
    DataSource* hds = dsi.list().dataSource( "ima", 0 ).get();
    dsi.header() = _buildHeader( hds, options );
    
    #ifdef SOMA_IO_DEBUG
      cout << "OSFORMATCHECKER:: Reading minf..." << endl;
    #endif
    if( !dsi.list().empty( "minf" ) ) {
      string obtype = dsi.header()->getProperty( "object_type" )->getString();
      DataSource* minfds = dsi.list().dataSource( "minf", 0 ).get();
      DataSourceInfoLoader::readMinf( *minfds, dsi.header() );
      dsi.header()->setProperty( "object_type", obtype );
    }
    
  }
  //--- write capabilities -----------------------------------------------------
  if( docapa ) {
    #ifdef SOMA_IO_DEBUG
      cout << "OSFORMATCHECKER:: Writing capabilities..." << endl;
    #endif
    dsi.capabilities().setMemoryMapping( false );
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
    cout << "OSFORMATCHECKER:: Checking done" << endl;
  #endif
  return dsi;
}


