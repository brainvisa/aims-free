/*
 *  Texture reader class
 */
#ifndef AIMS_IO_TEXR_H
#define AIMS_IO_TEXR_H

#include <aims/config/aimsdata_config.h>
#include <aims/mesh/texture.h>
#include <aims/io/texheader.h>
#include <aims/io/defaultItemR.h>
#include <cartobase/exception/ioexcept.h>
#include <memory>


namespace aims
{

  /**	Tex format readers for texture objects.
	This reader can be used directly, or via the Reader / TexFormat 
	interface.
	End users should always use Reader which hides the format-specific 
	operations, TexReader is only a part of the mechanism
  */
  template<class T> class AIMSDATA_API TexReader
  {
  public:
    TexReader( const std::string & filename ) 
      : _name( filename ), _itemr( 0 ) {}
    ~TexReader() {}

    void read( TimeTexture<T> & thing, int frame = -1 );
    void setItemReader( ItemReader<T>* ir )
      { delete _itemr; _itemr = ir; }

  private:
    std::string		_name;
    ItemReader<T>	*_itemr;
  };


  template<class T> AIMSDATA_API 
  inline 
  TexReader<T> & operator >> ( TexReader<T> & r, TimeTexture<T> & thing )
  {
    r.read( thing );
    return r;
  }


  template<class T>
  inline 
  void TexReader<T>::read( TimeTexture<T> & thing, int frame )
  {
    if ( frame >= 0 )
      std::cerr << "Warning : .tex single frame reading not implemented yet -\n" 
	        << "reading whole texture" << std::endl;

    TexHeader	hdr( _name );
    size_t	offset;

    hdr.read( &offset );

    std::ifstream	is( hdr.filename().c_str(), 
                            std::ios::in | std::ios::binary );
    if ( !is )
      carto::io_error::launchErrnoExcept( hdr.filename() );
    is.unsetf( std::ios::skipws );
    is.seekg( offset );
    if ( !is )
      carto::io_error::launchErrnoExcept( hdr.filename() );

    if ( !_itemr )
      _itemr = new DefaultItemReader<T>;
    ItemReader<T> *ir( _itemr->reader( hdr.openMode(), hdr.byteSwapping() ) );
    DefaultItemReader<uint32_t>	sr1;
    std::unique_ptr<ItemReader<uint32_t> > sr( sr1.reader( hdr.openMode(), hdr.byteSwapping() ) );

    int		nt;
    hdr.getProperty( "nb_t_pos", nt );
    
    thing.setHeader( hdr );

    for ( int t=0; t<nt; ++t )
      {
        uint32_t time, nitem;
        sr->read( is, time );
        sr->read( is, nitem );
        //cout << "time " << time << ", nitem : " << nitem << endl;
        std::vector<T>	& vec = thing[ time ].data();
        vec.clear();
        vec.insert( vec.end(), nitem, T() );
        ir->read( is, &vec[0], nitem );
      }
    
    delete ir;
  }

}


#endif
