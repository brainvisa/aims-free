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
 *  FDF data reader class
 */
#ifndef AIMS_IO_FDFR_H
#define AIMS_IO_FDFR_H

#include <aims/io/defaultItemR.h>
#include <aims/data/data.h>
#include <aims/io/fdfheader.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/type/byte_order.h>
#include <soma-io/datasource/filedatasource.h>
#include <stdio.h>

namespace aims
{
  template< class T > class FdfReader;

  template< class T > AIMSDATA_API FdfReader< T >&
    operator >> ( FdfReader< T >&, AimsData< T >& );

  template< class T >
    class FdfReader
    {
    public:

      FdfReader( const std::string& name )
        : _name( name ) { }
      virtual ~FdfReader() { }

      void read( AimsData< T >&, const carto::AllocatorContext & context, 
                 carto::Object options );

    private:

      std::string	_name;

      void readFrame( AimsData< T >& thing, const std::string & file,
                      uint slice, const aims::FdfHeader *hdr );
    };



  template< class T > inline
    FdfReader< T >&
    operator >> ( FdfReader< T >& reader, AimsData< T >& thing )
    {
      reader.read( thing, thing.allocator(), 
                   carto::Object::value( carto::PropertySet() ) );
      return reader;
    }

  template< class T > inline
    void FdfReader< T >::read( AimsData< T >& thing,
                                 const carto::AllocatorContext & context, 
                                 carto::Object options )
    {
      int borderWidth = 0;
      options->getProperty( "border", borderWidth );

      // Load the header of the first file
      std::auto_ptr<aims::FdfHeader> hdr
        = std::auto_ptr<aims::FdfHeader>( new aims::FdfHeader( _name ) );
      hdr->read();

      int rank = (int)hdr->getProperty( "rank" )->getScalar();

      // check if data type is compatible with type T
      int bits_allocated = (int)hdr->getProperty( "bits_allocated" )->getScalar();
      if ( ( bits_allocated / 8 ) > (int)sizeof( T ) )
      {
        throw carto::format_error( _name );
      }

      std::string		dir = carto::FileUtil::dirname( _name );
      std::vector<std::string>	files;

      if ( rank == 2 ) {
        files = hdr->inputFilenames();
      }
      else {
        files.push_back( carto::FileUtil::basename( _name ) );
      }

      // create an AimsData
      carto::AllocatorContext
        cont( context.accessMode(),
               carto::rc_ptr<carto::DataSource>
               ( new carto::FileDataSource
                 ( _name, 0, carto::DataSource::Read ) ), false,
               context.useFactor() );

      AimsData< T > data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), hdr->dimT(),
                          borderWidth, cont );
      data.setSizeXYZT( hdr->sizeX(), hdr->sizeY(), hdr->sizeZ(), hdr->sizeT() );

      // Read frames
      for( uint slice=0; slice < files.size(); slice++ ) {
          if ( ! carto::FileUtil::fileStat( dir + carto::FileUtil::separator() + files[slice] ).empty() ) {
            readFrame( data, dir + carto::FileUtil::separator() + files[slice], slice, hdr.get() );
          }
      }

      thing = data;
      thing.setHeader( hdr.release() );
    }

  template< class T > inline
    void FdfReader< T >::readFrame( AimsData< T >& thing,
                                    const std::string & filename, uint slice,
                                    const aims::FdfHeader *hdr )
    {
      std::vector< int > dims;
      hdr->getProperty( "volume_dimension", dims );
      int bits_allocated = (int)hdr->getProperty( "bits_allocated" )->getScalar();
      int byte_swapping = (int)hdr->getProperty( "byte_swapping" )->getScalar();
      int rank = (int)hdr->getProperty( "rank" )->getScalar();
//       uint byte_order;
//       if (byte_swapping) {
//         byte_order = stringToByteOrder( "DCBA" );
//       }
//       else {
//         byte_order = stringToByteOrder( "ABCD" );
//       }

      uint pixels_number = 1;

      for( int dim = 0; dim < rank; dim++ )
      {
        pixels_number *= dims[ dim ];
      }

      std::ifstream inFile(filename.c_str(), std::ios::in | std::ios::binary);

      // Check if there was an error opening the file
      if (!inFile)
      {
        throw carto::open_error( "No image data found in file.", filename );
      }

      // Get data position
      int zstart = 0, zend = thing.dimZ();
      inFile.seekg (0, std::ios::end);
      long int fileSize = inFile.tellg();
      size_t dataPosition = fileSize - (pixels_number * ( bits_allocated / 8 ) );
      inFile.seekg( dataPosition );

      DefaultItemReader<T> itemr;
      ItemReader<T> *ir
        = itemr.reader( "binar",
                         byte_swapping );

      if ((rank == 2) && (dims.size() > 2) && (dims[2] > 1)) {
         zstart = slice;
         zend = slice + 1;
      }

      for( int t=0; t < thing.dimT(); ++t )
        for( int z=zstart; z < zend; ++z ) {
          for( int y=0; y < thing.dimY(); ++y )
          {
            ir->read( inFile, &thing(0, y, z, t), thing.dimX() );
            if( !inFile || inFile.eof() ) {
              carto::io_error::launchErrnoExcept( filename );
            }
          }
        }

      bool success = !inFile.bad();
      inFile.close();

      if (!success)
      {
        throw carto::open_error( "Error reading data.", filename );
      }
    }
}

#endif
