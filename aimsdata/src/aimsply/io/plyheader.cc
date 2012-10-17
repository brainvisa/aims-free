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

#include <aims/io/plyheader.h>
#include <aims/def/general.h>
#include <aims/io/byteswap.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/getopt/getopt.h>
#include <cartobase/stream/fdinhibitor.h>
#include <aims/rply/rply.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace aims;
using namespace carto;
using namespace std;


PlyHeader::PlyHeader( const string & filename ) : 
  PythonHeader(), 
  _filename( filename )
{
}


PlyHeader::~PlyHeader()
{
}


namespace
{
  int vertex_cb(p_ply_argument /*argument*/)
  {
    return 1;
  }

  int face_cb( p_ply_argument argument )
  {
    long length, value_index;
    ply_get_argument_property(argument, NULL, &length, &value_index);
    if( value_index == -1 )
      {
        int	polysz = (int) ply_get_argument_value(argument);
        void	*pdata;
        if( !ply_get_argument_user_data(argument, &pdata, 0) )
          return 0;
        if( pdata )
          *(int *) pdata = polysz;
      }
    return 0;
  }

}


bool PlyHeader::read( uint32_t* )
{
  if( FileUtil::fileStat( filename() ).empty() )
    return false;	// file not found
  long nvertices = 0, ntriangles = 0;
  p_ply ply = ply_open( filename().c_str(), NULL, 0, NULL);
  int	polysize = 3;
  if (!ply) return false;
  if (!ply_read_header(ply)) return false;

  p_ply_element	elem = ply_get_next_element( ply, 0 );
  bool		found = false;
  const char	*name;
  long		ninstances;
  vector<string>	textures;

  while( !found && elem )
    {
      if( !ply_get_element_info( elem, &name, &ninstances ) )
        break;
      if( !strcmp( name, "vertex" ) )
        {
          nvertices = ninstances;
          found = true;
          p_ply_property prop = ply_get_next_property( elem, 0 );
          const char	*pname;
          e_ply_type	ptype, plentype, pvaltype;

          while( prop )
            {
              if( ply_get_property_info( prop, &pname, &ptype, &plentype, 
                                         &pvaltype ) )
                {
                  if( ptype != PLY_LIST 
                      && strcmp( pname, "x" ) && strcmp( pname, "y" ) 
                      && strcmp( pname, "z" ) && strcmp( pname, "nx" ) 
                      && strcmp( pname, "ny" ) && strcmp( pname, "nz" ) )
                    textures.push_back( string( pname ) );
                }
              prop = ply_get_next_property( elem, prop );
            }
        }
      elem = ply_get_next_element( ply, elem );
    }

  ntriangles = ply_set_read_cb( ply, "face", "vertex_indices", face_cb, 
                                &polysize, 0);

  // avoid printing anything from RPly lib
  fdinhibitor	fdi( STDERR_FILENO );
  fdi.close();

  ply_read(ply);

  fdi.open();
  ply_close(ply);

  setProperty( "file_type", string( "PLY" ) );
  string	ot = "Mesh";
  switch( polysize )
    {
    case 2:
      ot = "Segments";
      break;
    case 4:
      ot = "Mesh4";
      break;
    default:
      break;
    }
  setProperty( "object_type", ot );
  setProperty( "data_type", string( "VOID" ) );
  setProperty( "polygon_dimension", (int) polysize );
  setProperty( "nb_t_pos", (int) 1 );
  setProperty( "vertex_number", (int) nvertices );
  setProperty( "polygon_number", (int) ntriangles );
  if( !textures.empty() )
    setProperty( "texture_names", textures );

  // add meta-info to header
  readMinf( filename() + ".minf" );

  return true;
}


string PlyHeader::filename() const
{
  if( FileUtil::extension( _filename ) == "ply" )
    return _filename;
  else
    return _filename + ".ply";
}


set<string> PlyHeader::extensions() const
{
  set<string> exts;
  exts.insert( ".ply" );
  return exts;
}

