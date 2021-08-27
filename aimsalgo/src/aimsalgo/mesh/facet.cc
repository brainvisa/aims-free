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

#include <aims/mesh/facet.h>
using aims::Connectivity;

using namespace std;


int 
Facet::getNeighbor( const Connectivity& connect, Facet** pNeighbor ) const
{
  short* it_central = _offset;
  short* it_front   = _offset + connect.offset( _type );

  int t = 0;
  for ( byte i = 0; i < 3; i++ )
    if ( _type != i && _type != 5 - i )
    {
      if ( *( it_front + connect.offset( i ) ) == *it_central )
      {
        pNeighbor[ t ] = new Facet;
        pNeighbor[ t ]->offset() = it_front + connect.offset( i );
        pNeighbor[ t ]->type() = 5 - i;
        pNeighbor[ t ]->location() = _location + connect.xyzOffset( _type ) +
                                     connect.xyzOffset( i );
        pNeighbor[ t ]->id() = pNeighbor[ t ]->key(); 
        t++;
      }
      else if ( *( it_central + connect.offset( i ) ) == *it_central )
      {
        pNeighbor[ t ] = new Facet;
        pNeighbor[ t ]->offset() = it_central + connect.offset( i );
        pNeighbor[ t ]->type() = _type;
        pNeighbor[ t ]->location() = _location + connect.xyzOffset( i );
        pNeighbor[ t ]->id() = pNeighbor[ t ]->key(); 
        t++;
      }
      else
      {
        pNeighbor[ t ] = new Facet;
        pNeighbor[ t ]->offset() = it_central;
        pNeighbor[ t ]->type() = i;
        pNeighbor[ t ]->location() = _location;
        pNeighbor[ t ]->id() = pNeighbor[ t ]->key(); 
        t++;
      }
      if ( *( it_front + connect.offset( 5 - i ) ) == *it_central )
      {
        pNeighbor[ t ] = new Facet;
        pNeighbor[ t ]->offset() = it_front + connect.offset( 5 - i );
        pNeighbor[ t ]->type() = i;
        pNeighbor[ t ]->location() = _location + connect.xyzOffset( _type ) +
                                     connect.xyzOffset( 5 - i );
        pNeighbor[ t ]->id() = pNeighbor[ t ]->key(); 
        t++;
      }
      else if ( *( it_central + connect.offset( 5 - i ) ) == *it_central )
      {
        pNeighbor[ t ] = new Facet;
        pNeighbor[ t ]->offset() = it_central + connect.offset( 5 - i );
        pNeighbor[ t ]->type() = _type;
        pNeighbor[ t ]->location() = _location + connect.xyzOffset( 5 - i );
        pNeighbor[ t ]->id() = pNeighbor[ t ]->key(); 
        t++;
      }
      else
      {
        pNeighbor[ t ] = new Facet;
        pNeighbor[ t ]->offset() = it_central;
        pNeighbor[ t ]->type() = 5 - i;
        pNeighbor[ t ]->location() = _location;
        pNeighbor[ t ]->id() = pNeighbor[ t ]->key(); 
        t++;
      }
    }
  return t;
}
