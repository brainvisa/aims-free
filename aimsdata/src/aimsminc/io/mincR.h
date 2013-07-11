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

#ifndef AIMS_IO_MINCR_H
#define AIMS_IO_MINCR_H

#include <aims/config/aimsminc_config.h>

//MINC I/O support
extern "C" {
/* redefine some stupid macros that conflict with 
   those of the stupid system on Windows
*/
#define BOOLEAN MINC_BOOLEAN
#define FLOAT MINC_FLOAT
#define DOUBLE MINC_DOUBLE
#define SHORT MINC_SHORT
#include <cstdlib>
#include <volume_io.h>
#ifdef AIMS_HAS_MINC2
#include <minc2.h>
#endif
#undef SHORT
#undef DOUBLE
#undef FLOAT
#undef BOOLEAN
#undef X
#undef Y
}

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/datatypecode.h>
#include <aims/data/data.h>
#include <aims/transformation/affinetransformation3d.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/thread/mutex.h>
#ifdef USE_SOMA_IO
  #include <soma-io/allocator/mappingcopy.h>
  #include <soma-io/allocator/mappingro.h>
  #include <soma-io/allocator/mappingrw.h>
  #include <soma-io/datasource/filedatasource.h>
#else
  #include <cartobase/allocator/mappingcopy.h>
  #include <cartobase/allocator/mappingro.h>
  #include <cartobase/allocator/mappingrw.h>
  #include <cartobase/datasource/filedatasource.h>
#endif

#include <aims/io/mincheader.h>


namespace aims
{

  template<class T>
  /**
     A class for reading MINC files (from the Montreal Neurological Institute)
     It is still under development.
 
    Any type "T" can be instantiated, regardless of the actual file type.
    It is recommended to use S16 (short) types for reading MRI and label volumes and to use FLOAT for everything else.
    The constructor also specifies the "read mode". It can be either "real" or "voxel" depending on wether one wants to read "real values" (i.e. with the scale factor applied) or "voxel values". Default value is "real". It is strongly recommended to use the "voxel" mode for MRI volumes and "real" mode for all others (including label volumes).
  */
  class AIMSDATA_API MincReader
  {
  public:
    /**
     Constructor.
     @param name The name of the MINC file must be provided.
     @param read_mode The read mode is optional : it can be either "real" or "voxel" depending on wether one wants to read "real values" (i.e. with the scale factor applied) or "voxel values". Default value is "real". It is strongly recommended to use the "voxel" mode for MRI volumes and "real" mode for all others (including label volumes).
    */
    MincReader( const std::string& name, const std::string& read_mode="real", ItemReader<T>* ir = 0 ) 
      : _name( name ), _read_mode(read_mode), _itemr( ir )
    {}
    ~MincReader() { delete _itemr; }
    
    /**
     Read the MINC volume.
    */
    void read( AimsData<T>& thing, const carto::AllocatorContext & context, 
               carto::Object options );
 
    /// set read mode
    /// The read mode can be either "real" or "voxel"
    void setReadMode(const std::string & mode) { _read_mode=mode; }

    void setItemReader( ItemReader<T>* ir )
      { delete _itemr; _itemr = ir; }

  private:
    void readMinc1( AimsData<T> & data, int tmin, int dimt );
#ifdef AIMS_HAS_MINC2
    void readMinc2( AimsData<T> & data, int tmin, int dimt );
    static mitype_t minc2TypeCode();
#endif

    std::string		_name;
    std::string         _read_mode;
    ItemReader<T>	*_itemr;
  };


#ifdef AIMS_HAS_MINC2
  template <> inline mitype_t MincReader<int8_t>::minc2TypeCode()
  {
    return MI_TYPE_BYTE;
  }


  template <> inline mitype_t MincReader<uint8_t>::minc2TypeCode()
  {
    return MI_TYPE_UBYTE;
  }


  template <> inline mitype_t MincReader<int16_t>::minc2TypeCode()
  {
    return MI_TYPE_SHORT;
  }


