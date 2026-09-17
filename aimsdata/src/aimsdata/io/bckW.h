#ifndef AIMS_IO_BCKW_H
#define AIMS_IO_BCKW_H

#include <aims/io/bckItemW.h>
#include <aims/bucket/bucket.h>
#include <aims/io/bckheader.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/ioexcept.h>
#include <fstream>


namespace aims
{

  template<class T> class BckWriter
  {
  public:
    BckWriter( const std::string& name, bool ascii = false ) 
      : _name( name ), _itemw( 0 ), _ascii( ascii ) {}
    ~BckWriter() { delete _itemw; }

    void write( const AimsBucket<T> & thing );
    void setItemWriter( ItemWriter<T>* iw )
      { delete _itemw; _itemw = iw; }

  private:
    std::string				_name;
    ItemWriter<AimsBucketItem<T> >	*_itemw;
    bool				_ascii;
  };


  template<class T> inline 
  void BckWriter<T>::write( const AimsBucket<T> & thing )
  {
    std::string	openmode = ( _ascii ? "ascii" : "binar" );
    carto::DataTypeCode<T>	dt;
    BckHeader	hdr( _name, dt.name(), thing.sizeX(), thing.sizeY(), 
		     thing.sizeZ(), thing.sizeT(), AIMS_MAGIC_NUMBER, 
		     openmode );
    hdr.copy( thing.header() );
    hdr.write();

    std::ios::openmode omd = std::ios::app;
    if ( !_ascii )
      omd |= std::ios::binary;
    std::ofstream os( hdr.filename().c_str(), omd );
    if ( !os )
      carto::io_error::launchErrnoExcept( _name );

    if ( !_itemw )
      _itemw = new DefaultItemWriter<AimsBucketItem<T> >;
    std::unique_ptr< ItemWriter<AimsBucketItem<T> > > iw( _itemw->writer( openmode, false ) );
    DefaultItemWriter<uint32_t>	sw;

    typename AimsBucket<T>::const_iterator	ib, eb = thing.end();
    typename std::list<AimsBucketItem<T> >::const_iterator	ibi, ebi;

    if ( _ascii )
      os << "-dimt " << thing.size() << std::endl;
    else
      sw.write( os, (uint32_t)thing.size() );

    for ( ib=thing.begin(); ib!=eb; ++ib )
      {
	typename std::list< AimsBucketItem<T> >::size_type n = ib->second.size();
	if ( _ascii )
	  {
	    os << "-time " << ib->first << std::endl;
	    os << "-dim " << n << std::endl;
	  }
	else
	  {
	    sw.write( os, (uint32_t)ib->first );
	    sw.write( os, (uint32_t)n );
	  }

	const std::list<AimsBucketItem<T> >	& items = ib->second;
	for ( ibi=items.begin(), ebi=items.end(); ibi!=ebi; ++ibi )
	  iw->write( os, *ibi );
      }
  }

}


template<class T> inline 
aims::BckWriter<T> & operator << ( aims::BckWriter<T> & writer, 
				   const AimsBucket<T> & thing )
{
  writer.write( thing );
  return writer;
}


#endif
