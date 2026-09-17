/*
 *  Texture class
 */
#ifndef AIMS_MESH_TEX2GRAPH_H
#define AIMS_MESH_TEX2GRAPH_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>

class Graph;


namespace aims
{

  template <class T>
  class Tex2Graph
  {
  public:
    Tex2Graph();
    virtual ~Tex2Graph();

    /// Fills graph \c g from texture of labels \c tex on the mesh \c mesh
    //void makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
    //		    const Texture<short> & tex, float epsilon = 0.01 );
    
    /*
      void makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
      const Texture<std::set<short> > & tex,
      const std::map <short,std::string> & label2name);
    */
    std::map<T,int>     label2index;

    void fillLabel2index(const Texture<T> &);
    int getIndex(const T & label); 

    /// Fills graph \c g from texture of labels \c tex on the mesh \c mesh
    void makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
                    const Texture<T> & tex, float epsilon = 0.01);
    
    void makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
                    const Texture<T> & tex,
		    const std::map<T,std::string> & lab2name, float epsilon = 0.01);
    
    
  };

}


#endif

