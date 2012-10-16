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

#ifndef AIMS_IO_PLYR_H
#define AIMS_IO_PLYR_H

#include <aims/data/data.h>
#include <aims/io/plyheader.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/datasource/filedatasource.h>
#include <aims/rply/rply.h>

#include <aims/mesh/texture.h>
#include <aims/io/writer.h>


namespace aims
{

  template<long D, typename T>
  class PlyReader
  {
  public:
    PlyReader( const std::string& name ) : _name( name ) {}
    ~PlyReader() {}

    void read( AimsTimeSurface<D,T>& thing, 
               const carto::AllocatorContext & context, 
               carto::Object options );

  private:
    static int ply_vertex_cb(p_ply_argument argument);
    static int ply_normal_cb(p_ply_argument argument);
    static int ply_face_cb(p_ply_argument argument);
    static int ply_tex_cb(p_ply_argument argument);
    std::string		_name;
  };


  template <long D, typename T>
  inline PlyReader<D,T> & 
  operator >> ( PlyReader<D,T> & reader, AimsTimeSurface<D,T> & thing )
  {
    reader.read( thing, thing.allocator(), 
                 carto::Object::value( carto::PropertySet() ) );
    return reader;
  }


  template <long D, typename T> 
  inline int PlyReader<D,T>::ply_vertex_cb(p_ply_argument argument)
  {
    long	n;
    void	*pdata;
    if( !ply_get_argument_user_data(argument, &pdata, &n) )
      return 0;
    AimsTimeSurface<D,T>	*mesh = (AimsTimeSurface<D,T> *) pdata;
    double	x = ply_get_argument_value(argument);
    if( n == 0 )
      mesh->begin()->second.vertex().push_back( Point3df( (float) x, 0, 
                                                          0 ) );
    else
      mesh->begin()->second.vertex().back()[n] = (float) x;
    return 1;
  }


  template <long D, typename T> 
  inline int PlyReader<D,T>::ply_normal_cb(p_ply_argument argument)
  {
    long	n;
    void	*pdata;
    if( !ply_get_argument_user_data(argument, &pdata, &n) )
      return 0;
    AimsTimeSurface<D,T>	*mesh = (AimsTimeSurface<D,T> *) pdata;
    double	x = ply_get_argument_value(argument);
    if( n == 0 )
      mesh->begin()->second.normal().push_back( Point3df( (float) x, 0, 
                                                          0 ) );
    else
      mesh->begin()->second.normal().back()[n] = (float) x;
    return 1;
  }


  template <long D, typename T> 
  inline int PlyReader<D,T>::ply_tex_cb(p_ply_argument argument)
  {
    long	n;
    void	*pdata;
    if( !ply_get_argument_user_data(argument, &pdata, &n) )
      return 0;
    TimeTexture<float>	*tex = (TimeTexture<float> *) pdata;
    double		x = ply_get_argument_value(argument);
    (*tex)[n].push_back( (float) x );
    return 1;
  }


  template <long D, typename T> 
  inline int PlyReader<D,T>::ply_face_cb(p_ply_argument argument)
  {
    long length, value_index;
    void	*pdata;
    ply_get_argument_property(argument, NULL, &length, &value_index);
    if( !ply_get_argument_user_data(argument, &pdata, 0) )
      return 0;
    AimsTimeSurface<D,T>	*mesh = (AimsTimeSurface<D,T> *) pdata;
    if( value_index == 0 )
      {
        AimsVector<uint,D>	v;
        v[0] = (uint) rint( ply_get_argument_value(argument) );
        mesh->begin()->second.polygon().push_back( v );
      }
    else if( value_index > 0 && value_index < D )
      mesh->begin()->second.polygon().back()[ value_index ] 
        = (uint) rint( ply_get_argument_value(argument) );
    else if( value_index > 0 )
      std::cerr << "polygon with too many items: " << value_index << "\n";
    return 1;
  }


  template <long D, typename T>
  inline
  void PlyReader<D,T>::read( AimsTimeSurface<D,T>& thing, 
                             const carto::AllocatorContext & /*context*/, 
                             carto::Object /*options*/ )
  {
    std::cout << "PlyReader<" << D << ", " << carto::DataTypeCode<T>::name() 
              << ">\n";
    PlyHeader hdr( _name );
    hdr.read();

    long nvertices, ntriangles;
    p_ply ply = ply_open( _name.c_str(), NULL);
    if (!ply)
      throw carto::wrong_format_error( _name );
    if (!ply_read_header(ply))
      throw carto::wrong_format_error( _name );

    nvertices = ply_set_read_cb(ply, "vertex", "x", ply_vertex_cb, &thing, 0);
    thing.vertex().reserve( nvertices );
    thing.normal().reserve( nvertices );
    ply_set_read_cb(ply, "vertex", "y", ply_vertex_cb, &thing, 1);
    ply_set_read_cb(ply, "vertex", "z", ply_vertex_cb, &thing, 2);
    ply_set_read_cb(ply, "vertex", "nx", ply_normal_cb, &thing, 0);
    ply_set_read_cb(ply, "vertex", "ny", ply_normal_cb, &thing, 1);
    ply_set_read_cb(ply, "vertex", "nz", ply_normal_cb, &thing, 2);

    std::vector<std::string>	textures;
    carto::ObjectVector		*tex = 0;
    hdr.getProperty( "texture_names", textures );
    unsigned			ntex = textures.size();
    if( ntex )
      {
        carto::Object 
          o = carto::Object::value( carto::ObjectVector() );
        hdr.setProperty( "textures", o );
        tex = &o->carto::GenericObject::value<carto::ObjectVector>();
        tex->reserve( ntex );
        unsigned	i;

        for( i=0; i<ntex; ++i )
          {
            tex->push_back( carto::Object::value( TimeTexture<float>() ) );
            ply_set_read_cb( ply, "vertex", textures[i].c_str(), ply_tex_cb, 
                             &(*tex)[i]->carto::GenericObject::value
                             < TimeTexture<float> >(), 0 );
          }
      }

    ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", ply_face_cb, 
                                 &thing, 0);
    thing.polygon().reserve( ntriangles );
    int res = ply_read(ply);
    ply_close(ply);
    if( !res )
      throw carto::invalid_format_error( "PLY reader failed", _name );

    // verify polygons are all in the vertices range
    std::vector<AimsVector<uint,D> >  & poly = thing.polygon();
    typename std::vector<AimsVector<uint,D> >::iterator	ip;
    bool	broken = false;
    for ( ip=poly.begin(); ip!=poly.end(); ++ip )
      {
        AimsVector<uint,D>	& pol = *ip;
        for ( int j=0; j<D; ++j )
          if ( pol[j] >= (unsigned) nvertices )
            {
              if ( !broken )
                {
                  broken = true;
                  std::cerr << "Broken mesh: polygon pointing to a " 
                            << "vertex out of range" << std::endl;
                  std::cerr << pol[j] << " >= " << nvertices << std::endl;
                }
              poly.erase( ip );
              --ip;
              break;
            }
      }
    if( thing.normal().size() != thing.vertex().size() )
      { std::cout << "generating normals: " << thing.normal().size() 
                  << " instead of " << thing.vertex().size() << "\n";
      thing.updateNormals();
      } else std::cout << "normals read\n";

    std::cout << "PLY read OK\n";

    if( hdr.hasProperty( "filenames" ) )
      hdr.removeProperty( "filenames" );
    thing.setHeader( hdr );
  }

}

#endif

