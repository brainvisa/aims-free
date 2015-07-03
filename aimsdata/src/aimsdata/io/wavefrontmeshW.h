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

#ifndef AIMS_IO_WAVEFRONTMESHW_H
#define AIMS_IO_WAVEFRONTMESHW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/data/pheader.h>
#include <aims/mesh/surface.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>


namespace aims
{

  template<int D, class T>
  class WavefrontMeshWriter
  {
  public:
    WavefrontMeshWriter( const std::string & name )
      : _name( name ) { }
    ~WavefrontMeshWriter() {}

    inline void write( const AimsTimeSurface<D,T> & thing );

    /// Return a name without .obj extension
    inline static std::string removeExtension( const std::string& name );

  private:
    inline static PythonHeader* writeHeader(
      const Header & header, std::ostream & os, const std::string & filename,
      const std::string & dtype );
    inline static void writeObjectHeader( std::ostream & os, int timestep,
                                          PythonHeader *hdr );
    inline static void writeMtl( PythonHeader *hdr );
    std::string _name;
  };


  template<int D>
  class WavefrontMeshWriter<D, Void>
  {
  public:
    WavefrontMeshWriter( const std::string & name )
      : _name( name ) { }
    ~WavefrontMeshWriter() {}

    inline void write( const AimsTimeSurface<D, Void> & thing );

  private:
    template <int U, typename T>
    friend class aims::WavefrontMeshWriter;

    /// Return a name without .obj extension
    inline static std::string removeExtension(const std::string& name);

    inline static PythonHeader* writeHeader(
      const Header & header, std::ostream & os, const std::string & filename,
      const std::string & dtype );
    inline static void writeObjectHeader(
      std::ostream & os, int timestep, PythonHeader *hdr );
    inline static void writeMtl( PythonHeader *hdr );

  private:
    std::string _name;
  };


  template <int D, class T>
  inline
  WavefrontMeshWriter<D,T> &
  operator << ( WavefrontMeshWriter<D,T> & writer,
                const AimsTimeSurface<D,T> & thing )
  {
    writer.write( thing );
    return( writer );
  }


  template <int D, class T>
  std::string WavefrontMeshWriter<D,T>::removeExtension(
    const std::string& name )
  {
    return WavefrontMeshWriter<D, Void>::removeExtension( name );
  }


  template <int D, class T>
  PythonHeader* WavefrontMeshWriter<D,T>::writeHeader(
    const Header & header, std::ostream & os, const std::string & filename,
    const std::string & dtype )
  {
    return WavefrontMeshWriter<D, Void>::writeHeader( header, os, filename,
                                                      dtype );
  }


  template <int D, class T>
  void WavefrontMeshWriter<D,T>::writeObjectHeader(
    std::ostream & os, int timestep,
    PythonHeader *hdr )
  {
    WavefrontMeshWriter<D, Void>::writeObjectHeader( os, timestep, hdr );
  }


  template <int D, class T>
  void WavefrontMeshWriter<D,T>::writeMtl( PythonHeader *hdr )
  {
    WavefrontMeshWriter<D, Void>::writeMtl( hdr );
  }

  // ---

  template <int D>
  std::string WavefrontMeshWriter<D, Void>::removeExtension(
    const std::string& name )
  {
    std::string res = name;
    std::string ext="";
    if( res.length() > 4 )
      ext = res.substr( int(res.length() - 4), 4 );
    if( ext == ".obj" )
      res = res.substr( 0, res.length() - 4 );
    return res;
  }


