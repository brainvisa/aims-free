/*  Support of MINC file format (from the Montreal Neurological Institute)
 *
 *  Olivier Colliot, MNI, Montreal
 *  May 2004
 * (c) MNI, CEA 2004-2005
 */

#ifndef AIMS_IO_MINCR_H
#define AIMS_IO_MINCR_H

//MINC I/O support
extern "C" {
/* redefine some stupid macros that conflict with 
   those of the stupid system on Windows
*/
#define BOOLEAN MINC_BOOLEAN
#define FLOAT MINC_FLOAT
#define DOUBLE MINC_DOUBLE
#define SHORT MINC_SHORT
#include <volume_io.h>
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
#include <cartobase/allocator/mappingcopy.h>
#include <cartobase/allocator/mappingro.h>
#include <cartobase/allocator/mappingrw.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/datasource/filedatasource.h>

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
    std::string		_name;
    std::string         _read_mode;
    ItemReader<T>	*_itemr;
  };


  template <class T>
  inline AIMSDATA_API MincReader<T> & 
  operator >> ( MincReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing );
    return reader;
  }


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

    //std::cout << "alloc data\n";
    //std::cout << hdr->dimX() << " " << hdr->dimY() << " " << hdr->dimZ() << " " <<dimt <<"\n";

    std::string name = hdr->removeExtension( _name ) + ".mnc";

    carto::AllocatorContext	al 
      ( context.accessMode(), 
        carto::rc_ptr<carto::DataSource>
        ( new carto::FileDataSource( name, 0, carto::DataSource::Read ) ), 
        false, context.useFactor() );

    AimsData<T> data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), dimt, 
		      border, al );

    al = data.allocator();
    //std::cout << "Alloc OK\n";
    data.setSizeX( hdr->sizeX() );
    data.setSizeY( hdr->sizeY() );
    data.setSizeZ( hdr->sizeZ() );
    data.setSizeT( hdr->sizeT() );
    data.setHeader( hdr );

    if( al.allocatorType() != carto::AllocatorStrategy::ReadOnlyMap 
        && al.allocatorType() != carto::AllocatorStrategy::ReadWriteMap )
      {
	Volume volume;
	STRING dim_names[4];
	STRING fileName = create_string ( (char*)_name.c_str());



	dim_names[0] = create_string( const_cast<char*>( MIzspace ) );
	dim_names[1] = create_string( const_cast<char*>( MIyspace ) );
	dim_names[2] = create_string( const_cast<char*>( MIxspace ) );
	dim_names[3] = create_string( const_cast<char*>( MItime ) );
	set_print_error_function(MincHeader::my_empty_print_error);

	/*	int n_dimensions=4;

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
	if( input_volume( fileName, 0, dim_names,
			  MI_ORIGINAL_TYPE, TRUE,
			  0.0, 0.0, TRUE, &volume,
			  (minc_input_options *) NULL ) != OK )
	  return;


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

	carto::DataTypeCode<T>	dtc;
	//std::cout << "Output type: " << dtc.dataType() << "\n";

	//Read the volume according to the output data type (T, which can be U8, S8, S16, FLOAT...) and to the read mode which can be "real" or "voxel"
	//In every case, when the voxel size is negative the values are read "as usual" and when the voxel size is positive, values are read using a "mirror symmetry". (this is quite odd, I don't know why it is not the other way round.
	//Use the read mode "real" (i.e. read real values using the function "get_volume_real_value"
	if(_read_mode=="real") {
	  //If the output type is integer, "round" the values to the nearest integer (mostly necessary for label volumes)
	  if ( (dtc.dataType()=="U8") || (dtc.dataType()=="S8")  || (dtc.dataType()=="U16") || (dtc.dataType()=="S16") || (dtc.dataType()=="U32") || (dtc.dataType()=="S32")) {
	    for( int t=0; t<dimt; ++t )
	      for( int z=0; z<data.dimZ(); ++z )  
		for( int y=0; y<data.dimY(); ++y )  
		  for( int x=0; x<data.dimX(); ++x ) {
		    //data(x,y,z,t)=(T)rint(get_volume_real_value( volume, z, y, x, t, 0 ));
		    data(x,y,z,t)=(T)rint(get_volume_real_value( volume, (Z_pos)*data.dimZ()-dirZ*z-(Z_pos), (Y_pos)*data.dimY()-dirY*y-(Y_pos), (X_pos)*data.dimX()-dirX*x-(X_pos) , t, 0 ));
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
	else {
	  for( int t=0; t<dimt; ++t )
	    for( int z=0; z<data.dimZ(); ++z )  
	      for( int y=0; y<data.dimY(); ++y )  
		for( int x=0; x<data.dimX(); ++x ) {
		  //data(x,y,z,t)=(T)get_volume_voxel_value( volume, z, y, x, t, 0 );
		  data(x,y,z,t)=(T)get_volume_voxel_value( volume, (Z_pos)*data.dimZ()-dirZ*z-(Z_pos), (Y_pos)*data.dimY()-dirY*y-(Y_pos), (X_pos)*data.dimX()-dirX*x-(X_pos) , t, 0 );
		}
	}
	delete_volume(volume);
	delete_string(fileName);
	delete_string(dim_names[0]);
	delete_string(dim_names[1]);
	delete_string(dim_names[2]);
	delete_string(dim_names[3]);
	
      }

    thing = data;

    //Should we delete the header "hdr" ??
  }
}

#endif
