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
 *  SPM reader template functions
 */

#include <cstdlib>
#include <aims/io/spmR.h>
#include <aims/io/datatypecode.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/spmheader.h>
#include <aims/utility/flip.h>
#include <aims/data/data.h>
#include <aims/transformation/affinetransformation3d.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/datasource/filedatasource.h>

namespace aims
{

  template< class T >
  std::string SpmReader< T >::removeExtension(const std::string& name)
  {
    std::string res = name;
    std::string ext="";
    if ( res.length() > 4 )
      ext = res.substr( res.length() - 4, 4 );
    if (ext == ".hdr" || ext == ".img")
      res = res.substr( 0, res.length() - 4 );
    return res;
  }


  template <typename T>
  void SpmReader<T>::read(AimsData<T>& thing, 
                          const carto::AllocatorContext & context, 
                          carto::Object options )
  {
    std::string name = removeExtension( _name );
  
    aims::SpmHeader *hdr = new aims::SpmHeader( name + ".hdr" );
  
    try
      {
	hdr->read();
      }
    catch( std::exception & e )
      {
	delete hdr;
	throw e;
      }
  
    std::string	type;
    if ( !hdr->getProperty( "disk_data_type", type ) )
      type = carto::DataTypeCode<T>().dataType();
    hdr->setProperty( "data_type", carto::DataTypeCode<T>().dataType() );
    hdr->setProperty( "bits_allocated", (int) sizeof( T ) );

    std::vector<int>	dims;
    std::vector<float>	vs;
    if( ! hdr->getProperty( "volume_dimension", dims ) )
      throw std::logic_error( "Internal error: getProperty volume_dimension failed" );
    if( ! hdr->getProperty( "voxel_size", vs ) )
      throw std::logic_error( "Internal error: getProperty voxel_size failed" );

    int	frame = -1, border = 0;
    options->getProperty( "frame", frame );
    options->getProperty( "border", border );

    int	dimt = 1, tmin = 0;
    if ( frame >= 0 )
      {
	if ( frame >= dims[3] )
	  {
	    throw std::domain_error( "Frame number exceeds "
				     "volume time dimension" );
	  }
	tmin = frame;
      }
    else
      dimt = dims[3];
  
    int	bswapi = false;
    hdr->getProperty( "byte_swapping", bswapi );
    bool	bswap = bswapi;
    std::string filename = name + ".img";

    bool			series = false;
    std::vector<carto::Object>	fnames;
    std::string			bname;
    if ( tmin > 0 || dimt > 1 )
      {
        series = hdr->getProperty( "series_filenames", fnames );
        if ( series )
          {
            if ( fnames.size() < (unsigned) ( dimt + tmin ) )
              throw std::domain_error( "SPM reader: not enough filenames for " 
                                       "all frames" );
            bname = carto::FileUtil::dirname( name ) 
              + carto::FileUtil::separator();
            hdr->removeProperty( "series_filenames" );
          }
      }

    carto::AllocatorContext	al 
      ( context.accessMode(), 
        carto::rc_ptr<carto::DataSource>
        ( new carto::FileDataSource( filename, 0, carto::DataSource::Read ) ), 
        false, context.useFactor() );

    //std::cout << "alloc type: " << al.allocatorType() << std::endl;
    AimsData<T> data( dims[0], dims[1], dims[2], dimt, border, al );
    data.setSizeXYZT( vs[0], vs[1] , vs[2], vs[3] );
  
    std::ifstream is;
    if ( !series )
      {
        is.open( filename.c_str(), std::ios::in | std::ios::binary );
        if ( !is )
          {
            carto::io_error::launchErrnoExcept( filename );
          }
        is.unsetf( std::ios::skipws );
        if ( type == "S8") 
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( int8_t ), std::ios::cur );
        else if ( type == "U8") 
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( uint8_t ), std::ios::cur );
        else if ( type =="S16" )
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( int16_t ), std::ios::cur );
        else if ( type =="U16" )
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( uint16_t ), std::ios::cur );
        else if ( type =="S32" )
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( int32_t ), std::ios::cur );
        else if ( type =="U32" )
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( uint32_t ), std::ios::cur );
        else if ( type == "FLOAT" )
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( float ), std::ios::cur );
        else if ( type == "DOUBLE" )
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( double ), std::ios::cur );
        else
          is.seekg( (std::ifstream::pos_type) tmin * data.dimZ() * data.dimY() 
                    * data.dimX() * sizeof( T ), std::ios::cur );
      }

    std::vector<T> buffer ( dims[0] );
    std::string	   fnm;

    std::vector<float> vstom;
    AffineTransformation3d  stom;
    if( hdr->getProperty( "storage_to_memory", vstom ) )
      stom = AffineTransformation3d( vstom );
    else
    {
      // standard orientation
      stom.rotation()(1,1) = -1;
      stom.rotation()(2,2) = -1;
      stom.matrix()(1, 3) = data.dimY() - 1;
      stom.matrix()(2, 3) = data.dimZ() - 1;
      bool        radio = false;
      try
      {
        radio = (bool) hdr->getProperty( "spm_radio_convention" )->getScalar();
      }
      catch( std::exception & )
      {
      }
      if( !radio )
      {
        stom.rotation()(0,0) = -1;
        stom.matrix()(0, 3) = data.dimX() - 1;
      }
    }

    for ( int t=0; t<dimt; ++t )
      {
        if ( series )
          {
            fnm = bname 
              + carto::FileUtil::basename( fnames[t+tmin]->getString() );
            is.open( ( fnm ).c_str(), std::ios::in | std::ios::binary );
            if ( !is )
              {
                carto::io_error::launchErrnoExcept( fnm );
              }
            is.unsetf( std::ios::skipws );
          }
        hdr->setProperty( "scale_factor_applied", false ) ;
        readFrame( is, data, t, type, hdr, bswap, stom );
        if ( series )
          is.close();
      }

    if ( !series )
      is.close();

    thing = data;
    thing.setHeader( hdr );
  }


  template<typename T> void 
  SpmReader<T>::readFrame( std::ifstream & is, AimsData<T> & data, int t, 
                           const std::string &, SpmHeader*, bool bswap, 
                           const AffineTransformation3d & stom )
  {
    DefaultItemReader<T> itemR ;
    ItemReader<T>	*ir 
      = itemR.reader( "binar", bswap );

    Point3df incf = stom.transform( Point3df( 1, 0, 0 ) )
        - stom.transform( Point3df( 0, 0, 0 ) );
    Point4d inc = Point4d( int16_t( rint( incf[0] ) ), 
                           int16_t( rint( incf[1] ) ),
                           int16_t( rint( incf[2] ) ), 0 );
    Point3df d0f;
    Point4d d0;
    std::unique_ptr<AffineTransformation3d>  m2s = stom.inverse();
    Point3df df = m2s->transformVector( Point3df( data.dimX(), data.dimY(),
                                        data.dimZ() ) );
    Point4d tdims = Point4d( short( rint( fabs( df[0] ) ) ), 
                   short( rint( fabs( df[1] ) ) ),
                   short( rint( fabs( df[2] ) ) ), (short) data.dimT() );
    int x, dx = tdims[0];
    std::vector<T> buffer( dx );

    for ( int z=0;z<tdims[2];++z)
      for ( int y=0;y<tdims[1];++y)
      {
        d0f = stom.transform( Point3df( 0, y, z ) );
        d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                      int16_t( rint( d0f[2] ) ), t );
        if( inc == Point4d( 1, 0, 0, 0 ) )
          ir->read( is, &data( d0 ), dx );
        else
          {
            ir->read( is, &buffer[0], dx );
            for ( x=0; x<dx; ++x, d0+=inc )
              data( d0 ) = buffer[ x ];
          }
      }
    delete ir ;
  }


  template <typename T> template<typename U>
  void 
  SpmReader<T>::readScaledFrame( std::ifstream & is, AimsData<T>& data, 
                                 int t, SpmHeader *hdr, bool bswap, 
                                 const AffineTransformation3d & stom )
  {
    float scaleFactor = 1;
    bool scaleFactorApplied = false;

    hdr->getProperty( "scale_factor", scaleFactor );
    hdr->getProperty( "scale_factor_applied", scaleFactorApplied );
    if ( scaleFactorApplied || scaleFactor == 0. )
      scaleFactor = 1. ;

    DefaultItemReader<U> itemR ;
    ItemReader<U>	*ir 
      = itemR.reader( "binar", bswap );      

    Point3df incf = stom.transform( Point3df( 1, 0, 0 ) )
        - stom.transform( Point3df( 0, 0, 0 ) );
    Point4d inc = Point4d( int16_t( rint( incf[0] ) ), 
                           int16_t( rint( incf[1] ) ),
                           int16_t( rint( incf[2] ) ), 0 );
    Point3df d0f;
    Point4d d0;
    std::unique_ptr<AffineTransformation3d>  m2s = stom.inverse();
    Point3df df = m2s->transformVector( Point3df( data.dimX(), data.dimY(),
                                        data.dimZ() ) );
    Point4d tdims = Point4d( short( rint( fabs( df[0] ) ) ), 
                   short( rint( fabs( df[1] ) ) ),
                   short( rint( fabs( df[2] ) ) ), data.dimT() );
    int x, dx = tdims[0];
    std::vector<U> buffer( dx );

    for ( int z=0;z<tdims[2];++z)
      for ( int y=0;y<tdims[1];++y)
      {
        d0f = stom.transform( Point3df( 0, y, z ) );
        d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                      int16_t( rint( d0f[2] ) ), t );
        ir->read( is, &buffer[0] , dx );
        for ( x=0; x<dx; ++x, d0+=inc )
          data( d0 ) = (T) ( scaleFactor * buffer[ x ] );
      }
    delete ir;
    hdr->setProperty( "scale_factor_applied", true ) ;
  }


  template<> void 
  SpmReader<float>::readFrame( std::ifstream & is, AimsData<float> & data, 
                               int t, const std::string & type, 
                               SpmHeader* hdr, bool bswap, 
                               const AffineTransformation3d & stom )
  {
    if ( type == "S8") 
      readScaledFrame<int8_t>( is, data, t, hdr, bswap, stom );
    else if ( type == "U8") 
      readScaledFrame<uint8_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="S16" )
      readScaledFrame<int16_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="U16" )
      readScaledFrame<uint16_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="S32" )
      readScaledFrame<int32_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="U32" )
      readScaledFrame<uint32_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="FLOAT" )
      readScaledFrame<float>( is, data, t, hdr, bswap, stom );
    else if ( type =="DOUBLE" )
      readScaledFrame<double>( is, data, t, hdr, bswap, stom );
    else
      std::cerr << "SPM read not implemented: " << type << " as FLOAT" 
                << std::endl;
  }

  template<> void 
  SpmReader<double>::readFrame( std::ifstream & is, AimsData<double>& data, 
                                int t, const std::string & type, 
                                SpmHeader* hdr, bool bswap,
                                const AffineTransformation3d & stom )
  {
    if ( type == "S8") 
      readScaledFrame<int8_t>( is, data, t, hdr, bswap, stom );
    else if ( type == "U8") 
      readScaledFrame<uint8_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="S16" )
      readScaledFrame<int16_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="U16" )
      readScaledFrame<uint16_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="S32" )
      readScaledFrame<int32_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="U32" )
      readScaledFrame<uint32_t>( is, data, t, hdr, bswap, stom );
    else if ( type =="FLOAT" )
      readScaledFrame<float>( is, data, t, hdr, bswap, stom );
    else if ( type =="DOUBLE" )
      readScaledFrame<double>( is, data, t, hdr, bswap, stom );
    else
      std::cerr << "SPM read not implemented: " << type << " as DOUBLE" 
                << std::endl;
  }

}

