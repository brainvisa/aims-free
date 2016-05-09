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
#include <iomanip>

using namespace std;

//
// Description of the indices of the facets :
//
//                            z
//                          __
//                           /|
//                          /
//                         /
//                        /1
//                       / ^         3
//                      /  |        /
//                     /   |       /
//                    +----+----------+
//                   /|    |     /   /|
//                  /      |        / |
//                 /  |    x   /   /  |
//                /           x   /   |
//               /    |          /    |               
//              +---------------+-----+----------------> x
//              |     |         |     |
//   0 <--------+--x    _  _  _ |_ x--+----------> 5
//              |     /         |     /
//              |       x       |    /
//              |   /  /    x   |   /
//              |     /     |   |  /
//              | /  /          | /
//              |   /       |   |/
//              +--/------------+
//              | /         |
//              |2          |
//              |           V
//              |           4
//              |
//              |
//              V
//
//              y
//


//
// 1) Tracks all the facets that belong to some interfaces
//
//    Structure of base :
//
//         +---+----------------------------------------+---+
//   mvec  |   |                                        |   |  vector of 
//         +---+----------------------------------------+---+  map ptrs
//            |                                            |
//            v                                            v
//       +---------+                                  +---------+
//       |         |  maps of facet pointers          |         |
//       +---------+                                  +---------+
//        |       |                                    |       |
//        v       v                                    v       v
//       +-+     +-+                                  +-+     +-+
//       | |     | |                                  | |     | |
//       | |     | |  facet pointers                  | |     | |
//       | |     | |                                  | |     | |
//       +-+     +-+                                  +-+     +-+
//
//
// 2) Builds adjacence graph of the facets
//
//    Structure of output : 
//
//           +---+----------------------------------------+---+
//    lmvec  |   |                                        |   |  vector of list
//           +---+----------------------------------------+---+  ptrs of map
//             |                                            |    ptrs
//             v                                            v
//        +---------+                                  +---------+
//        |         |  lists of map ptrs               |         |
//        +---------+                                  +---------+
//         |       |                                    |       |
//         |       +-------+                    +-------+       |
//         |               |    maps            |               |
//         v               v    of facet ptrs   v               v
//    +---------+     +---------+          +----------+    +----------+
//    |         |     |         |          |          |    |          |
//    +---------+     +---------+          +----------+    +----------+
//     |       |       |       |            |        |      |        |
//     v       v       v       v            v        v      v        v
//    +-+     +-+     +-+     +-+          +-+      +-+    +-+      +-+
//    | |     | |     | |     | |          | |      | |    | |      | |
//    | |     | |     | |     | | facet    | |      | |    | |      | |
//    | |     | |     | |     | | pointers | |      | |    | |      | |
//    +-+     +-+     +-+     +-+          +-+      +-+    +-+      +-+
//
//
void Mesher::getInterface( map< size_t, list< MapOfFacet > >& interface,
                           const AimsData<short>& thing )
{
  int x, y, z, f, v;
  int dimX = thing.dimX();
  int dimY = thing.dimY();
  int dimZ = thing.dimZ();

  ASSERT( thing.borderWidth() != 0 );
  ASSERT( thing[ -1 ] == -1 );

  AimsData<short>::const_iterator it;
  it = thing.begin() + thing.oFirstPoint();

  Facet* facet = NULL;
  map< size_t, MapOfFacet > mvec;

  Connectivity connect( thing.oLine(), thing.oSlice(),
                            Connectivity::CONNECTIVITY_6_XYZ );

  if( _verbose )
    cout << "reading slice      : " << setw( 3 ) << 0 << flush;
  for ( z = 0; z < dimZ; z++ )
  {
    if( _verbose )
      cout << "\b\b\b" << setw( 3 ) << z + 1 << flush;
    for ( y = 0; y < dimY; y++ )
    {
      for ( x = 0; x < dimX; x++ )
      {
        if ( *it )
	  {
	    MapOfFacet	& mf = mvec[ *it ];
	    for ( f = 0; f < connect.nbNeighbors(); f++ )
	      if ( *( it + connect.offset( f ) ) != *it )
		{
		  facet = new Facet;
		  facet->offset() = (short *)it; // ### mais qu'est-ce que c'est que ca ? ca marche vraiment ? code a revoir !
		  facet->type() = (byte)f;
		  facet->location() = Point3d( x, y, z );
		  facet->id() = facet->key();
		  mf[ facet->id() ] = facet;
		}
	  }
        it++;
      }
      it += thing.oPointBetweenLine();
    }
    it += thing.oLineBetweenSlice();
  }
  if( _verbose )
    cout << endl;


  Facet *currentFacet, *pNeighbor[ 4 ];


  map< size_t, MapOfFacet >::iterator	imf, emf = mvec.end();
  MapOfFacet tmp;
  MapOfFacet::iterator it_find;

  if( _verbose )
    cout << "getting interface  : " << setw(5) << 0 << flush;
  for ( imf=mvec.begin(); imf!=emf; ++imf )
    if ( !imf->second.empty() )
    {
      list< MapOfFacet >	& lmf = interface[ imf->first ];
      MapOfFacet		& mf = imf->second;
      if( _verbose )
        cout << "\b\b\b\b\b" << setw(5) << imf->first << flush;
      do
      {
        MapOfFacet mof;
        lmf.push_back( mof );
        MapOfFacet& current = lmf.back();

        facet = (Facet*)(mf.begin())->second;
        mf.erase( mf.begin() );
        tmp[ facet->id() ] = facet;
        do
        { 
          currentFacet = (Facet*)(tmp.begin())->second;
          tmp.erase( tmp.begin() );
          current[ currentFacet->id() ] = currentFacet;

          ASSERT( currentFacet->getNeighbor( connect, pNeighbor ) == 4 );

          for ( v = 0; v < 4; v++ )
          {
            if ( ( it_find = mf.find( pNeighbor[ v ]->id() ) ) !=
                 mf.end() )
            {
              facet = (Facet*)it_find->second;
              delete pNeighbor[ v ];
              pNeighbor[ v ] = facet;
              tmp[ facet->id() ] = facet;
              mf.erase( it_find );
            }
            else if ( ( it_find = current.find( pNeighbor[ v ]->id() ) ) !=
                      current.end() ) 
            {
              facet = (Facet*)it_find->second;
              delete pNeighbor[ v ];
              pNeighbor[ v ] = facet;
            }
            else if ( ( it_find = tmp.find( pNeighbor[ v ]->id() ) ) !=
                      tmp.end() )
            {
              facet = (Facet*)it_find->second;
              delete pNeighbor[ v ];
              pNeighbor[ v ] = facet;
            }
          }

          currentFacet->setNeighbor( pNeighbor, 4 );

        }
        while ( tmp.size() );
      }
      while ( !mf.empty() );
    }

  if( _verbose )
    cout << "\b\b\b\b\b" << "done " << endl;
}


//  
// Clear interface
//
void Mesher::clear( map< size_t, list< MapOfFacet > >& interface )
{
  if( _verbose )
    cout << "clearing interface : " << flush;

  list< MapOfFacet >::iterator i;
  MapOfFacet::iterator j;
  map< size_t, list< MapOfFacet > >::iterator	ii;

  while ( !interface.empty() )
  {
    ii = interface.begin();
    list< MapOfFacet >	& lf = ii->second;
    for ( i = lf.begin(); i != lf.end(); i++ )
    {
      for ( j = i->begin(); j != i->end(); j++ )
        delete (Facet*)j->second;
      i->clear();
    }
    interface.erase( ii );
  }
  if( _verbose )
    cout << "done" << endl;
}
