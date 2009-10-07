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
 *  Tri reader class
 */
#include <aims/io/triR.h>
#include <aims/io/triheader.h>
#include <cartobase/exception/ioexcept.h>

using namespace aims;
using namespace carto;
using namespace std;


void TriReader::read( AimsSurfaceTriangle& thing, int frame )
{
  if( frame >= 0 )
    cerr << "Warning : .tri single frame reading not implemented yet -\n" 
	 << "reading whole mesh\n";

  TriHeader	hdr( _name );
  size_t	offset;
  if( !hdr.read( &offset ) )
    io_error::launchErrnoExcept( hdr.filename() );

  ifstream	is( hdr.filename().c_str() );
  if( !is )
    io_error::launchErrnoExcept( hdr.filename() );
#ifdef _WIN32
  /* apparently windows can't properly tellg/seekg on ascii streams. 
     It's mysterious. */
  string tstr;
  is >> tstr >> tstr;
#else
  is.seekg( offset );
#endif
  if( !is )
    io_error::launchErrnoExcept( hdr.filename() );

  //	cleanup surface
  thing.erase();

  int nVertex = 0;
  hdr.getProperty( "vertex_number", nVertex );

  thing.vertex().reserve( nVertex );
  thing.normal().reserve( nVertex );

  Point3df vertex, normal;
  int		n;

  //cout << "TRI: reading vertices\n";
  for( n = 0; n < nVertex; ++n )
  {
    is >> vertex[ 0 ] >> vertex[ 1 ] >> vertex[ 2 ]
       >> normal[ 0 ] >> normal[ 1 ] >> normal[ 2 ];
    thing.vertex().push_back( vertex );
    thing.normal().push_back( normal );
  }
  /*cout << "TRI: vertices OK\n";
  cout << "first vertex: " << *thing.vertex().begin() << endl;
  cout << "last vertex: " << *thing.vertex().rbegin() << endl;*/

  string	tmp;
  is >> tmp;
  if( tmp != "-" )
    throw corrupt_stream_error( hdr.filename() );
  int nTriangle = 0;
  is >> nTriangle;
  is >> tmp;
  is >> tmp;

  thing.polygon().reserve( nTriangle );

  //cout << "TRI: reading triangles\n";

  AimsVector< uint, 3 > triangle;
  for ( n = 0; n < nTriangle; n++ )
  {
    is >> triangle[ 0 ] >> triangle[ 1 ] >> triangle[ 2 ];
    thing.polygon().push_back( triangle );
  }
  //cout << "TRI: triangles OK\n";

  if( thing.normal().size() != thing.vertex().size() )
    thing.updateNormals();

  thing.setHeader( hdr );
}
