/*
 *  Bucket reader class
 */
#ifndef AIMS_IO_BCKMAPR_H
#define AIMS_IO_BCKMAPR_H

#include <aims/config/aimsdata_config.h>
#include <aims/bucket/bucketMap.h>
#include <aims/io/bckheader.h>
#include <aims/io/defaultItemR.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/utilities/asciidatasourcetraits.h>


namespace aims
{

  /**	BucketMap objects can be read / written by BckMapReader / BckMapWriter,
	these IO classes operate on the EXACT SAME FORMAT as the BCK format 
	used by AimsBucket objects.
  */
  template<class T> class BckMapReader
  {
  public:
    BckMapReader( const std::string& name ) : _name(name), _itemr( 0 ) {}
    ~BckMapReader() { delete _itemr; }

    void read( BucketMap<T> & thing, int frame = -1 );
    void setItemReader( ItemReader<T>* ir )
      { delete _itemr; _itemr = ir; }

  private:
    std::string		_name;
    ItemReader<T>	*_itemr;
  };


  template<class T> inline 
  void BckMapReader<T>::read( BucketMap<T> & thing, int frame )
  {
    if( frame >= 0 )
      std::cerr 
	<< "Warning : .bck single frame reading not implemented yet -\n" 
	<< "reading whole bucket\n";

    BckHeader hdr( _name );
    uint32_t buf;
    hdr.read( &buf );
    std::ifstream::off_type offset = buf; // ## does not support large files!
 
    std::string		fname = hdr.filename();
    bool ascii = ( hdr.openMode() == "ascii" );
    std::ifstream	is( fname.c_str(), std::ios::in | std::ios::binary );
    if( !is )
      carto::io_error::launchErrnoExcept( fname );
    is.unsetf( std::ios::skipws );
    is.seekg( offset );
    if( !is )
      carto::io_error::launchErrnoExcept( fname );

    if( !_itemr )
      _itemr = new DefaultItemReader<T>;
    ItemReader<T> *ir 
      = _itemr->reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER );
    DefaultItemReader<uint32_t>	sr1;
    ItemReader<uint32_t> *sr 
      = sr1.reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER );
    DefaultItemReader<Point3d>	posr1;
    ItemReader<Point3d>		*posr 
      = posr1.reader( hdr.openMode(), hdr.byteOrder() != AIMS_MAGIC_NUMBER );

    thing.clear();
    thing.setHeader( hdr );

    uint32_t time=0, nitem=0;
    int size = hdr.dimT();
    std::string	tmp;

    for( int t=0; t<size; ++t )
      {
	if( ascii )
	  {
            carto::StreamUtil::skip( is );
	    is >> tmp;
	    if( tmp != "-time" )
	      {
		delete sr;
		delete ir;
		delete posr;
		throw carto::invalid_format_error( fname );
	      }
	  }
	sr->read( is, time );
	if( ascii )
	  {
            carto::StreamUtil::skip( is );
	    is >> tmp;
	    if( tmp != "-dim" )
	      {
		delete sr;
		delete ir;
		delete posr;
		throw carto::invalid_format_error( fname );
	      }
	  }
	sr->read( is, nitem );
	if( !is )
	  {
	    delete sr;
	    delete ir;
	    delete posr;
	    carto::io_error::launchErrnoExcept( fname );
	  }
	typename BucketMap<T>::Bucket	& items = thing[ time ];
	Point3d			pos;
	T			item;

        //std::cout << "time: " << time << ", nitem: " << nitem << "\n";

	for( uint32_t n=0; n<nitem; ++n )
	  {
	    posr->read( is, pos );
	    ir->read( is, item );
	    items[ pos ] = item;
	  }
	if( !is )
	  {
	    delete sr;
	    delete ir;
	    delete posr;
	    carto::io_error::launchErrnoExcept( fname );
	  }
      }

    delete sr;
    delete ir;
    delete posr;
  }

}


template<class T> inline 
aims::BckMapReader<T> & operator >> ( aims::BckMapReader<T> & reader, 
				      aims::BucketMap<T> & thing )
{
  reader.read( thing );
  return reader;
}


#endif
