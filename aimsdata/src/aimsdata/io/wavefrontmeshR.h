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

#ifndef AIMS_IO_WAVEFRONTMESHR_H
#define AIMS_IO_WAVEFRONTMESHR_H

#include <aims/data/pheader.h>
#include <aims/io/datatypecode.h>
#include <aims/io/wavefrontheader.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/fileutil.h>

#include <aims/mesh/texture.h>
#include <aims/io/writer.h>

#include <soma-io/utilities/asciidatasourcetraits.h>
#include <soma-io/datasource/streamdatasource.h>
#include <soma-io/datasource/filedatasource.h>
#include <cartobase/exception/ioexcept.h>


namespace aims
{

  template<long D, typename T>
  class WavefrontMeshReader
  {
  public:
    WavefrontMeshReader( const std::string& name ) : _name( name ) {}
    ~WavefrontMeshReader() {}

    void read( AimsTimeSurface<D,T>& thing,
               const carto::AllocatorContext & context,
               carto::Object options );

  private:
    void storeMesh( AimsTimeSurface<D,T> & thing,
                    const std::vector<Point3df> & vertices,
                    const std::vector<Point3df> &normals,
                    const std::vector<AimsVector<uint, D> > & polygons,
                    const std::vector<T> & texture,
                    const std::vector<int> & normals_ind,
                    const std::vector<int> & texture_ind, int timestep );
    inline static
    carto::Object readMtlFle( std::string & mtl_filename );
    inline static
    void setMtl( carto::Object mtl_dict, const std::string & mtl_objname,
                 PythonHeader & hdr );

    std::string		_name;
  };


  template <long D, typename T>
  inline WavefrontMeshReader<D,T> &
  operator >> ( WavefrontMeshReader<D,T> & reader,
                AimsTimeSurface<D,T> & thing )
  {
    reader.read( thing, thing.allocator(),
                 carto::Object::value( carto::PropertySet() ) );
    return reader;
  }


  template <long D, typename T>
  inline
  carto::Object WavefrontMeshReader<D,T>::readMtlFle(
    std::string & mtl_filename )
  {
    carto::Object mtl_dict = carto::Object::value( carto::Dictionary() );

    std::ifstream ms( mtl_filename.c_str() );
    if( !ms )
    {
      std::cerr << "warning: could not open " << mtl_filename << std::endl;
      return mtl_dict;
    }

    ms.setf( std::ios::skipws );
    soma::IStreamDataSource ds( ms );
    int line = 0;
    carto::Object current_obj;
    std::string l, element, objname;

    while( ds && !ds.eof() )
    {
      if( !soma::StreamUtil::getline( ds, l ) )
      {
        if( ds.eof() )
          break;
        else
        {
          std::cerr << "warning: could read " << mtl_filename << ", line "
            << line + 1 << std::endl;
          return mtl_dict;
        }
      }
      ++line;

      if( l.empty() || l[0] == '#' )
        continue;

      std::stringstream s( l );
      s >> element;

      if( element == "newmtl" )
      {
        s >> objname;
        current_obj = carto::Object::value( carto::Dictionary() );
        mtl_dict->setProperty( objname, current_obj );
      }
      else if( element == "Ka" )
      {
        if( !current_obj )
        {
          std::cerr << "MTL error: no current object.\n";
          continue;
        }
        std::vector<float> ambient( 3 );
        s >> ambient[0] >> ambient[1] >> ambient[2];
        current_obj->setProperty( "ambient", ambient );
      }
      else if( element == "Kd" )
      {
        if( !current_obj )
        {
          std::cerr << "MTL error: no current object.\n";
          continue;
        }
        std::vector<float> diffuse( 3 );
        s >> diffuse[0] >> diffuse[1] >> diffuse[2];
        current_obj->setProperty( "diffuse", diffuse );
      }
      else if( element == "Ks" )
      {
        if( !current_obj )
        {
          std::cerr << "MTL error: no current object.\n";
          continue;
        }
        std::vector<float> specular( 3 );
        s >> specular[0] >> specular[1] >> specular[2];
        current_obj->setProperty( "specular", specular );
      }
      else if( element == "Ns" )
      {
        if( !current_obj )
        {
          std::cerr << "MTL error: no current object.\n";
          continue;
        }
        float ns;
        s >> ns;
        current_obj->setProperty( "shininess", ns );
      }
      else
      {
        std::cout << "unrecognized MTL tag: " << element << std::endl;
      }
    }

    return mtl_dict;
  }


