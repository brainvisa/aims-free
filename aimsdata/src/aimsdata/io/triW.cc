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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Tri writer class
 */
#include <aims/io/triW.h>
#include <aims/io/triheader.h>
#include <cartobase/exception/file.h>

using namespace aims;
using namespace carto;
using namespace std;

void 
TriWriter::write( const AimsSurfaceTriangle& thing )
{
  TriHeader	hdr( _name );
  string name = hdr.removeExtension( _name ) + ".tri";
  ofstream	os( name.c_str() );
  if( !os )
    throw file_error( name );

  os << "- " << thing.vertex().size() << endl;

  vector<Point3df>::size_type n;

  for ( n = 0; n < thing.vertex().size(); n++ )
    os << thing.vertex()[ n ][ 0 ] << " "
       << thing.vertex()[ n ][ 1 ] << " "
       << thing.vertex()[ n ][ 2 ] << " "
       << thing.normal()[ n ][ 0 ] << " "
       << thing.normal()[ n ][ 1 ] << " "
       << thing.normal()[ n ][ 2 ] << endl;
  os << "- " << thing.polygon().size() << " "
     << thing.polygon().size() << " "
     << thing.polygon().size() << endl;
  for ( n = 0; n < thing.polygon().size(); n++ )
    os << thing.polygon()[ n ][ 0 ] << " "
       << thing.polygon()[ n ][ 1 ] << " "
       << thing.polygon()[ n ][ 2 ] << endl;

  hdr.copy( thing.header() );
  if( hdr.hasProperty( "nb_t_pos" ) )
    hdr.removeProperty( "nb_t_pos" );
  hdr.writeMinf( hdr.filename() + ".minf" );
}


