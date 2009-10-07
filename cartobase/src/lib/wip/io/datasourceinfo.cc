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

#include <cartobase/wip/io/datasourceinfo.h>
#include <cartobase/wip/io/formatchecker.h>
#include <cartobase/object/property.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/datasource/datasource.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/object/pythonreader.h>
#include <cartobase/exception/file.h>
#include <cartobase/wip/io/reader.h>
#include <memory>
#include <iostream>

// #define CARTO_IO_DEBUG

using namespace carto;
using namespace std;


namespace
{

  struct Private_Static
  {
    Private_Static();
    map<string, rc_ptr<FormatChecker> >	formats;
    multimap<string, string>		extensions;
  };


  Private_Static::Private_Static()
  {
  }


  inline Private_Static & getstatic()
  {
    static auto_ptr<Private_Static>	ps( new Private_Static );
    return *ps;
  }

}


struct DataSourceInfo::Private
{
  Private();

  State		state;
  string	errormsg;
  int		errorcode;
};


DataSourceInfo::Private::Private()
  : state( Unchecked ), errorcode( -1 )
{
}


DataSourceInfo::DataSourceInfo()
  : d( new Private )
{
}


DataSourceInfo::~DataSourceInfo()
{
  delete d;
}


DataSourceInfo::State DataSourceInfo::state() const
{
  return d->state;
}


string DataSourceInfo::errorMessage() const
{
  return d->errormsg;
}


void DataSourceInfo::launchException()
{
  if( d->state != Error || d->errorcode < 0 )
    return;	// no error (?)
  string	defmsg;
  if( d->errormsg.empty() )
    defmsg = "could not identify format";
  io_error::launchExcept( d->errorcode, d->errormsg, defmsg );
}


void DataSourceInfo::registerFormat( const string & fmtid, 
                                     FormatChecker* format, 
                                     const vector<string> & extensions )
{
  Private_Static	& ps = getstatic();
  ps.formats[ fmtid ].reset( format );

  vector<string>::const_iterator	ie, ee = extensions.end();
  for( ie=extensions.begin(); ie!=ee; ++ie )
    ps.extensions.insert( pair<string, string>( *ie, fmtid ) );
}


FormatChecker* DataSourceInfo::formatInfo( const string & format )
{
  Private_Static	& ps = getstatic();

  map<string, rc_ptr<FormatChecker> >::const_iterator	i 
    = ps.formats.find( format );
  if( i == ps.formats.end() )
    return( 0 );
  return( i->second.get() );
}


Object DataSourceInfo::check( DataSource & ds )
{
#ifdef CARTO_IO_DEBUG
  cout << "DataSourceInfo::check()\n";
#endif
  static bool plugs = false;
  if( !plugs )
    {
      plugs = true;
      PluginLoader::load();
    }

  d->state = Unchecked;
  d->errorcode = -1;
  d->errormsg = "";

  //	find filename extension if it's a file
  string		ext;
  int			excp = 0;
#ifdef CARTO_IO_DEBUG
  cout << "filename: " << ds.url() << endl;
#endif
  ext = FileUtil::extension( ds.url() );
#ifdef CARTO_IO_DEBUG
  cout << "ext : " << ext << endl;
#endif

  //	Check compatible formats
  set<string>			tried;
  FormatChecker			*reader;
  set<string>::iterator	notyet = tried.end();
  Private_Static		& ps = getstatic();

  pair<multimap<string, string>::const_iterator, 
    multimap<string, string>::const_iterator>	iext 
    = ps.extensions.equal_range( ext );
  multimap<string, string>::const_iterator	ie, ee = iext.second;
  carto::offset_t				dspos = ds.at();

  // try every matching format until one works
  for( ie=iext.first; ie!=ee; ++ie )
    if( tried.find( ie->second ) == notyet )
      {
#ifdef CARTO_IO_DEBUG
	cout << "trying " << (*ie).second << "...\n";
#endif
	reader = formatInfo( ie->second );
	if( reader )
	  {
	    try
	      {
                d->state = Ok;
		return reader->check( ds, *this );
	      }
	    catch( exception & e )
	      {
                d->state = Error;
		io_error::keepExceptionPriority( e, excp, d->errorcode, 
						 d->errormsg );
                ds.at( dspos );
	      }
	    tried.insert( ie->second );
	  }
      }

#ifdef CARTO_IO_DEBUG
  cout << "not found yet... pass2...\n";
#endif
  if( !ext.empty() )
    {
      // not found or none works: try readers with no extension
      iext = ps.extensions.equal_range( "" );

      for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
	if( tried.find( ie->second ) == notyet )
	  {
#ifdef CARTO_IO_DEBUG
            cout << "pass2, trying " << (*ie).second << "...\n";
#endif
	    reader = formatInfo( ie->second );
	    if( reader )
	      {
		try
		  {
                    d->state = Ok;
		    return reader->check( ds, *this );
		  }
		catch( exception & e )
		  {
                    d->state = Error;
		    io_error::keepExceptionPriority( e, excp, d->errorcode, 
						     d->errormsg );
                    ds.at( dspos );
		  }
		tried.insert( ie->second );
	      }
	  }
    }

#ifdef CARTO_IO_DEBUG
  cout << "not found yet... pass3...\n";
#endif
  // still not found ? well, try EVERY format this time...
  iext.first = ps.extensions.begin();
  iext.second = ps.extensions.end();

  for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
    if( tried.find( ie->second ) == notyet )
      {
	reader = formatInfo( ie->second );
	if( reader )
	  {
#ifdef CARTO_IO_DEBUG
            cout << "pass3, trying " << (*ie).second << "...\n";
#endif
	    try
	      {
                d->state = Ok;
		return reader->check( ds, *this );
	      }
	    catch( exception & e )
	      {
                d->state = Error;
		io_error::keepExceptionPriority( e, excp, d->errorcode, 
						 d->errormsg );
                ds.at( dspos );
	      }
	    tried.insert( ie->second );
	  }
      }

#ifdef CARTO_IO_DEBUG
  cout << "not found at all, giving up\n";
#endif
  // still not succeeded, it's hopeless...

  d->state = Error;
  if( d->errorcode < 0 )
    {
      d->errorcode = 0;
      d->errormsg = ds.url() + " : could not identify format";
    }
  //launchException();
  return none();
}


