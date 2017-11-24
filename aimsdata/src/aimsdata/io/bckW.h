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
