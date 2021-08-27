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

#include <aims/mesh/mesher.h>

using namespace std;

float Mesher::surfaceOfInterface( const AimsSurfaceTriangle& surface )
{
  const vector< Point3df >& vertex = surface.vertex();
  const vector< AimsVector< uint,3> >& triangle = surface.polygon();

  float surf = 0.0f;
  Point3df H, AB, AC, CH, zero( 0.0, 0.0, 0.0 );
  int size = (int)triangle.size();

  for ( int n = 0; n < size; n++ )
  {
    AB = vertex[ triangle[ n ][ 1 ] ] - vertex[ triangle[ n ][ 0 ] ];
    AC = vertex[ triangle[ n ][ 2 ] ] - vertex[ triangle[ n ][ 0 ] ];
    H  = vertex[ triangle[ n ][ 0 ] ] + ( AB.dot( AC ) / norm2( AB ) ) * AB;
    CH = H - vertex[ triangle[ n ][ 2 ] ];
    if ( AB != zero )
      surf += norm( CH ) * norm( AB ) / 2;
  }

  return surf;
}
