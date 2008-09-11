/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Data writer class
 */
#ifndef AIMS_IO_GIDW_H
#define AIMS_IO_GISW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/gisheader.h>
#include <aims/data/data.h>
#include <cartobase/exception/file.h>
#include <aims/io/datatypecode.h>
#include <string>


namespace aims
{

  template<class T>
  class AIMSDATA_API GisWriter
  {
  public:
    GisWriter( const std::string& name, bool ascii = false, 
	       ItemWriter<T>* ir = 0 ) 
      : _name( name ), _itemw( ir ), _binmode( !ascii ) {}
    ~GisWriter() { delete _itemw; }

    bool write( const AimsData<T>& thing );
    /// Return a name without .dim or .ima extension
    std::string removeExtension( const std::string& name );
    /// set input file mode
    void setMode( bool ascii );
    bool asciiMode() const { return( !_binmode ); }
    void setItemWriter( ItemWriter<T>* ir )
      { delete _itemw; _itemw = ir; }
    std::string name() { return( _name ); }

  private:
    std::string		_name;
    ItemWriter<T>	*_itemw;
    bool		_binmode;
  };


  template <class T>
  inline AIMSDATA_API GisWriter<T> & 
  operator << ( GisWriter<T> & writer, const AimsData<T> & thing )
  {
    writer.write( thing );
    return writer;
  }

  template <class T>
  inline
  void GisWriter<T>::setMode( bool ascii )
  {
    _binmode = !ascii;
  }


  template <class T>
  inline 
  std::string GisWriter<T>::removeExtension( const std::string& name )
  {
    std::string res = name;
    std::string ext="";
    if ( res.length() > 4 )
      ext = res.substr( int(res.length() - 4), 4 );
    if (ext == ".dim" || ext == ".ima")
      res = res.substr( 0, res.length() - 4 );
    return res;
  }



  template <class T>
  inline
  bool GisWriter<T>::write( const AimsData<T>& thing )
  {
    std::string	openmode = ( _binmode ? "binar" : "ascii" );
    carto::DataTypeCode<T>	dt;
    GisHeader	hdr( _name, dt.dataType(), 
		     thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(),
		     thing.sizeX(), thing.sizeY(), thing.sizeZ(), 
		     thing.sizeT(), AIMS_MAGIC_NUMBER, openmode );
    // copy existing attributes from existing header (if any)
    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    if( ph )
      hdr.copy( *ph );
    hdr.write();

    std::string 	name = removeExtension( _name ) + ".ima";
    std::ios::openmode	omd = std::ios::out;
    if( _binmode )
      omd |= std::ios::binary;
    std::ofstream	os( name.c_str(), omd );
    if( !os )
      carto::io_error::launchErrnoExcept( name );
    if( _binmode )
      os.unsetf( std::ios::skipws );

    if( !_itemw )
      _itemw = new DefaultItemWriter<T>;
    ItemWriter<T>	*ir = _itemw->writer( asciiMode() ? "ascii" : "binar", 
					      false );

    for( int t=0; t<thing.dimT(); ++t) 
      for( int z=0; z<thing.dimZ(); ++z )  
	for( int y=0; y<thing.dimY(); ++y )  
	  ir->write( os, &thing(0,y,z,t), thing.dimX() );

    delete ir;
    return( true );
  }


}

#endif