  template <int D>
  PythonHeader* WavefrontMeshWriter<D, Void>::writeHeader(
    const Header & header, std::ostream & os, const std::string & filename,
    const std::string & dtype )
  {
    PythonHeader	*hdr = new PythonHeader;
    const PythonHeader
      *ph = dynamic_cast<const PythonHeader *>( &header );
    if( ph )
      hdr->copy( *ph );

    if( hdr->hasProperty( "nb_t_pos" ) )
      hdr->removeProperty( "nb_t_pos" );
    hdr->setProperty( "file_type", std::string( "WAVEFRONT" ) );
    hdr->setProperty( "object_type",
                      carto::DataTypeCode<
                        AimsTimeSurface<D, Void> >::objectType() );
    if( !dtype.empty() )
      hdr->setProperty( "data_type", dtype );

    os << "# Wavefront OBJ\n" << std::endl;

    carto::Object material;
    try
    {
      material = hdr->getProperty( "material" );
      if( material.get() )
      {
        std::string mtlfilename = removeExtension( filename ) + ".mtl";
        os << "mtllib " << mtlfilename << std::endl << std::endl;

        hdr->setProperty( "mtl", carto::Dictionary() );
        carto::Object mtl = hdr->getProperty( "mtl" );
        mtl->setProperty( "filename", mtlfilename );
      }
    }
    catch( ... )
    {
    }

    return hdr;
  }


  template <int D>
  void WavefrontMeshWriter<D, Void>::writeObjectHeader(
    std::ostream & os, int timestep, PythonHeader *hdr )
  {
    std::stringstream namess;
    namess << "mesh_" << timestep;
    std::string name = namess.str();
    os << "o " << name << std::endl << std::endl;
    os << "s 1\n" << std::endl;

    if( hdr->hasProperty( "material" ) )
    {
      carto::Object material, mtl;
      material = hdr->getProperty( "material" );
      mtl = hdr->getProperty( "mtl" );

      os << "usemtl " << name << std::endl << std::endl;

      carto::Object mtldict = carto::Object::value( carto::Dictionary() );

      try
      {
        mtldict = mtl->getProperty( name );
      }
      catch( ... )
      {
        mtl->setProperty( name, mtldict );
      }

      try
      {
        carto::Object ambient = material->getProperty( "ambient" );
        carto::Object it = ambient->objectIterator();
        std::stringstream s;
        for( int i=0; it->isValid() && i < 3; it->next(), ++i )
        {
          if( i != 0 )
            s << " ";
          s << it->currentValue()->getScalar();
        }
        mtldict->setProperty( "Ka", s.str() );
      }
      catch( ... )
      {
      }

      try
      {
        carto::Object diffuse = material->getProperty( "diffuse" );
        carto::Object it = diffuse->objectIterator();
        std::stringstream s;
        for( int i=0; it->isValid() && i < 3; it->next(), ++i )
        {
          if( i != 0 )
            s << " ";
          s << it->currentValue()->getScalar();
        }
        mtldict->setProperty( "Kd", s.str() );
      }
      catch( ... )
      {
      }

      try
      {
        carto::Object specular = material->getProperty( "specular" );
        carto::Object it = specular->objectIterator();
        std::stringstream s;
        for( int i=0; it->isValid() && i < 3; it->next(), ++i )
        {
          if( i != 0 )
            s << " ";
          s << it->currentValue()->getScalar();
        }
        mtldict->setProperty( "Ks", s.str() );
      }
      catch( ... )
      {
      }

      try
      {
        carto::Object shininess = material->getProperty( "shininess" );
        std::stringstream s;
        s << shininess->getScalar();
        mtldict->setProperty( "Ns", s.str() );
      }
      catch( ... )
      {
      }

    }
  }


  template <int D>
  void WavefrontMeshWriter<D, Void>::writeMtl( PythonHeader *hdr )
  {
    carto::Object mtl;
    try
    {
      mtl = hdr->getProperty( "mtl" );
    }
    catch( ... )
    {
      return;
    }

    std::string mtlfilename = mtl->getProperty( "filename" )->getString();
    std::ofstream ms( mtlfilename.c_str() );
    carto::Object oit = mtl->objectIterator();
    for( ; oit->isValid(); oit->next() )
    {
      if( oit->key() == "filename" )
        continue;
      std::string oname = oit->key();
      ms << "newmtl " << oname << std::endl << std::endl;
      carto::Object it = oit->currentValue()->objectIterator();
      for( ; it->isValid(); it->next() )
      {
        ms << it->key() << " " << it->currentValue()->getString() << std::endl;
      }
    }
    hdr->removeProperty( "mtl" );
  }

  // ---

