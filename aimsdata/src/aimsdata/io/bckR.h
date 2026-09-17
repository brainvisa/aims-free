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
#include <soma-io/utilities/asciidatasourcetraits.h>


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
    std::unique_ptr< ItemReader<AimsBucketItem<T> > > ir( _itemr->reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER ) );
    DefaultItemReader<uint32_t>	sr1;
    std::unique_ptr< ItemReader<uint32_t> > sr( sr1.reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER ) );

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
