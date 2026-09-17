
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/mesh/mesher.h>
using aims::Connectivity;

using namespace carto;
using namespace std;


void Mesher::splitting( const rc_ptr<Volume< short > > & thing,
                        const vector< Facet* >& vfac,
                        const AimsSurfaceTriangle& surface,
                        map< short, list< AimsSurfaceTriangle > >& splitted )
{
  size_t size = vfac.size();

  vector< bool > taken( size );

  Connectivity connect( thing->getStrides()[1], thing->getStrides()[2],
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
