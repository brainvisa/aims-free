
#ifndef AIMS_TOPOLOGY_CC18ADJACENT_H
#define AIMS_TOPOLOGY_CC18ADJACENT_H

#include <aims/topology/adjacency.h>


class CC18Adjacent : public Adjacency
{
public:

  CC18Adjacent() : Adjacency()  { _deb = 1; _fin = 18; }
};

#endif
