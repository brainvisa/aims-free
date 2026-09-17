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
      // use a view in LPI orientation
      carto::VolumeRef<T> vol2( ptr, std::vector<int>( 4, 0 ), vol.getSize() );
      // vol2->flipToOrientation( "LPI" );

      AimsData<T>			d( vol2 );
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

  // ----

  template<typename T>
  VolumeRefAimsFormat<T>::VolumeRefAimsFormat( const std::string & prefformat )
    : FileFormat<carto::VolumeRef<T> >(), _preferredFormat( prefformat )
  {
  }


  template<typename T>
  VolumeRefAimsFormat<T>::~VolumeRefAimsFormat()
  {
  }


  template<typename T>
  bool VolumeRefAimsFormat<T>::read( const std::string & filename,
                                     carto::VolumeRef<T> & vol,
                                     const carto::AllocatorContext & context,
                                     carto::Object options )
  {
    Reader<carto::Volume<T> >	r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    vol.reset( r.read() );
    return true;
  }


  template<typename T>
  carto::VolumeRef<T>*
  VolumeRefAimsFormat<T>::read( const std::string & filename,
                                const carto::AllocatorContext & context,
                                carto::Object options )
  {
    Reader<carto::Volume<T> >	r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    carto::VolumeRef<T> *vol = new carto::VolumeRef( r.read() );
    return vol;
  }


  template<typename T>
  bool VolumeRefAimsFormat<T>::write( const std::string & filename,
                                      const carto::VolumeRef<T> & vol,
                                      carto::Object options )
  {
    Writer<carto::Volume<T> >	w( filename, options );
    return w.write( *vol );
  }

}

#endif

