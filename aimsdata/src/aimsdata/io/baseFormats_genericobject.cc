// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/baseFormats_genericobject.h>
#include <soma-io/io/reader.h>
#include <soma-io/writer/pythonwriter.h>

namespace aims
{

  template <typename T>
  MinfFormat<T>::~MinfFormat()
  {
  }


  template <>
  bool MinfFormat<carto::GenericObject>::read(
      const std::string & filename, carto::GenericObject & obj,
      const carto::AllocatorContext & context, carto::Object options )
  {
    carto::Reader<carto::GenericObject> r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    return r.read( obj );
  }


  template <>
  carto::GenericObject *MinfFormat<carto::GenericObject>::read(
      const std::string & filename, const carto::AllocatorContext & context,
      carto::Object options )
  {
    carto::Reader<carto::GenericObject> r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    return r.read();
  }


  template <typename T>
  bool MinfFormat<T>::write( const std::string & filename, const T & obj,
                             carto::Object )
  {
    carto::PythonWriter  pw( filename );
    pw.write( obj );
    return true;
  }


  template <>
  bool MinfFormat<carto::Object>::read(
      const std::string & filename, carto::Object & obj,
      const carto::AllocatorContext & context, carto::Object options )
  {
    carto::Reader<carto::GenericObject> r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    carto::GenericObject *go = r.read();
    if( !go )
      return false;
    obj.reset( go );
    return true;
  }


  template <>
  carto::Object *MinfFormat<carto::Object>::read(
      const std::string & filename, const carto::AllocatorContext & context,
      carto::Object options )
  {
    carto::Reader<carto::GenericObject> r( filename );
    r.setAllocatorContext( context );
    r.setOptions( options );
    return new carto::Object( r.read() );
  }


  /*
  template <typename T>
  MinfXMLFormat<T>::~MinfXMLFormat()
  {
  }


  template <typename T>
  bool MinfFormat<T>::write( const std::string & filename, const T & obj,
                             bool )
  {
    MinfXMLWriter  pw( filename );
    pw.write( obj );
  }
  */


  template class MinfFormat<carto::GenericObject>;
  template class MinfFormat<carto::Object>;
}



