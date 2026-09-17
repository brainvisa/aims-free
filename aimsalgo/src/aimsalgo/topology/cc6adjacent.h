
#ifndef AIMS_TOPOLOGY_CC6ADJACENT_H
#define AIMS_TOPOLOGY_CC6ADJACENT_H

#include <aims/topology/adjacency.h>


class CC6Adjacent : public Adjacency
{
public:

  CC6Adjacent() : Adjacency()  { _deb = 1; _fin = 6; }
};

#endif
