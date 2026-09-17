// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/mesh/tex2graph_d.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/datatypecode.h>
#include <graph/graph/graph.h>
#include <stdio.h>

using namespace aims;
using namespace std;
using namespace carto;

template <class T>
Tex2Graph<T>::Tex2Graph()
{
  
}

template <class T>
Tex2Graph<T>::~Tex2Graph()
{
}

namespace aims 
{
  template class Tex2Graph<short>;
  template class Tex2Graph<std::set<short> >;

  //template
  //void Tex2Graph::makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
  //                           const Texture<short> & tex,
  //                           const std::map<short,std::string> & lab2name, float epsilon = 0.01);
  //template
  //void Tex2Graph::makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
  //                           const Texture<short> & tex, float epsilon = 0.01);
}