  template <> inline mitype_t MincReader<uint16_t>::minc2TypeCode()
  {
    return MI_TYPE_USHORT;
  }


  template <> inline mitype_t MincReader<int32_t>::minc2TypeCode()
  {
    return MI_TYPE_INT;
  }


  template <> inline mitype_t MincReader<uint32_t>::minc2TypeCode()
  {
    return MI_TYPE_UINT;
  }


  template <> inline mitype_t MincReader<float>::minc2TypeCode()
  {
    return MI_TYPE_FLOAT;
  }


  template <> inline mitype_t MincReader<double>::minc2TypeCode()
  {
    return MI_TYPE_DOUBLE;
  }


  template <> inline mitype_t MincReader<cfloat>::minc2TypeCode()
  {
    return MI_TYPE_FCOMPLEX;
  }


  template <> inline mitype_t MincReader<cdouble>::minc2TypeCode()
  {
    return MI_TYPE_DCOMPLEX;
  }
#endif


  template <class T>
  inline AIMSDATA_API MincReader<T> & 
  operator >> ( MincReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing );
    return reader;
  }


  template <class T>
  inline
  void MincReader<T>::readMinc1( AimsData<T>& data, int tmin, int dimt )
  {
    Header *h = data.header();
    MincHeader *hdr = static_cast<MincHeader *>( h );

    Volume volume;
    STRING dim_names[4];
    STRING fileName = create_string ( (char*)_name.c_str());



    dim_names[0] = create_string( const_cast<char*>( MIzspace ) );
    dim_names[1] = create_string( const_cast<char*>( MIyspace ) );
    dim_names[2] = create_string( const_cast<char*>( MIxspace ) );
    dim_names[3] = create_string( const_cast<char*>( MItime ) );
    set_print_error_function(MincHeader::my_empty_print_error);

    /*      int n_dimensions=4;

    if(thing.dimT()==1) {
      if(thing.dimZ()==1) {
        if(thing.dimY()==1) {
          n_dimensions=1;
        }
        else n_dimensions=2;
      }
      else n_dimensions=3;
      }

      std::cout << "n_dimensions: " << n_dimensions << "\n";*/
    MincHeader::mincMutex().lock();
    int res = input_volume( fileName, 0, dim_names,
                      MI_ORIGINAL_TYPE, TRUE,
                      0.0, 0.0, TRUE, &volume,
                      (minc_input_options *) NULL );
    MincHeader::mincMutex().unlock();
    if( res != OK )
    {
      std::cout << "input_volume failed.\n";
      throw carto::corrupt_stream_error( "could not read MINC file",
                                          _name );
    }


    //Handle positive/negative voxel size.
    //Variables dirX, dirY and dirZ are defined as 1 if voxel size is positive and as -1 if voxel size is negative.
    int dirX= (int)(volume->separations[2]/fabs(volume->separations[2]));
    int dirY= (int)(volume->separations[1]/fabs(volume->separations[1]));
    int dirZ= (int)(volume->separations[0]/fabs(volume->separations[0]));

    //Variables X_pos, Y_pos and Z_pos are defined as 1 if voxel size is positive and 0 if voxel size is negative.
    //This is further used during the actual reading.
    int X_pos,Y_pos,Z_pos;
    if(dirX>0) X_pos=1; 
    else X_pos=0;
    if(dirY>0) Y_pos=1; 
    else Y_pos=0;
    if(dirZ>0) Z_pos=1; 
    else Z_pos=0;

    //std::cout << "X:"<< dirX<<","<<X_pos<<"Y:"<< dirY<<","<<Y_pos<<"Z:"<< dirZ<<","<<Z_pos<<"\n";

    //std::cout << "reading\n";

    carto::DataTypeCode<T>  dtc;
    //std::cout << "Output type: " << dtc.dataType() << "\n";

    //Read the volume according to the output data type (T, which can be U8, S8, S16, FLOAT...) and to the read mode which can be "real" or "voxel"
    //In every case, when the voxel size is negative the values are read "as usual" and when the voxel size is positive, values are read using a "mirror symmetry". (this is quite odd, I don't know why it is not the other way round.
    //Use the read mode "real" (i.e. read real values using the function "get_volume_real_value"
    if(_read_mode=="real")
    {
      std::cout << "minc read\n";
      //If the output type is integer, "round" the values to the nearest integer (mostly necessary for label volumes)
      if ( (dtc.dataType()=="U8") || (dtc.dataType()=="S8")  || (dtc.dataType()=="U16") || (dtc.dataType()=="S16") || (dtc.dataType()=="U32") || (dtc.dataType()=="S32"))
      {
        for( int t=0; t<dimt; ++t )
          for( int z=0; z<data.dimZ(); ++z )  
            for( int y=0; y<data.dimY(); ++y )  
              for( int x=0; x<data.dimX(); ++x )
              {
                //data(x,y,z,t)=(T)rint(get_volume_real_value( volume, z, y, x, t, 0 ));
                data(x,y,z,t) = (T)rint(get_volume_real_value(
                  volume,
                  (Z_pos)*data.dimZ()-dirZ*z-(Z_pos),
                  (Y_pos)*data.dimY()-dirY*y-(Y_pos),
                  (X_pos)*data.dimX()-dirX*x-(X_pos) , t, 0 ));
              }
      }
      else {
        for( int t=0; t<dimt; ++t )
          for( int z=0; z<data.dimZ(); ++z )  
            for( int y=0; y<data.dimY(); ++y )  
              for( int x=0; x<data.dimX(); ++x ) {
                //data(x,y,z,t)=(T)get_volume_real_value( volume, z, y, x, t, 0 );
                data(x,y,z,t)=(T)get_volume_real_value( volume, (Z_pos)*data.dimZ()-dirZ*z-(Z_pos), (Y_pos)*data.dimY()-dirY*y-(Y_pos), (X_pos)*data.dimX()-dirX*x-(X_pos) , t, 0 );
              }
      }

    }
    //Use the read mode "voxel" (i.e. read voxel values using the function "get_volume_voxel_value"
    else
    {
      std::cout << "minc voxel mode\n";
      for( int t=0; t<dimt; ++t )
        for( int z=0; z<data.dimZ(); ++z )  
          for( int y=0; y<data.dimY(); ++y )  
            for( int x=0; x<data.dimX(); ++x ) {
              //data(x,y,z,t)=(T)get_volume_voxel_value( volume, z, y, x, t, 0 );
              data(x,y,z,t) = (T)get_volume_voxel_value(
                volume, (Z_pos)*data.dimZ()-dirZ*z-(Z_pos),
                (Y_pos)*data.dimY()-dirY*y-(Y_pos),
                (X_pos)*data.dimX()-dirX*x-(X_pos) , t, 0 );
            }
    }
    delete_volume(volume);
    delete_string(fileName);
    delete_string(dim_names[0]);
    delete_string(dim_names[1]);
    delete_string(dim_names[2]);
    delete_string(dim_names[3]);
  }


