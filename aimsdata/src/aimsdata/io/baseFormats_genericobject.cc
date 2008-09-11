/* Copyright (c) 2007-2008 CEA
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

#include <aims/io/baseFormats_genericobject.h>
#include <cartobase/wip/io/reader.h>
#include <cartobase/object/pythonwriter.h>


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
                             bool )
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