SyntaxSet & DataSourceInfo::minfSyntax()
{
  static SyntaxSet	pheader_syntax;
  if( pheader_syntax.empty() )
    {
      // we don't absolutely need a syntax, but some pieces of code may 
      // expect it
      Syntax	&sx = pheader_syntax[ "__generic__" /*"PythonHeader"*/ ];

      sx[ "_borderWidth" ] = Semantic( "int", false, true );
      sx[ "sizeX"        ] = Semantic( "int", false, true );
      sx[ "sizeY"        ] = Semantic( "int", false, true );
      sx[ "sizeZ"        ] = Semantic( "int", false, true );
      sx[ "sizeT"        ] = Semantic( "int", false, true );

      sx[ "data_type" ] = Semantic( "string" );
      sx[ "object_type" ] = Semantic( "string" );
      sx[ "disk_data_type" ] = Semantic( "string", false, true );
      sx[ "possible_data_type" ] = Semantic( "string_vector" );
      sx[ "ascii" ] = Semantic( "int", false, true );
      sx[ "byte_swapping" ] = Semantic( "int", false, true );
      sx[ "spm_normalized" ] = Semantic( "bool" );
      sx[ "origin" ] = Semantic( "float_vector" );
      sx[ "spm_origin" ] = Semantic( "float_vector" );
      //sx[ "origin" ] = Semantic( DataTypeCode<vector<float> >::name() );
      sx[ "transfer_syntax" ] = Semantic( "string" );
      sx[ "manufacturer" ] = Semantic( "string" );
      sx[ "modality" ] = Semantic( "string" );
      sx[ "patient_id" ] = Semantic( "string" );
      sx[ "study_id" ] = Semantic( "string" );
      sx[ "series_number" ] = Semantic( "int" );
      sx[ "volume_dimension" ] = Semantic( "int_vector" );
      sx[ "voxel_size" ] = Semantic( "float_vector" );
      sx[ "filter_param" ] = Semantic( "float_vector" );
      sx[ "bits_allocated" ] = Semantic( "int" );
      sx[ "time_resolution" ] = Semantic( "float" );
      sx[ "te" ] = Semantic( "float" );
      sx[ "tr" ] = Semantic( "float" );
      sx[ "flip_angle" ] = Semantic( "float" );
      sx[ "slice_thickness" ] = Semantic( "float" );
      sx[ "inter_slices" ] = Semantic( "float" );
      sx[ "isotope_halflife" ] = Semantic( "float" );
      sx[ "category" ] = Semantic( "string" );
      sx[ "b_value" ] = Semantic( "float" );
      sx[ "scale_factor" ] = Semantic( "float" );
      sx[ "scale_offset" ] = Semantic( "float" );
      sx[ "nb_t_pos" ] = Semantic( "int" );
      sx[ "process_code" ] = Semantic( "int" );
      sx[ "filter_code" ] = Semantic( "int" );
      sx[ "polygon_dimension" ] = Semantic( "int" );
      sx[ "vertex_number" ] = Semantic( "int" );
      sx[ "start_time" ] = Semantic( "int_vector" );
      sx[ "duration_time" ] = Semantic( "int_vector" );
      sx[ "isotope" ] = Semantic( "string" );
      sx[ "image_unit" ] = Semantic( "string" );
      sx[ "radiopharmaceutical" ] = Semantic( "string" );
      sx[ "process_list" ] = Semantic( "string" );
      sx[ "zero_start_time" ] = Semantic( "S32" );
      sx[ "original_filename" ] = Semantic( "string" );
      sx[ "filter_type" ] = Semantic( "string" );
      sx[ "scale_factor_applied" ] = Semantic( "bool", false, true );
      sx[ "vox_units" ] = Semantic( "string" );
      sx[ "cal_units" ] = Semantic( "string" );
      sx[ "db_name" ] = Semantic( "string" );
      sx[ "aux_file" ] = Semantic( "string" );
      sx[ "generated" ] = Semantic( "string" );
      sx[ "scannum" ] = Semantic( "string" );
      sx[ "exp_date" ] = Semantic( "string" );
      sx[ "exp_time" ] = Semantic( "string" );
      sx[ "SPM_data_type" ] = Semantic( "string", false, true );
      sx[ "orient" ] = Semantic( "int" );
      sx[ "views" ] = Semantic( "int" );
      sx[ "start_field" ] = Semantic( "int" );
      sx[ "field_skip" ] = Semantic( "int" );
      sx[ "omax" ] = Semantic( "int" );
      sx[ "omin" ] = Semantic( "int" );
      sx[ "smax" ] = Semantic( "int" );
      sx[ "smin" ] = Semantic( "int" );
      sx[ "minimum" ] = Semantic( "int" );
      sx[ "maximum" ] = Semantic( "int" );
      sx[ "history" ] = Semantic( "string_vector" );
      sx[ "ecat_file_type" ] = Semantic( "string" );
      sx[ "ecat_system_type" ] = Semantic( "short" );
      sx[ "ecat_file_type" ] = Semantic( "short" );
      sx[ "ecat_start_scan_time" ] = Semantic( "unsigned int" );
      sx[ "ecat_acquisition_type" ] = Semantic( "" );
      sx[ "ecat_acquisition_mode" ] = Semantic( "short" );
      sx[ "ecat_original_file_name" ] = Semantic( "string" );
      sx[ "ecat_radiopharmaceutical" ] = Semantic( "string" );
      sx[ "ecat_plane_separation" ] = Semantic( "float" );
      sx[ "ecat_bin_size" ] = Semantic( "float" );
      sx[ "ecat_corrections_applied" ] = Semantic( "short" );
      sx[ "ecat_prompts" ] = Semantic( "int_vector" );
      sx[ "ecat_delayed" ] = Semantic( "int_vector" );
      sx[ "ecat_multiples" ] = Semantic( "int_vector" );
      sx[ "ecat_prompts" ] = Semantic( "int_vector" );
      sx[ "ecat_net_trues" ] = Semantic( "int_vector" );
      sx[ "ecat_tot_avg_uncor" ] = Semantic( "float_vector" );
      sx[ "ecat_tot_avg_cor" ] = Semantic( "float_vector" );
      sx[ "ecat_total_coin_rate" ] = Semantic( "int_vector" );
      sx[ "ecat_loss_correction_fctr" ] = Semantic( "float_vector" );
      sx[ "ecat_uncor_singles" ] = Semantic( "float_vector" );
      sx[ "ecat_start_time" ] = Semantic( "int_vector" );
      sx[ "ecat_duration_time" ] = Semantic( "int_vector" );
      sx[ "ecat_x_resolution" ] = Semantic( "float" );
      sx[ "storage_to_memory" ] = Semantic( "float_vector" );
      sx[ "referentials" ] = Semantic( "string_vector" );

      sx[ "textures" ] = Semantic( "vector of texture of FLOAT", false, true );

      pheader_syntax[ "int_vector" ][ "" ] = Semantic( "int" );
      pheader_syntax[ "float_vector" ][ "" ] = Semantic( "float" );
    }

  return pheader_syntax;
}


