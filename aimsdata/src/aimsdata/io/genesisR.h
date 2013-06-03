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
 *  Data reader class
 */

#ifndef AIMS_IO_GENESIDR_H
#define AIMS_IO_GENESISR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/genesisheader.h>
#include <aims/io/datatypecode.h>
#include <aims/data/data.h>
#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#ifdef USE_SOMA_IO
  #include <soma-io/datasource/filedatasource.h>
#else
  #include <cartobase/datasource/filedatasource.h>
#endif

namespace aims
{

  template<class T> class GenesisReader
  {
  public:
    GenesisReader( const std::string & name ) : _name( name ) {}
    ~GenesisReader() {}

    void read( AimsData<T> & thing, const carto::AllocatorContext & context, 
               carto::Object options );

  private:
    std::string		_name;
  };

  template <class T>
  inline GenesisReader<T> & 
  operator >> ( GenesisReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing );
    return reader;
  }

  template <class T>
  inline
  void GenesisReader<T>::read( AimsData<T>& thing, 
                               const carto::AllocatorContext & context, 
                               carto::Object options )
  {
    GenesisHeader	*hdr = new GenesisHeader( _name );
    try
      {
	hdr->read();
      }
    catch( std::exception & e )
      {
	delete hdr;
	throw;
      }

    carto::DataTypeCode<T>	dtc;
    if( hdr->dataType() != dtc.dataType() )
      throw carto::datatype_format_error
	( std::string( "data type / reader type mismatch : file is " ) 
	  + hdr->dataType() + ", expecting " + dtc.dataType(), _name );

    int	frame = -1, border = 0;
    options->getProperty( "frame", frame );
    options->getProperty( "border", border );

    int	dimt, tmin;
    if( frame >= 0 )
      {
	if( frame >= hdr->dimT() )
	  {
	    delete hdr;
	    throw std::domain_error( "frame higher than file dimT" );
	  }
	dimt = 1;
	tmin = frame;
      }
    else
      {
	dimt = hdr->dimT();
	tmin = 0;
      }

    /*cout << "dimensions: " << hdr->dimX() << ", " << hdr->dimY() << ", " 
      << hdr->dimZ() << ", " << dimt << endl;*/

    carto::AllocatorContext 
      ac( context.accessMode(), 
          carto::rc_ptr<carto::DataSource>
          ( new carto::FileDataSource( _name, 0, carto::DataSource::Read ) ), 
          false, context.useFactor() );

    AimsData<T> data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), dimt, 
		      border, ac );

    data.setSizeX( hdr->sizeX() );
    data.setSizeY( hdr->sizeY() );
    data.setSizeZ( hdr->sizeZ() );
    data.setSizeT( hdr->sizeT() );
    data.setHeader( hdr );

    // we must scandir to obtain a files list

    char	s = carto::FileUtil::separator();
    std::string::size_type	pos = _name.rfind( s );
    std::string	dirname;
    if( pos == std::string::npos )
      dirname = ".";
    else
      dirname = _name.substr( 0, pos );

    carto::Directory				dir( dirname );
    std::set<std::string>		names = dir.files();
    std::set<std::string>::iterator	in, en = names.end();
    std::string				name;
    int					z, t, ss = 0;
    std::vector<bool>			reads( data.dimT() * data.dimZ() );

    z = data.dimZ();	// temp
    for( in=names.begin(); in!=en; ++in )
      {
	name = dirname + s + *in;
	//cout << "read " << name << endl;
	GenesisHeader	h( name );	// (re)read each header
	try
	  {
	    h.read();
	    std::ifstream	is( name.c_str(), 
                                    std::ios::in | std::ios::binary );
	    if( !is )
	      carto::io_error::launchErrnoExcept( name );
            is.unsetf( std::ios::skipws );
	    is.seekg( h.dataOffset() );
	    if( !is )
	      carto::io_error::launchErrnoExcept( name );
	    if( h.compressCode() != 1 )
	      throw 
		carto::format_mismatch_error( "Genesis reader can't deal with "
					      "compressed data", name );
	    t = 0;	// temporarily
	    h.getProperty( "slice", z );
	    h.getProperty( "start_slice", ss );
            z -= ss;
	    if( z >= 0 && z < data.dimZ() && t >= 0 && t < data.dimT() )
	      {
		if( reads[ t*data.dimZ() + z ] )
		  std::cerr << "warning: slice z=" << z << ", t=" << t 
			    << " already read" << std::endl;
		else
		  reads[ t*data.dimZ() + z ] = true;
		DefaultItemReader<T>	ib1;
		ItemReader<T>	*ir = ib1.reader( "binar", h.byteSwapping() );
		for( int y=0; y<data.dimY(); ++y )  
		  ir->read( is, &data(0,y,z,t), data.dimX() );

		delete ir;
	      }
	  }
	catch( std::exception & )
	  {
	    std::cout << name << " not part of genesis volume\n";
	  }
      }

    //cout << "read finished, checking if we've got all slices...\n";
    for( t=0; t<data.dimT(); ++t )
      for( z=0; z<data.dimZ(); ++z )
	if( !reads[ t*data.dimZ() + z ] )
	  std::cerr << "warning: missing slice " << z << ", t=" << t 
		    << std::endl;
    thing = data;
  }
}

#endif
