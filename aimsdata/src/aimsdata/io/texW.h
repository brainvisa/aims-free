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

#ifndef AIMS_IO_TEXW_H
#define AIMS_IO_TEXW_H

#include <aims/config/aimsdata_config.h>
#include <aims/mesh/texture.h>
#include <aims/io/texheader.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>
#include <memory>


namespace aims
{

  /**	Tex format readers for texture objects.
	This reader can be used directly, or via the Reader / TexFormat 
	interface.
	End users should always use Reader which hides the format-specific 
	operations, TexReader is only a part of the mechanism
  */
  template<class T> class AIMSDATA_API TexWriter
  {
  public:
    TexWriter( const std::string & filename ) 
      : _name( filename ), _itemw( 0 ) {}
    ~TexWriter() {}

    void write( const TimeTexture<T> & thing, bool ascii );
    void setItemWriter( ItemWriter<T>* iw )
      { delete _itemw; _itemw = iw; }

  private:
    std::string		_name;
    ItemWriter<T>	*_itemw;
  };


  template<class T> AIMSDATA_API 
  inline 
  ItemWriter<T> & operator >> ( ItemWriter<T> & w, TimeTexture<T> & thing )
  {
    w.write( thing );
    return w;
  }


  template<class T>
  inline
  void TexWriter<T>::write( const TimeTexture<T> & thing, bool ascii )
  {
    TexHeader	hdr( _name, carto::DataTypeCode<T>().dataType(), thing.size(), 
		     ascii );

    hdr.copy( thing.header() );
    hdr.write();

    std::ios::openmode	omd = std::ios::app;
    if ( !ascii )
      omd |= std::ios::binary;
    std::ofstream	os( hdr.filename().c_str(), omd );
    if ( !os )
      throw carto::file_error( hdr.filename() );

    if ( !_itemw )
      _itemw = new DefaultItemWriter<T>;
    std::string	opmode = ascii ? "ascii" : "binar";
    ItemWriter<T>	*iw = _itemw->writer( opmode );
    DefaultItemWriter<uint32_t>	sw1;
    std::unique_ptr<ItemWriter<uint32_t> > sw( sw1.writer( opmode ) );

    typename TimeTexture<T>::const_iterator it, et = thing.end();

    for ( it=thing.begin(); it!=et; ++it )
      {
	sw->write( os, it->first );
	const Texture<T> & tex = it->second;
	uint32_t nitem = tex.nItem();
	sw->write( os, nitem );
	const std::vector<T> & vec = tex.data();
	iw->write( os, &vec[0], nitem );
      }

    delete iw;
  }

}


#endif