  template <int D, class T>
  void WavefrontMeshWriter<D,T>::write( const AimsTimeSurface<D,T> & thing )
  {
    std::string fname = _name;
    std::ofstream os( fname.c_str() );
    if( !os )
      throw carto::file_error( "Could not open file", fname );

    PythonHeader *hdr = writeHeader(
      thing.header(), os, _name,
      carto::DataTypeCode< AimsTimeSurface<D, T> >::dataType() );

    typename AimsTimeSurface<D,T>::const_iterator is, es = thing.end();
    int timestep = 0;

    for( is=thing.begin(); is!=es; ++is, ++timestep )
    {
      writeObjectHeader( os, timestep, hdr );

      const std::vector<Point3df> &vert = (*is).second.vertex();
      const std::vector<Point3df> &norm = (*is).second.normal();
      const std::vector<AimsVector<uint,D> > &poly= is->second.polygon();

      // vertices
      for( std::vector<Point3df>::const_iterator iv = vert.begin();
           iv != vert.end(); ++iv )
        os << "v " << (*iv)[0] << " " << (*iv)[1] << " " << (*iv)[2]
          << std::endl;
      os << std::endl;

      // normals
      for( std::vector<Point3df>::const_iterator in = norm.begin();
            in != norm.end(); ++in )
        os << "vn " << (*in)[0] << " " << (*in)[1] << " " << (*in)[2]
          << std::endl;
      os << std::endl;

      // texture
      typename std::vector<T>::const_iterator it,
        et=is->second.texture().end();

      for( it=is->second.texture().begin(); it!=et; ++it )
      {
        os << "vt " << *it << std::endl;
      }
      os << std::endl;

      // polygons
      for( typename std::vector<AimsVector<uint,D> >::const_iterator ip =
             poly.begin(); ip != poly.end(); ++ip )
      {
        os << "f";
        for( int p=0; p<D; ++p )
          // WARNING obj indices start at 1 (like matlab...)
          os << " " << (*ip)[p] + 1 << "/" << (*ip)[p] + 1 << "/"
            << (*ip)[p] + 1;
        os << std::endl;
      }
      os << std::endl;

    }

    writeMtl( hdr );
    hdr->writeMinf( fname + ".minf" );

  }



  template <int D>
  void WavefrontMeshWriter<D, Void>::write(
    const AimsTimeSurface<D, Void> & thing )
  {
    std::string fname = _name;
    std::ofstream os( fname.c_str() );
    if( !os )
      throw carto::file_error( "Could not open file", fname );

    PythonHeader *hdr = writeHeader( thing.header(), os, _name, "VOID" );

    typename AimsTimeSurface<D, Void>::const_iterator is, es = thing.end();
    int timestep = 0;

    for( is=thing.begin(); is!=es; ++is, ++timestep )
    {
      writeObjectHeader( os, timestep, hdr );

      const std::vector<Point3df> &vert = (*is).second.vertex();
      const std::vector<Point3df> &norm = (*is).second.normal();
      const std::vector<AimsVector<uint,D> > &poly= is->second.polygon();

      // vertices
      for( std::vector<Point3df>::const_iterator iv = vert.begin();
           iv != vert.end(); ++iv )
        os << "v " << (*iv)[0] << " " << (*iv)[1] << " " << (*iv)[2]
          << std::endl;
      os << std::endl;

      // normals
      for( std::vector<Point3df>::const_iterator in = norm.begin();
            in != norm.end(); ++in )
        os << "vn " << (*in)[0] << " " << (*in)[1] << " " << (*in)[2]
          << std::endl;
      os << std::endl;

      // polygons
      for( typename std::vector<AimsVector<uint,D> >::const_iterator ip =
             poly.begin(); ip != poly.end(); ++ip )
      {
        os << "f";
        for( int p=0; p<D; ++p )
          // WARNING obj indices start at 1 (like matlab...)
          os << " " << (*ip)[p] + 1 << "//" << (*ip)[p] + 1;
        os << std::endl;
      }
      os << std::endl;

    }

    writeMtl( hdr );
    hdr->writeMinf( fname + ".minf" );
  }
}


#endif
