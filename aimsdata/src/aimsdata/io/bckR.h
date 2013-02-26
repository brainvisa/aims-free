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
 *  Bucket reader class
 */
#ifndef AIMS_IO_BCKR_H
#define AIMS_IO_BCKR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/bckItemR.h>
#include <aims/bucket/bucket.h>
#include <aims/io/bckheader.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/io/asciidatasourcetraits.h>


namespace aims
{

  template<class T> class AIMSDATA_API BckReader
  {
  public:
    BckReader( const std::string& name ) : _name(name), _itemr( 0 ) {}
    ~BckReader() { delete _itemr; }

    void read( AimsBucket<T> & thing, int frame = -1 );
    void setItemReader( ItemReader<T>* ir )
      { delete _itemr; _itemr = ir; }

  private:
    std::string				_name;
    ItemReader<AimsBucketItem<T> >	*_itemr;
  };


  template<class T> inline 
  void BckReader<T>::read( AimsBucket<T> & thing, int frame )
  {
    if ( frame >= 0 )
      std::cerr 
	<< "Warning : .bck single frame reading not implemented yet -\n" 
	<< "reading whole bucket\n";

    BckHeader	hdr( _name );
    uint32_t	buf;
    hdr.read( &buf );
    std::ifstream::off_type offset = buf; // ## does not support large files!

    std::string		fname = hdr.filename();
    bool ascii = ( hdr.openMode() == "ascii" );
    std::ifstream	is( fname.c_str(), std::ios::in | std::ios::binary );
    if ( !is )
      carto::io_error::launchErrnoExcept( fname );
    is.unsetf( std::ios::skipws );
    is.seekg( offset );
    if ( !is )
      carto::io_error::launchErrnoExcept( fname );

    if ( !ascii )
      is.unsetf( std::ios::skipws );
    if ( !_itemr )
      _itemr = new DefaultItemReader<AimsBucketItem<T> >;
    std::auto_ptr< ItemReader<AimsBucketItem<T> > > ir( _itemr->reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER ) );
    DefaultItemReader<uint32_t>	sr1;
    std::auto_ptr< ItemReader<uint32_t> > sr( sr1.reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER ) );

    thing.clear();
    thing.setHeader( hdr );

    int size = hdr.dimT();
    uint32_t time=0, nitem=0;
    std::string tmp;

    for ( int t=0; t<size; ++t )
      {
	if ( ascii )
	  {
            carto::StreamUtil::skip( is );
	    is >> tmp;
	    if ( tmp != "-time" )
	      throw carto::invalid_format_error( fname );
	  }
	sr->read( is, time );
	if ( ascii )
	  {
            carto::StreamUtil::skip( is );
	    is >> tmp;
	    if ( tmp != "-dim" )
	      throw carto::invalid_format_error( fname );
	  }
	sr->read( is, nitem );
	if ( !is )
	  carto::io_error::launchErrnoExcept( fname );
	std::list<AimsBucketItem<T> >	& items = thing[ time ];
	AimsBucketItem<T> 		item;

	for ( uint32_t n=0; n<nitem; ++n )
	  {
	    ir->read( is, item );
	    items.push_back(item);
	  }
	if ( !is )
	  carto::io_error::launchErrnoExcept( fname );
      }
  }

}


template<class T> inline 
aims::BckReader<T> & operator >> ( aims::BckReader<T> & reader, 
                                   AimsBucket<T> & thing )
{
  reader.read( thing );
  return reader;
}


#endif
