
#ifndef AIMS_TOPOLOGY_ADJACENCY_H
#define AIMS_TOPOLOGY_ADJACENCY_H


class Adjacency
{
public:

  Adjacency() : _deb( 0 ), _fin( 0 )  { }
  virtual ~Adjacency()  { }

  int begin() const { return _deb; }
  int end() const { return _fin; }

protected:

  int _deb;
  int _fin;
};

#endif