#ifdef AIMS_HAS_MINC2
  template <class T>
  inline
  void MincReader<T>::readMinc2( AimsData<T>& data, int tmin, int dimt )
  {
    Header *h = data.header();
    MincHeader *hdr = static_cast<MincHeader *>( h );

    mihandle_t    minc_volume;
    int result, i;
    double voxel;

    result = miopen_volume( _name.c_str(), MI2_OPEN_READ, &minc_volume);
    if (result != MI_NOERROR)
    {
      std::cerr << "Error opening input file: " << result << std::endl;
      throw carto::corrupt_stream_error( "could not read MINC file",
                                          _name );
    }

    T *pdata;
    long xinc, yinc;

    int ndim;
    miget_volume_dimension_count( minc_volume, MI_DIMCLASS_ANY,
                                  MI_DIMATTR_ALL, &ndim );
    std::vector<midimhandle_t> dimensions( ndim );
    double        world_location[3];
    double        dvoxel_location[3];
    std::vector<unsigned long> start( ndim ), count( ndim );
    std::vector<unsigned int>  sizes( ndim );

    miget_volume_dimensions( minc_volume, MI_DIMCLASS_ANY,
      MI_DIMATTR_ALL, MI_DIMORDER_FILE, ndim, &dimensions[0] );
    miget_dimension_sizes( &dimensions[0], ndim, &sizes[0] );

    // std::cout << "ndim: " << ndim << std::endl;

    std::vector<int> dimindex( ndim );
    std::vector<int> vdimindex( ndim );
    if( ndim < 4 )
    {
      dimindex.push_back( 0 );
      vdimindex.push_back( 0 );
      start.push_back( 0 );
      count.push_back( 1 );
      sizes.push_back( 1 );
    }
    int indoffset = 0;
    if( ndim < 4 )
    {
      indoffset = 1;
      vdimindex[0] = 3;
    }
    for( i=0; i<ndim; ++i )
    {
      char* dimname;
      miget_dimension_name( dimensions[i], &dimname );
      if( std::string( "xspace" ) == dimname )
        vdimindex[0] = i+indoffset;
      else if( std::string( "yspace" ) == dimname )
        vdimindex[1] = i+indoffset;
      else if( std::string( "zspace" ) == dimname )
        vdimindex[2] = i+indoffset;
      else if( std::string( "time" ) == dimname )
        vdimindex[3] = i+indoffset;
      else
        std::cout << "unknown dimension " << dimname << std::endl;
      free( dimname );
    }
    if( ndim < 4 )
    {
      dimindex[3] = 3;
      dimindex[0] = 3; // index in loop
      dimindex[1] = 0;
      dimindex[2] = 1;
      dimindex[3] = 2;
    }
    else
    {
      dimindex[0] = 0; // index in loop
      dimindex[1] = 1;
      dimindex[2] = 2;
      dimindex[3] = 3;
    }
    /*
    std::cout << "dimindex: " << dimindex[0] << ", " << dimindex[1] << ", " << dimindex[2] << ", " << dimindex[3] << std::endl;
    std::cout << "vdimindex: " << vdimindex[0] << ", " << vdimindex[1] << ", " << vdimindex[2] << ", " << vdimindex[3] << std::endl;
    */

    // get increments (and "real" storage_to_memory)

    AffineTransformation3d s2m;
    // and flip all axes to switch between MNI and AIMS conventions
    AffineTransformation3d flip;
    flip.rotation()( 0, 0 ) = -1;
    flip.rotation()( 1, 1 ) = -1;
    flip.rotation()( 2, 2 ) = -1;
    s2m = flip * s2m;

    // fix offsets
    s2m.translation()[0] = data.dimX() - 1;
    s2m.translation()[1] = data.dimY() - 1;
    s2m.translation()[2] = data.dimZ() - 1;

    Point4df p( 0.F, 0.F, 0.F, 1.F );
    Point3df ptrans = s2m.transform( Point3df( p[ vdimindex[0] ],
                                     p[ vdimindex[1] ],
                                     p[ vdimindex[2] ] ) ) 
      - s2m.transform( Point3df( 0.F ) );
    xinc = &data( int( rint( ptrans[0] ) ), int( rint( ptrans[1] ) ), 
                 int( rint( ptrans[2] ) ), int( rint( p[ vdimindex[3] ] ) ) )
      - &data( 0 );

    /*
    std::cout << "pos trans: " << ptrans << ", t trans: " << int( rint( p[ vdimindex[3] ] ) ) << std::endl;
    std::cout << "sizes: " << sizes[0] << ", " << sizes[1] << ", " << sizes[2] << ", " << sizes[3] << std::endl;
    */

    count[0] = 1;
    count[1] = 1;
    count[2] = 1;
    count[3] = 1;
    count[ndim-1] = sizes[ ndim-1 ];

    int vdimx, vdimy, vdimz, vdimt;
    vdimx = sizes[ dimindex[3] ];
    vdimy = sizes[ dimindex[2] ];
    vdimz = sizes[ dimindex[1] ];
    vdimt = sizes[ dimindex[0] ];
    // std::cout << "vdim now: " << vdimx << ", " << vdimy << ", " << vdimz << ", " << vdimt << std::endl;

    std::vector<double> slab;
    double *buf;
    slab.resize( count[ndim-1] );

    for( int t=0; t<vdimt; ++t )
    {
      start[ dimindex[0] ] = t;

      for( int z=0; z<vdimz; ++z )
      {
        start[dimindex[1]] = z;
        for( int y=0; y<vdimy; ++y )
        {
          Point4df ipos = Point4df( t, z, y, 0 ); // minc indices order
          Point3df vpos( ipos[ vdimindex[0] ], ipos[ vdimindex[1] ], 
                         ipos[ vdimindex[2] ] );
          Point3df pos = s2m.transform( vpos );
          // std::cout << "ipos: " << ipos << ", vpos: " << vpos << ", pos: " << pos << ", t: " << ipos[ vdimindex[3] ] << std::endl;
          pdata = &data( int( rint( pos[0] ) ), int( rint( pos[1] ) ), 
                         int( rint( pos[2] ) ), 
                         int( rint( ipos[ vdimindex[3] ] ) ) );
          start[ dimindex[2] ] = y;
          start[ dimindex[3] ] = 0;
          buf = &slab[0];
          if( miget_voxel_value_hyperslab( minc_volume, MI_TYPE_DOUBLE /*minc2TypeCode()*/,
              &start[0], &count[0], buf ) != MI_NOERROR )
          {
            miclose_volume( minc_volume );
            throw carto::corrupt_stream_error( "read error in MINC file",
                                                _name );
          }

          for( int x=0; x<vdimx; ++x, pdata += xinc )
            *pdata = (T) *buf++;
        }
      }
    }
    miclose_volume( minc_volume );
  }
