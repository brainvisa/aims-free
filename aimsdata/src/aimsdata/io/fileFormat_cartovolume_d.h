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
 *  Data reader class
 */
#ifndef AIMS_IO_FILEFORMAT_CARTOVOLUME_D_H
#define AIMS_IO_FILEFORMAT_CARTOVOLUME_D_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/header.h>
#include <aims/io/baseFormats_cartovolume.h>
#include <aims/io/baseFormats_volume.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

namespace aims
{

  template<typename T>
  VolumeFormat<T>::VolumeFormat( const std::string & prefformat )
    : FileFormat<carto::Volume<T> >(), _preferredFormat( prefformat )
  {
  }


  template<typename T>
  VolumeFormat<T>::~VolumeFormat()
  {
  }


  template<typename T>
  bool VolumeFormat<T>::read( const std::string & filename, 
                              carto::Volume<T> & vol, 
                              const carto::AllocatorContext & context, 
                              carto::Object options )
  {
    /*
    std::cout << "VolumeFormat<" << carto::DataTypeCode<T>::name() 
              << ">::read(" << filename << ")\n";
    */
    Reader<AimsData<T> >		r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    carto::rc_ptr<carto::Volume<T> >	ptr( &vol );
    int	border = 0;
    options->getProperty( "border", border );
    {
      AimsData<T>			d( ptr );
      std::string			*fmt = 0;
      if( !_preferredFormat.empty() )
        fmt = &_preferredFormat;
      r.read( d, border, fmt );
      // Up to now I don't know how to avoid a full copy
      vol = *d.volume(); // #####
    }
    ptr.release(); // don't destroy vol !
    return true;
  }


  template<typename T>
  carto::Volume<T>* 
  VolumeFormat<T>::read( const std::string & filename, 
                         const carto::AllocatorContext & context, 
                         carto::Object options )
  {
    /*
    std::cout << "VolumeFormat<" << carto::DataTypeCode<T>::name() 
              << ">::read*(" << filename << ")\n";
    */
    Reader<AimsData<T> >		r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    carto::rc_ptr<carto::Volume<T> >	ptr;
    int	border = 0;
    options->getProperty( "border", border );
    {
      AimsData<T>			d;
      std::string			*fmt = 0;
      if( !_preferredFormat.empty() )
        fmt = &_preferredFormat;
      if( !r.read( d, border, fmt ) )
        return 0;
      ptr = d.volume();
    }
    carto::Volume<T>	*vol = ptr.release(); // don't destroy vol !
    return vol;
  }


  template<typename T>
  bool VolumeFormat<T>::write( const std::string & filename, 
                               const carto::Volume<T> & vol,
                               carto::Object options )
  {
    Writer<AimsData<T> >		w( filename, options );
    carto::rc_ptr<carto::Volume<T> > 
      ptr( const_cast<carto::Volume<T> *>( &vol ) );
    {
      AimsData<T>			d( ptr );
      std::string			*fmt = 0;
      if( !_preferredFormat.empty() )
        fmt = &_preferredFormat;
      bool ascii = false;
      try
      {
        if( !options.isNull() )
        {
          carto::Object aso = options->getProperty( "ascii" );
          if( !aso.isNull() )
            ascii = (bool) aso->getScalar();
        }
      }
      catch( ... )
      {
      }
      w.write( d, ascii, fmt );
    }
    ptr.release(); // don't destroy vol !
    return true;
  }


  /*
  template<typename T>
  void FileFormatDictionary<carto::Volume<T> >::registerBaseFormats()
  {
    std::set<std::string> f = FileFormatDictionary<AimsData<T> >::formats();
    std::set<std::string>::iterator	i, e = f.end();
    const std::multimap<std::string, std::string> 
      & ext = FileFormatDictionary<AimsData<T> >::extensions();
    std::multimap<std::string, std::string>::const_iterator ie, ee;

    for( i=f.begin(); i!=e; ++i )
      {
        std::vector<std::string>	ext2;
        for( ie=ext.lower_bound( *i ), ee=ext.upper_bound( *i ); ie!=ee; 
             ++ie )
          ext2.push_back( ie->second );
        VolumeFormat<T>	*vf = new VolumeFormat<T>;
        registerFormat( *i, vf, ext2 );
      }
  }
  */

  // -----------

  template<typename T>
  VolumeRefFormat<T>::VolumeRefFormat( const std::string & prefformat )
    : FileFormat<carto::VolumeRef<T> >(), _volformat( prefformat )
  {
  }


  template<typename T>
  VolumeRefFormat<T>::~VolumeRefFormat()
  {
  }


  template<typename T>
  bool VolumeRefFormat<T>::read( const std::string & filename, 
                                 carto::VolumeRef<T> & vol, 
                                 const carto::AllocatorContext & context, 
                                 carto::Object options )
  {
    carto::Volume<T> *vref = _volformat.read( filename, context, options );
    if( vref )
    {
      vol.reset( vref );
      return true;
    }
    return false;
  }


  template<typename T>
  carto::VolumeRef<T>* 
  VolumeRefFormat<T>::read( const std::string & filename, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    carto::Volume<T>	*vol = _volformat.read( filename, context, options );
    if( vol )
      return new carto::VolumeRef<T>( vol );
    return 0;
  }


  template<typename T>
  bool VolumeRefFormat<T>::write( const std::string & filename, 
                                  const carto::VolumeRef<T> & vol,
                                  carto::Object options )
  {
    return _volformat.write( filename, *vol, options );
  }

}

#endif

