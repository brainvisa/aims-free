
#ifndef AIMS_TOPOLOGY_CC26ADJACENT_H
#define AIMS_TOPOLOGY_CC26ADJACENT_H

#include <aims/topology/adjacency.h>


class CC26Adjacent : public Adjacency
{
public:

  CC26Adjacent() : Adjacency()  { _deb = 1; _fin = 26; }
};

#endif
