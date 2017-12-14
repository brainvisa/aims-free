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

#include <aims/io/wavefrontheader.h>
#include <aims/def/general.h>
#include <aims/vector/vector.h>
#include <cartobase/exception/ioexcept.h>
#include <fstream>
#include <soma-io/datasource/streamdatasource.h>
#include <soma-io/utilities/asciidatasourcetraits.h>

using namespace aims;
using namespace carto;
using namespace std;


WavefrontHeader::WavefrontHeader( const string & filename ) :
  PythonHeader(),
  _filename( filename )
{
}


WavefrontHeader::~WavefrontHeader()
{
}


bool WavefrontHeader::read( size_t* offset )
{
  setProperty( "file_type", string( "WAVEFRONT" ) );
  setProperty( "data_type", string( "VOID" ) );
  setProperty( "polygon_dimension", 3 );

  readMesh( true, 0, -1 );

  // add meta-info to header
  readMinf( removeExtension( filename() ) + extension() + ".minf" );

  return true;
}


string WavefrontHeader::openMode() const
{
  return string("ascii");
}


bool WavefrontHeader::byteSwapping() const
{
  int	bswap = 0;
  getProperty( "byte_swapping", bswap );
  return (bool)bswap;
}


string WavefrontHeader::filename() const
{
  if ( _filename.length() > 4
      && ( _filename.substr( _filename.length() - 4, 4 ) == ".obj" )
           || _filename.substr( _filename.length() - 4, 4 ) == ".OBJ" )
    return _filename;
  else
    return _filename + ".obj";
}


set<string> WavefrontHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".obj" );
  return exts;
}


void WavefrontHeader::readMesh( bool stopWhenIdentified, int poly_size,
                                int tex_num )
{
  std::string fname = filename();
//   WavefrontHeader hdr( _name );
//   hdr.read();

  std::ifstream istr( fname.c_str(), std::ios::in );
  istr.unsetf( std::ios::skipws );
  if( !istr )
    soma::io_error::launchErrnoExcept( fname );

  istr.setf( std::ios::skipws );
  soma::IStreamDataSource ds( istr );
//   thing.erase();
  std::string l;
  int line = 0;

  std::vector<Point3df> vertices, normals;
  std::vector<vector<uint> > polygons;
  std::vector<float> texture;
  std::vector<int> normals_ind;
  std::vector<int> texture_ind;
  int timestep = 0;
  bool update_normals = false;
  carto::Object mtl_dict;
  int has_texture = -1;

  while( ds && !ds.eof() )
  {
    if( !soma::StreamUtil::getline( ds, l ) )
    {
      if( ds.eof() )
        break;
      else
        throw soma::wrong_format_error( fname );
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
//         storeMesh( thing, vertices, normals, polygons, texture, normals_ind,
//                     texture_ind, timestep );

//         if( thing[timestep].normal().size() != vertices.size() )
//           update_normals = true;
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
//       mtl_dict = readMtlFle( mtl_filename );
    }
    else if( element == "usemtl" )
    {
      std::string mtl_objname;
      s >> mtl_objname;
//       setMtl( mtl_dict, mtl_objname, hdr );
    }
    else if( element == "s" )
    {
      // ?
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
      if( tex_num < 1 )
      {
        tex_num = 1;
        setProperty( "data_type", "FLOAT" );
      }
      float tex;
      s >> tex;
      texture.push_back(tex);
    }
    else if( element == "f" )
    {
      // face
      std::string item;
      std::vector<uint> poly;
      int tex;
      int norm;
      vector<string::size_type> pos;
      string::size_type pos0 = 2, pos1, ls = l.length();
      while( l[pos0] == ' ' || ( l[pos0] == '\t' && pos0 < ls ) )
        ++pos0;
      if( pos0 == ls )
        throw carto::parse_error( "malformed face", "", fname, line );
      pos.push_back( pos0 );
      while( pos0 != string::npos && pos0 != string::npos )
      {
        while( l[pos0] == ' ' || ( l[pos0] == '\t' && pos0 < ls ) )
          ++pos0;
        if( pos0 == ls )
          break;
        pos1 = l.find( ' ', pos0 + 1 );
        pos.push_back( pos1 );
        pos0 = pos1;
      }
      int p = pos.size() - 1;
      if( poly_size == 0 )
      {
        poly_size = p;
        setProperty( "polygon_dimension", poly_size );
      }
      else if( p != poly_size )
        throw carto::parse_error( "inconsistant polygon size", "",
                                  fname, line );

      poly.resize( poly_size );

      for( p=0; p<poly_size; ++p )
      {
        item = l.substr( pos[p], pos[p+1] - pos[p] );
        pos0 = item.find('/');
        std::stringstream z( item.substr(0, pos0 ) );
        z >> poly[p];
        --poly[p]; // nums start at 1
        if( pos0 == std::string::npos )
          continue;

        pos1 = item.find( '/', pos0 + 1 );
        if( pos1 == pos0 + 1 )
        {
          if( tex_num > 0 )
            throw carto::parse_error( "malformed face, mising texrure", "",
                                      fname, line );
          else if( tex_num < 0 )
          {
            tex_num = 0;
            setProperty( "data_type", "VOID" );
          }
        }
        else
        {
          if( tex_num < 0 )
          {
            tex_num = 1;
            setProperty( "data_type", "FLOAT" );
          }
          else if( tex_num != 1 )
            throw carto::parse_error(
              "malformed face, inconsistant texture numbers", "", fname,
              line );
          std::stringstream z( item.substr(pos0, pos1 - pos0 ) );
          z >> tex;
          if( texture_ind.size() <= poly[p] )
            texture_ind.resize( poly[p] + 1 );
          texture_ind[ poly[p] ] = tex;
        }
        if( pos1 == std::string::npos )
          continue;
        pos0 = pos1;
        pos1 = item.find('/', pos0 + 1);
        if( pos1 == std::string::npos )
          continue;
        std::stringstream n( item.substr(pos0, pos1 - pos0 ) );
        n >> norm;
        if( normals_ind.size() <= poly[p] )
          normals_ind.resize( poly[p] + 1 );
        normals_ind[ poly[p] ] = norm;
      }
      polygons.push_back( poly );
    }
    else
    {
      std::cerr << "unrecognized element: " << l << std::endl;
    }

    if( stopWhenIdentified && poly_size > 0 && tex_num >= 0 )
      break;
  }

//   storeMesh( thing, vertices, normals, polygons, texture, normals_ind,
//               texture_ind, timestep );
//
//   if( update_normals || thing[timestep].normal().size() != vertices.size() )
//     thing.updateNormals();

  // std::cout << "WAVEFRONT read OK\n";

  if( hasProperty( "filenames" ) )
    removeProperty( "filenames" );
//   thing.setHeader( hdr );
}
