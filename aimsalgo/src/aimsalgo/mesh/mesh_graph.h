/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_MESH_GRAPH_H
#define AIMS_MESH_GRAPH_H

#include <aims/vector/vector.h>
#include <cartobase/smart/rcptr.h>
#include <list>
#include <vector>

namespace aims
{

  namespace meshgraph
  {

    /** Mesh described as a graph, with nodes and edges. These are edges.
        Taken from Cathier's former aims-til library
    */
    template < typename TMeshVertexNode >
    struct MeshFaceNode
    {
      AimsVector<typename std::list<TMeshVertexNode>::iterator, 3> face;
    };


    /** Mesh described as a graph, with nodes and edges. These are nodes.
        Taken from Cathier's former aims-til library
    */
    template < typename TAttribute >
    class MeshVertexNode
    {
    public: // typedefs

      typedef MeshVertexNode<TAttribute> Self;
      typedef AimsVector<float,3> Vertex;
      typedef std::list<Self> VertexCollection;
      typedef typename VertexCollection::iterator VertexIndex;
      typedef std::list<VertexIndex> VertexIndexCollection;

      typedef MeshFaceNode<Self>                    Face;
      typedef std::list<Face>                       FaceCollection;
      typedef typename FaceCollection::iterator     FaceIndex;
      typedef std::list<FaceIndex>                  FaceIndexCollection;

    public: // set & get

      Vertex       & pos()       { return _pos; }
      Vertex const & pos() const { return _pos; }

      VertexIndexCollection       & neighbors()       { return _neighbors; }
      VertexIndexCollection const & neighbors() const { return _neighbors; }

      FaceIndexCollection       & faces()       { return _faces; }
      FaceIndexCollection const & faces() const { return _faces; }

      TAttribute       & attribute()       { return _attribute; }
      TAttribute const & attribute() const { return _attribute; }

    private: // data

      Vertex                  _pos;
      VertexIndexCollection   _neighbors;
      FaceIndexCollection     _faces;
      TAttribute              _attribute;
    };


    // we usually use these types
    typedef MeshVertexNode<uint> MeshGraphVertex;
    typedef MeshFaceNode<MeshGraphVertex> MeshGraphFace;
    typedef std::list<MeshGraphVertex> MeshGraphVertices;
    typedef std::list<MeshGraphFace> MeshGraphFaces;


      //---------------------------//
     //  List2GraphMeshConvertor  //
    //---------------------------//

    /** Converts a AimsSurface mesh into a
        (vertexList, faceList) mesh graph.
    */
    template < typename TVertexNode, typename TFaceNode,
               typename TVertexCollection, typename TFaceCollection,
               typename TNeighborCollection >
    class List2GraphMeshConvertor
    {
    public: // operators

      void operator()
      (
        TVertexCollection const & vertices,    ///< [input] mesh vertices
        TFaceCollection const & faceIndices,   ///< [input] mesh faces
        std::vector<std::list<std::size_t> > const & invertedFaceIndices,  ///< [input] inverted face indices
        TNeighborCollection const & neighbors, ///< [input] vertex neighborhoods
        std::list<TVertexNode> & graph_vertices,
        std::list<TFaceNode> & graph_faces
      );

    public: // set & get

    private: // data, internal
      std::vector<typename std::list<TVertexNode>::iterator>  _index_vertex;
      std::vector<typename std::list<TFaceNode>::iterator>    _index_face;
    };


      //---------------------------//
     //  Graph2ListMeshConvertor  //
    //---------------------------//

    template < typename TMesh >
    class Graph2ListMeshConvertor
    {
    public: // typedefs

      typedef std::map<const void*, std::size_t> VertexIndexMap;

    public: // set & get

      Graph2ListMeshConvertor( carto::rc_ptr<TMesh> mesh
                                 = carto::rc_ptr<TMesh>( 0 ) )
        : _mesh( mesh ? mesh : carto::rc_ptr<TMesh>( new TMesh ) ) {}

