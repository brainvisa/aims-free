
#ifndef AIMS_TOPOLOGY_TOPOBASE_H
#define AIMS_TOPOLOGY_TOPOBASE_H

#include <aims/vector/vector.h>
#include <aims/topology/components18.h>
#include <aims/topology/components26.h>
#include <aims/topology/connectivity26.h>


class TopologyBase
{
public:

  TopologyBase();
  virtual ~TopologyBase();

  int Cstar() { return _cstar; }
  int Cbar()  { return _cbar; }

  int *X() { return myX; }

  void flipX();
  void computeLocalCCNumbers();

  virtual void fillNeighborhood( const Point3d&, int ) { }
  virtual void fillNeighborhood( const Point3d&, int, int ) { }
  virtual void fillNeighborhoodComplement( const Point3d&, int ) { }

protected:

  Connectivity26 connex26;

  Components26Neighborhood *_comp26;
  Components18Neighborhood *_comp18;

  int myX[ 27 ];

  int _cstar;
  int _cbar;
};

#endif
