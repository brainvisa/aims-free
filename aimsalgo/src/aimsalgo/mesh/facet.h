
#ifndef AIMS_MESH_FACET_H
#define AIMS_MESH_FACET_H

#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>
#include <aims/vector/vector.h>
#include <aims/connectivity/connectivity.h>

class AIMSALGO_API Facet
{
  public:

    Facet();
    virtual ~Facet();

    uint key() const;
    bool equal( const Facet& other ) const;

    Point3d& location() { return _location; }
    const Point3d& location() const { return _location; }

    byte& type() { return _type; }
    byte type() const { return _type; }

    short*& offset() { return _offset; }
    short* offset() const { return _offset; }

    uint& id() { return _id; }
    uint id() const { return _id; }

    void setNeighbor( Facet** pNeighor, int nNeighbor );
    void setNeighbor( int nNeighbor );
    int getNeighbor( const aims::Connectivity& connect, 
		      Facet** pNeighor ) const;

    Facet*& pNeighbor( int index ) { return _pNeighbor[ index ]; } 
    const Facet*& pNeighbor( int index ) const 
      { return (const Facet *&)_pNeighbor[ index ]; }

    int nNeighbor() const { return _nNeighbor; }

  protected:

    Point3d _location;
    byte _type;

    int _nNeighbor;
    Facet** _pNeighbor;

    uint _id;
    short* _offset;
};


inline
Facet::Facet()
{
  _nNeighbor = 0;
  _pNeighbor = NULL;
}


inline
Facet::~Facet()
{
  if ( _pNeighbor )
    delete [] _pNeighbor;
}


inline
uint Facet::key() const
{
  return ( (uint)_type << 29 ) |
         ( 640000U * (uint)_location[ 2 ] +
              800U * (uint)_location[ 1 ] +
                      (uint)_location[ 0 ] ); 
}


inline
bool Facet::equal( const Facet& other ) const
{ 
  return _offset == other._offset && _type == other._type;
}


inline
void Facet::setNeighbor( Facet** pNeighbor, int nNeighbor )
{
  if ( _pNeighbor )
    delete [] _pNeighbor;

  _nNeighbor = nNeighbor;
  _pNeighbor = new Facet*[ nNeighbor ];

  memcpy( _pNeighbor, pNeighbor, nNeighbor * sizeof( Facet* ) );
}


inline
void Facet::setNeighbor( int nNeighbor )
{
  _nNeighbor = nNeighbor;
 
  if ( _pNeighbor )
    delete [] _pNeighbor;
  if ( nNeighbor > 0 )
    _pNeighbor = new Facet*[ nNeighbor ];
  else
    _pNeighbor = 0;
}


inline
std::ostream& operator << ( std::ostream& out, const Facet& thing )
{
  out << "{location=" << thing.location()
      << ", type=" << (int)thing.type()
      << ", offset=" << thing.offset()
      << ", id=" << thing.id()
      << ", nNeighbor=" << thing.nNeighbor()
      << "(";
  for ( int n = 0; n < thing.nNeighbor(); n++ )
    out << thing.pNeighbor( n ) << ", ";
  out << "NULL)}";

  return out;
}

#endif