      VertexIndexMap & index_vertex() { return _index_vertex; }
      carto::rc_ptr<TMesh> & mesh() { return _mesh; }

    public: // operators

      template < typename TVertexNode, typename TFaceNode >
      void operator()
      (
        const std::list<TVertexNode> & graph_vertices,
        const std::list<TFaceNode> & graph_faces
      )
      {
          {
            _mesh->vertex().resize( graph_vertices.size() );
            std::size_t c = 0;
            for (typename std::list<TVertexNode>::const_iterator i = graph_vertices.begin(); i != graph_vertices.end(); ++i)
            {
              _mesh->vertex()[c] = i->pos();
              _index_vertex.insert(std::make_pair(static_cast<const void*>(&*i),c));
              ++c;
            }
          }
          {
            int c = 0;
            _mesh->polygon().resize( graph_faces.size() );
            for (typename std::list<TFaceNode>::const_iterator i = graph_faces.begin(); i != graph_faces.end(); ++i)
            {
              for (int j = 0; j < 3; ++j)
                _mesh->polygon()[c][j] = _index_vertex[static_cast<const void*>(&*(i->face[j]))];
              ++c;
            }
          }
          _mesh->normal().clear();
      }

    private: // data
      carto::rc_ptr<TMesh> _mesh;
      VertexIndexMap _index_vertex;
    };



    template < typename TIndexCollection >
    inline
    std::vector<std::list<std::size_t> >
    invertIndices( const std::vector<TIndexCollection> & c )
    {
      // Do a first pass to get maximum index
      typename TIndexCollection::value_type mx = 0;
      for (std::size_t i = 0; i < c.size(); ++i)
        for (std::size_t j = 0; j < c[i].size(); ++j)
          mx = std::max( c[i][j], mx );

      // allocate result
      std::vector<std::list<std::size_t> > res(mx+1);

      // Fill result
      for (std::size_t i = 0; i < c.size(); ++i)
        for (std::size_t j = 0; j < c[i].size(); ++j)
          res[c[i][j]].push_back(i);

      return res;
    }

    template < typename TFaceCollection >
    carto::rc_ptr<std::vector<std::vector<
      typename TFaceCollection::value_type::value_type> > >
    circular_neighborhoods( TFaceCollection const & faces,
                            std::size_t nVertices );

    //--  shortcuts  --//

    // TODO: this is wrong, because l2g is destroyed. EIther use smart pointers,
    // OR pass along the
    // reference to vertexOut/facesOut to l2g.

    template < typename TVerticesIn, typename TFacesIn, typename TNeighbors,
               typename TVertexNode, typename TFaceNode >
    inline
    void list2graph_mesh_conversion
    (
      const TVerticesIn & verticesIn,
      const TFacesIn & facesIn,
      const TNeighbors & neighc,
      std::list<TVertexNode> & verticesOut,
      std::list<TFaceNode> & facesOut
    )
    {
      std::vector<std::list<std::size_t> > faceIndices
        = invertIndices(facesIn);
      List2GraphMeshConvertor<TVertexNode, TFaceNode, TVerticesIn, TFacesIn,
                              TNeighbors> l2g;
      l2g( verticesIn, facesIn, faceIndices, neighc, verticesOut, facesOut );
    }


    template < typename TVerticesIn, typename TFacesIn, typename TVertexNode,
               typename TFaceNode >
    inline
    void list2graph_mesh_conversion
    (
      const TVerticesIn & verticesIn,
      const TFacesIn & facesIn,
      std::list<TVertexNode> & verticesOut,
      std::list<TFaceNode> & facesOut
    )
    {
      carto::rc_ptr<std::vector<std::vector<
        typename TFacesIn::value_type::value_type > > > neighc
        = circular_neighborhoods( facesIn, verticesIn.size() );
      list2graph_mesh_conversion( verticesIn, facesIn, *neighc, verticesOut,
                                  facesOut);
    }

  }

}

#endif

