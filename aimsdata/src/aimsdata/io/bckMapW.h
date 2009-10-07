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

#ifndef AIMS_IO_BCKMAPW_H
#define AIMS_IO_BCKMAPW_H

#include <aims/config/aimsdata_config.h>
#include <aims/bucket/bucketMap.h>
#include <aims/io/bckheader.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/ioexcept.h>


namespace aims
{

  /**	BucketMap objects can be read / written by BckMapReader / BckMapWriter,
	these IO classes operate on the EXACT SAME FORMAT as the BCK format 
	used by AimsBucket objects.
  */
  template<class T> class BckMapWriter
  {
  public:
    BckMapWriter( const std::string& name, bool ascii = false )
      : _name(name), _itemw( 0 ), _binmode( !ascii ) {}
    ~BckMapWriter() { delete _itemw; }

    void write( const BucketMap<T> & thing );
    void setMode( bool ascii );
    bool asciiMode() const { return( !_binmode ); }
    void setItemWriter( ItemWriter<T>* iw )
      { delete _itemw; _itemw = iw; }

  private:
    std::string		_name;
    ItemWriter<T>	*_itemw;
    bool		_binmode;
  };


  template <class T>
  inline
  void BckMapWriter<T>::setMode( bool ascii )
  {
    _binmode = !ascii;
  }


  template<class T> inline 
  void BckMapWriter<T>::write( const BucketMap<T> & thing )
  {
    std::string	openmode = ( _binmode ? "binar" : "ascii" );
    carto::DataTypeCode<T>	dtc;
    BckHeader		hdr( _name, dtc.name(), thing.sizeX(), 
			     thing.sizeY(), thing.sizeZ(), thing.sizeT(), 
			     AIMS_MAGIC_NUMBER, openmode );
    hdr.copy( thing.header() );
    hdr.write();

    std::string 	name = hdr.filename();
    std::ios::openmode	omd = std::ios::app;
    if( _binmode )
      omd |= std::ios::binary;
    std::ofstream	os( name.c_str(), omd );
    if( !os )
      carto::io_error::launchErrnoExcept( name );

    if( !_itemw )
      _itemw = new DefaultItemWriter<T>;
    ItemWriter<T>		*iw = _itemw->writer( openmode, false );
    DefaultItemWriter<Point3d>	itemw1;
    ItemWriter<Point3d>		*posw  = itemw1.writer( openmode, false );
    DefaultItemWriter<uint32_t>	sw;
    typename BucketMap<T>::const_iterator		ib, eb = thing.end();
    typename BucketMap<T>::Bucket::const_iterator	ibi, ebi;

    if( _binmode )
      sw.write( os, thing.size() );
    else
      os << "-dimt " << thing.size() << std::endl;;

    for( ib=thing.begin(); ib!=eb; ++ib )
      {
	if( _binmode )
	  {
	    sw.write( os, ib->first );
	    sw.write( os, ib->second.size() );
	  }
	else
	  {
	    os << "-time " << ib->first << std::endl;
	    os << "-dim " << ib->second.size() << std::endl;
	  }
	for( ibi=ib->second.begin(), ebi=ib->second.end(); ibi!=ebi; ++ibi )
	  {
	    posw->write( os, ibi->first );
	    iw->write( os, ibi->second );
	    if( !_binmode )
	      os << std::endl;
	  }
      }

    delete iw;
    delete posw;
  }

}


template<class T> inline 
aims::BckMapWriter<T> & operator << ( aims::BckMapWriter<T> & writer, 
				      aims::BucketMap<T> & thing )
{
  writer.write( thing );
  return( writer );
}


#endif