#endif


  template <class T>
  inline
  void MincReader<T>::read( AimsData<T>& thing, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    //cout << "reading MINC (new version)...\n";
    MincHeader	*hdr = new MincHeader( _name );
    //cout << "(header allocated)\n";
    try
    {
      hdr->read();
    }
    catch( std::exception & )
    {
      //cout << "wrong header\n";
      delete hdr;
      throw;
    }

    int	frame = -1, border = 0;
    options->getProperty( "frame", frame );
    options->getProperty( "border", border );

    //std::cout << "header read OK\n";

    int	dimt, tmin;
    if( frame >= 0 )
    {
      if( frame >= hdr->dimT() )
      {
        delete hdr;
        throw carto::invalid_format_error( "frame higher than file dimT",
                                            _name );
      }
      dimt = 1;
      tmin = frame;
    }
    else
    {
      dimt = hdr->dimT();
      tmin = 0;
    }

    std::string name = hdr->removeExtension( _name ) + ".mnc";

    carto::AllocatorContext	al 
      ( context.accessMode(), 
        carto::rc_ptr<carto::DataSource>
        ( new carto::FileDataSource( name, 0, carto::DataSource::Read ) ), 
        false, context.useFactor() );

    AimsData<T> data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), dimt, 
                      border, al );

    al = data.allocator();
    data.setSizeX( hdr->sizeX() );
    data.setSizeY( hdr->sizeY() );
    data.setSizeZ( hdr->sizeZ() );
    data.setSizeT( hdr->sizeT() );
    data.setHeader( hdr );

    if( al.allocatorType() != carto::AllocatorStrategy::ReadOnlyMap
        && al.allocatorType() != carto::AllocatorStrategy::ReadWriteMap )
    {
      // std::cout << "reading...\n";

#ifdef AIMS_HAS_MINC2
      try
      {
        readMinc2( data, tmin, dimt );
      }
      catch( ... )
      {
        // std::cout << "Minc2 read failed, using volume_io\n";
        readMinc1( data, tmin, dimt );
      }
#else
      // only Minc1 available
      readMinc1( data, tmin, dimt );
#endif
    }

    thing = data;

    //Should we delete the header "hdr" ?? No.
  }
}

#endif