  template <long D, typename T>
  inline
  void WavefrontMeshReader<D,T>::setMtl( carto::Object mtl_dict,
                                         const std::string & mtl_objname,
                                         PythonHeader & hdr )
  {
    carto::Object material = carto::Object::value( carto::Dictionary() );
    try
    {
      material = hdr.getProperty( "material" );
    }
    catch( ... )
    {
      hdr.setProperty( "material", material );
    }

    try
    {
      carto::Object m = mtl_dict->getProperty( mtl_objname );
      material->copyProperties( m );
    }
    catch( ... )
    {
    }
  }


  template <long D, typename T>
  inline
  void WavefrontMeshReader<D,T>::storeMesh(
    AimsTimeSurface<D,T>& thing,
    const std::vector<Point3df> & vertices,
    const std::vector<Point3df> &normals,
    const std::vector<AimsVector<uint, D> > & polygons,
    const std::vector<T> & texture,
    const std::vector<int> & normals_ind,
    const std::vector<int> & texture_ind, int timestep )
  {
    thing[timestep].vertex() = vertices;
    std::vector<Point3df> o_normals;
    o_normals.resize( normals_ind.size() );
    // copy reordered normals
    for( int i=0, k=normals_ind.size(); i<k; ++i )
    {
      if( normals_ind[i] < 0 || normals_ind[i] >= vertices.size() )
      {
        std::stringstream s;
        s << "normal index out of range: " << normals_ind[i] << " / "
          << vertices.size() << " at index " << i;
        throw carto::parse_error( s.str() , i, _name );
      }
      o_normals[i] = normals[normals_ind[i]];
    }
    thing[timestep].normal() = o_normals;
    thing[timestep].polygon() = polygons;

    if( carto::DataTypeCode<T>::name()
        != carto::DataTypeCode<Void>::name() )
    {
      std::cout << "read texture\n";
      std::cout << "tex 0: " << texture[0] << std::endl;
      // copy reordered texture
      std::vector<T> o_texture;
      o_texture.resize( texture_ind.size() );
      for( int i=0, k=texture_ind.size(); i<k; ++i )
      {
        if( texture_ind[i] < 0 || texture_ind[i] >= vertices.size() )
          throw carto::parse_error( "texture index out of range", i, _name );
        o_texture[i] = texture[texture_ind[i]];
//         std::cout << texture_ind[i] << " -> " << texture[texture_ind[i]] << std::endl;
      }
      thing[timestep].texture() = o_texture;
    }
  }


