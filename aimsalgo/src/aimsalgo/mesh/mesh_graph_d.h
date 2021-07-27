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


#ifndef AIMS_MESH_MESH_GRAPH_D_H
#define AIMS_MESH_MESH_GRAPH_D_H

#include <aims/mesh/mesh_graph.h>
#include <aims/utility/cyclic_iterator.h>

namespace aims
{

  namespace meshgraph
  {

    template < typename TFaceCollection >
    carto::rc_ptr<std::vector<std::vector<
      typename TFaceCollection::value_type::value_type> > >
    circular_neighborhoods( TFaceCollection const & faces,
                            std::size_t nVertices )
    {
      typedef typename TFaceCollection::value_type::value_type VertexIndex;
      typedef std::list<VertexIndex> Neighborhood;

      // Allocate the result -- it should have as many elements as
      // the number of vertices of the mesh.
      std::vector<Neighborhood> res(nVertices);
      // To check if the circular neighbor of a point is finished
      // NB: we don't use a vector<bool> as it is apparently quite weird...
      std::vector<unsigned char> isFinished(nVertices, 0);
      // To check if everything is finished
      bool allFinished = false;

      // NB: the strategy here is quite dumb; basically, we loop through all triangles but will discard a neighbor
      // if it is not the one that fits at the end of our chain. Thus, we have to loop again and again through
      // this triangle list -- up to the maximum number of neighbors per point, that is, something like 14 times
      // for brain meshes. Plus, during the last pass, nothing should change, i.e. we'll do an extra pass for
      // nothing!

      while (!allFinished)
      {
        allFinished = true;

        // for all faces
        for (typename TFaceCollection::const_iterator iFic = faces.begin(); iFic != faces.end(); ++iFic)
        {
          // for all couple of points on the face
          typename TFaceCollection::value_type::const_iterator iFi1 = iFic->begin();
          const_cyclic_iterator<typename TFaceCollection::value_type> iFi2(*iFic, iFi1+1);

          // NB: We decide that iFi2 is the main point, and iFi1 is the neighbor of iFi2 we want to add.
          // There is a reason why we do not also consider the reverse couple at the same time: this is to
          // ensure that all circular neighbors are turning in the same order, if the triangles do so.
          for (;iFi1 != iFic->end(); ++iFi1, ++iFi2)
          {
            std::size_t i = *iFi2;

            // Skip voxel if its neighborhood is already done
            if( !isFinished[i] )
            {

              // Check size of the neighborhood
              if (res[i].size() == 0)
              {
                // This is the first neighbor added: no need to check anything
                res[i].push_back(*iFi1);
                allFinished = false;
              }
              else
              {
                // We add the neighbor only if it is the "next" neighbor of the chain. That means
                // that the last point of the neighbor chain has to be in the current face as well.
                // Note also that we have to end as well: if this next neighbor also happens to be
                // the first of the chain, we mark this voxel neighborhood as done.

                // Loop through all face vertices
                VertexIndex lastPoint = res[i].back();
                if( *iFi1 != lastPoint ) // otherwise already done this one
                {

                  typename TFaceCollection::value_type::const_iterator
                    iFaceVertex = iFic->begin();
                  for( ; iFaceVertex != iFic->end(); ++iFaceVertex )
                  {
                    if (*iFaceVertex == lastPoint)
                    {
                      // Check that the point we are about to add is not
                      // actually the first point of the circular neighborhood.
                      if (*iFi1 == res[i].front())
                      {
                        isFinished[i] = 1;
                      }
                      else if( *iFi1 != lastPoint )
                      {
                        res[i].push_back(*iFi1);
                        allFinished = false;
                      }
                      break;
                    }
                  }
                }
              }
            }

            // and the other way
            i = *iFi1;

            // Skip voxel if its neighborhood is already done
            if( !isFinished[i] )
            {

              // Check size of the neighborhood
              if (res[i].size() == 0)
              {
                // This is the first neighbor added: no need to check anything
                res[i].insert( res[i].begin(), *iFi2 );
                allFinished = false;
              }
              else
              {
                // We add the neighbor only if it is the "first" neighbor of the chain. That means
                // that the first point of the neighbor chain has to be in the current face as well.
                // Note also that we have to end as well: if this first neighbor also happens to be
                // the last of the chain, we mark this voxel neighborhood as done.

                // Loop through all face vertices
                VertexIndex firstPoint = res[i].front();
                if( *iFi2 != firstPoint ) // otherwise already done this one
                {

                  typename TFaceCollection::value_type::const_iterator
                    iFaceVertex = iFic->begin();
                  for( ; iFaceVertex != iFic->end(); ++iFaceVertex )
                  {
                    if (*iFaceVertex == firstPoint)
                    {
                      // Check that the point we are about to add is not
                      // actually the last point of the circular neighborhood.
                      if (*iFi2 == res[i].back())
                      {
                        isFinished[i] = 1;
                      }
                      else if( *iFi2 != firstPoint )
                      {
                        res[i].insert( res[i].begin(), *iFi2 );
                        allFinished = false;
                      }
                      break;
                    }
                  }
                }
              }
            }

          }
        }
      }

      //convert the result into a vector
      carto::rc_ptr<std::vector<std::vector<VertexIndex> > >
        res2(new std::vector<std::vector<VertexIndex> >( res.size()) );

      {
        for (std::size_t i = 0; i < res.size(); ++i)
        {
          std::vector<VertexIndex> & r2 = (*res2)[i];
          r2.resize( res[i].size() );
          size_t j = 0;
          typename std::list<VertexIndex>::iterator il, el = res[i].end();
          for( il=res[i].begin(); il!=el; ++il, ++j )
            r2[j] = *il;
        }
      }
      return res2;
    }


