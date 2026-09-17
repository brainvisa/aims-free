
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
  for ( ::byte i = 0; i < 3; i++ )
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
