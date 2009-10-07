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


#include <aims/mesh/mesher.h>
using aims::Connectivity;

using namespace std;


void Mesher::splitting( const AimsData< short >& thing,
                        const vector< Facet* >& vfac,
                        const AimsSurfaceTriangle& surface,
                        map< short, list< AimsSurfaceTriangle > >& splitted )
{
  size_t size = vfac.size();

  vector< bool > taken( size );

  Connectivity connect( thing.oLine(), thing.oSlice(),
                            Connectivity::CONNECTIVITY_6_XYZ );

  MapOfFacet all;
  MapOfFacet::iterator it_find;
  size_t	i;
  for ( i = 0; i < size; i++ )
    all[ vfac[ i ]->id() ] = vfac[ i ];

  Facet* facet = 0;
  Facet* currentFacet = 0;
  Facet* neighbor = 0;
  size_t nTrue = 0;
  int v;

  do
  {
    nTrue = 0;
    for ( i = 0; i < size; i++ )
      taken[ i ] = false;

    facet = ( Facet* )(all.begin())->second;
    short value = *( ( short* )facet->offset() + 
                     connect.offset( facet->type() ) );

    all.erase( all.begin() );

    MapOfFacet tmp;
    tmp[ facet->id() ] = facet;

    do
    {
      currentFacet = ( Facet* )(tmp.begin())->second;
      tmp.erase( tmp.begin() );
      taken[ currentFacet->id() ] = true;
      nTrue++;

      for ( v = 0; v < currentFacet->nNeighbor(); v++ )
      {
        neighbor = currentFacet->pNeighbor( v );
        if ( taken[ neighbor->id() ] == false )
        {
          short neighborValue = *( (short*)neighbor->offset() + 
                                   connect.offset( neighbor->type() ) );
          if ( neighborValue == value )
          { 
            tmp[ neighbor->id() ] = neighbor;
            if ( ( it_find = all.find( neighbor->id() ) ) != all.end() )
              all.erase( it_find );
          }
        }
      }
    }
    while ( tmp.size() );

    AimsSurfaceTriangle theSurface;
    splitted[ value ].push_back( theSurface );
    AimsSurfaceTriangle& current = splitted[ value ].back();

    current.vertex().reserve( nTrue );
    current.normal().reserve( nTrue );
    vector< uint > lut( size );
    uint n = 0;

    for ( i = 0; i < size; i++ )
      if ( taken[ i ] )
      {
        current.vertex().push_back( surface.vertex()[ i ] );
        current.normal().push_back( surface.normal()[ i ] );
        lut[ i ] = n++;
      }

    size_t nTriangle = 0;
    for ( i = 0; i < surface.polygon().size(); i++ )
      if ( taken[ surface.polygon()[ i ][ 0 ] ] &&
           taken[ surface.polygon()[ i ][ 1 ] ] &&
           taken[ surface.polygon()[ i ][ 2 ] ]    )
        nTriangle++;
    current.polygon().reserve( nTriangle );

    AimsVector< uint, 3 > tri;
    for ( i = 0; i < surface.polygon().size(); i++ )
      if ( taken[ surface.polygon()[ i ][ 0 ] ] &&
           taken[ surface.polygon()[ i ][ 1 ] ] &&
           taken[ surface.polygon()[ i ][ 2 ] ]    )
      {
        tri[ 0 ] = lut[ surface.polygon()[ i ][ 0 ] ];
        tri[ 1 ] = lut[ surface.polygon()[ i ][ 1 ] ];
        tri[ 2 ] = lut[ surface.polygon()[ i ][ 2 ] ];
        current.polygon().push_back( tri );
      }
  }
  while ( all.size() );
}