Object DataSourceInfo::readMinf( DataSource & ds, Object base )
{
  Object	minf;
  if( base.get() )
    minf = base;
  else
    minf = Object::value( PropertySet() );

  string	filename = ds.url();
  if( filename.empty() )
    return minf;
  if( FileUtil::extension( filename ) != "minf" )
    filename += ".minf";

  Object	opts = Object::value( PropertySet() );
  opts->setProperty( "syntaxset", minfSyntax() );

  try
    {
      Reader<GenericObject>	rg( filename );
      rg.setOptions( opts );
      bool hasbs = minf->hasProperty( "byte_swapping" );
      int bs = 0;
      if( hasbs )
        hasbs = minf->getProperty( "byte_swapping", bs );
      rg.read( *minf );
      // remove some forbidden properties
      if( minf->hasProperty( "object_type" ) )
        minf->removeProperty( "object_type" );
      if( minf->hasProperty( "data_type" ) )
        minf->removeProperty( "data_type" );
      if( minf->hasProperty( "item_type" ) )
        minf->removeProperty( "item_type" );
      if( hasbs )
        minf->setProperty( "byte_swapping", bs );
      else if( minf->hasProperty( "byte_swapping" ) )
        minf->removeProperty( "byte_swapping" );
      return minf;
    }
  catch( exception & e )
    {
    }
  catch( ... )
    {
      cerr << "Unknown exception" << endl;
    }

  return minf;
}