    //-------------------------------------------------------------------------

    template < typename TVertexNode, typename TFaceNode, typename TVertexCollection, typename TFaceCollection, typename TNeighborCollection >
    void List2GraphMeshConvertor<TVertexNode, TFaceNode, TVertexCollection, TFaceCollection, TNeighborCollection>::
    operator()
    (
        TVertexCollection                     const & vertices,             ///< [input] mesh vertices
        TFaceCollection                       const & faceIndices,          ///< [input] mesh faces
        std::vector<std::list<std::size_t> >  const & invertedFaceIndices,  ///< [input] inverted face indices
        TNeighborCollection                   const & neighbors,            ///< [input] vertex neighborhoods
        std::list<TVertexNode>                      & graph_vertices,
        std::list<TFaceNode>                        & graph_faces
    )
    {
      if( vertices.size() != invertedFaceIndices.size() )
        throw std::runtime_error(
          "vertices and invertedFaceIndices sizes differ" );
      if( vertices.size() != neighbors.size() )
        throw std::runtime_error( "vertices and neighbors sizes differ" );

      _index_vertex.resize(vertices.size());
      _index_face.resize(faceIndices.size());
      {
        // push all vertices into the vertex list and get an index2iterator translation
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
          // create a node with current position
          TVertexNode m;
          m.pos() = vertices[i];
          // push this node at the end of the vertex list
          _index_vertex[i] = graph_vertices.insert(graph_vertices.end(), m);
        }

        // now push all faces into the face list
        for (std::size_t i = 0; i < faceIndices.size(); ++i)
        {
          // create a face with current face indices
          TFaceNode f;
          for (int j = 0; j < 3; ++j)
          {
            if( vertices.size() <= faceIndices[i][j] )
              throw std::runtime_error(
                "vertices size smaller than used faceIndices" );
            f.face[j] = _index_vertex[faceIndices[i][j]];
          }
          // push this face at the end of the face list
          _index_face[i] = graph_faces.insert(graph_faces.end(), f);
        }

        // now we need a second pass on the vertices to fill in the neighbor
        // and the faces structure.
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
          for (std::list<std::size_t>::const_iterator j = invertedFaceIndices[i].begin(); j != invertedFaceIndices[i].end(); ++j)
          {
            if( faceIndices.size() <= *j )
              throw std::runtime_error(
                "faceIndices size smaller than used invertedFaceIndices" );
            _index_vertex[i]->faces().push_back(_index_face[*j]);
          }
          for (typename TNeighborCollection::value_type::const_iterator iN = neighbors[i].begin();
              iN != neighbors[i].end(); ++iN)
          {
            if( vertices.size() <= *iN )
              throw std::runtime_error(
                "vertices size smaller than used neighbors indices" );
            _index_vertex[i]->neighbors().push_back(_index_vertex[*iN]);
          }
        }
      }
    }

  }
}

#endif