  template <long D, typename T>
  inline
  void WavefrontMeshReader<D,T>::read( AimsTimeSurface<D,T>& thing,
                             const carto::AllocatorContext & /*context*/,
                             carto::Object /*options*/ )
  {
    std::string filename = _name;
    WavefrontHeader hdr( _name );
    hdr.read();

    std::ifstream istr( filename.c_str(), std::ios::in );
    istr.unsetf( std::ios::skipws );
    if( !istr )
      soma::io_error::launchErrnoExcept( filename );

    istr.setf( std::ios::skipws );
    soma::IStreamDataSource ds( istr );
    thing.erase();
    std::string l;
    int line = 0;

    std::vector<Point3df> vertices, normals;
    std::vector<AimsVector<uint, D> > polygons;
    std::vector<T> texture;
    std::vector<int> normals_ind;
    std::vector<int> texture_ind;
    int timestep = 0;
    bool update_normals = false;
    carto::Object mtl_dict;

    while( ds && !ds.eof() )
    {
      if( !soma::StreamUtil::getline( ds, l ) )
      {
        if( ds.eof() )
          break;
        else
          throw soma::wrong_format_error( filename );
      }
      ++line;

      if( l.empty() )
        continue;
      if( l[0] == '#' )
        continue;
      std::stringstream s(l);
      std::string element;
      s >> element;
      if( element == "o" || element == "g" )
      {
        // new object / group
        if( timestep != 0 || !vertices.empty() )
        {
          storeMesh( thing, vertices, normals, polygons, texture, normals_ind,
                     texture_ind, timestep );

          if( thing[timestep].normal().size() != vertices.size() )
            update_normals = true;
          ++timestep;
          vertices.clear();
          normals.clear();
          polygons.clear();
          texture.clear();
          normals_ind.clear();
          texture_ind.clear();
        }
      }
      else if( element == "mtllib" )
      {
        std::string mtl_filename;
        s >> mtl_filename;
        mtl_dict = readMtlFle( mtl_filename );
      }
      else if( element == "usemtl" )
      {
        std::string mtl_objname;
        s >> mtl_objname;
        setMtl( mtl_dict, mtl_objname, hdr );
      }
      else if( element == "v" )
      {
        // vertex
        Point3df p;
        s >> p[0] >> p[1] >> p[2];
        vertices.push_back(p);
      }
      else if( element == "vn" )
      {
        // normal
        Point3df p;
        s >> p[0] >> p[1] >> p[2];
        normals.push_back(p);
      }
      else if( element == "vt" )
      {
        // texture
        T tex;
        s >> tex;
        texture.push_back(tex);
      }
      else if( element == "f" )
      {
        // face
        std::string item;
        std::string::size_type pos, pos0;
        AimsVector<uint, D> poly;
        int tex;
        int norm;
        for( int p=0; p<D; ++p )
        {
          s >> item;
          pos0 = item.find( '/' );
          std::stringstream z( item.substr(0, pos0 ) );
          z >> poly[p];
          --poly[p]; // nums start at 1
          if( pos0 == std::string::npos )
            continue;

          ++pos0;
          pos = item.find( '/', pos0 );
          if( carto::DataTypeCode<T>::name()
              != carto::DataTypeCode<Void>::name() )
          {
            std::stringstream z( item.substr(pos0, pos - pos0 ) );
            z >> tex;
            if( texture_ind.size() <= poly[p] )
              texture_ind.resize( poly[p] + 1 );
            texture_ind[ poly[p] ] = tex - 1; // (starts at 1)
          }
          else if( pos != pos0 )
            throw carto::parse_error( "malformed face", line, filename );
          if( pos == std::string::npos )
            continue;
          pos0 = pos + 1;
          pos = item.find( '/', pos0 );
          if( pos == std::string::npos )
            continue;
          std::stringstream n( item.substr(pos0, pos - pos0 ) );
          n >> norm;
          std::cout << item << " " << pos0 << " " << pos << " " << n << std::endl;
          if( normals_ind.size() <= poly[p] )
            normals_ind.resize( poly[p] + 1 );
          normals_ind[ poly[p] ] = norm - 1; // (starts at 1)
        }
        polygons.push_back( poly );
      }
      else
      {
        std::cerr << "unrecognized element: " << l << std::endl;
      }
    }

    storeMesh( thing, vertices, normals, polygons, texture, normals_ind,
               texture_ind, timestep );

    if( update_normals || thing[timestep].normal().size() != vertices.size() )
      thing.updateNormals();

    // std::cout << "WAVEFRONT read OK\n";

    if( hdr.hasProperty( "filenames" ) )
      hdr.removeProperty( "filenames" );
    thing.setHeader( hdr );
  }

}

#endif

