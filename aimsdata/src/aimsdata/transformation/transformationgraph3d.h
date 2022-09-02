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

#ifndef AIMS_TRANSFORMATION_TRANSFORMATIONGRAPH3D_H
#define AIMS_TRANSFORMATION_TRANSFORMATIONGRAPH3D_H

#include <graph/graph/graph.h>
#include <soma-io/transformation/transformation.h>


namespace aims
{

    /** Manage transformations (affine and non-linear) transformations graph.

    It maintains a graph of directed transformations (vertices are
    referentials, edges are 3D transformations).

    It can be populated from a dictionary (Object) which can be read from a
    JSON or YAML file, and performs lazy-loading: transformations are actually
    loaded when they are used, more precisely in getTransformation() and when
    building a chain of composed transformations.

    The graph is oriented, and does not manage inverse transforms. We could
    thing this could be done automatically for affine or other invertible
    transformations, but the lazy loading feature does not allow to know if a
    not yet loaded transform will be invertible or not. So this has to be done
    manually for now.

    The graph populates with composed transforms when they are used: indirect
    paths from a referential to another are generated when needed, and stored
    in the graph as "deduced" transforms. Later access will thus use this cache
    instead of reccomputing it.
    A drawback, thus, is that the graph has to be used "statically": any
    modification in the graph structure, or in any of its transformations,
    should invalidate a number of deduced, cached ones, which we do not handle
    for now (it's a quite difficult problem to manage it efficiently). To date
    the only solution is to completely clear the cached transforms
    (clearCache()) after any modification.

    The TransformationGraph3d structure is a Graph, and its elements are Vertex
    or Edge objects. To manipulate transformations, the API generally allows to
    use identifiers (generally UUIDs) for elements, and the Transformation
    objects are accessed through the "transformation" property of edges, or
    using the transformation() convenience method:

        Vertex *ref1 = tgraph.referentialById(
          "23df7ce8-e405-bc31-3863-d543e3cc89e5" );
        Edge *trans_edge = tgraph.getTransformation(
          "23df7ce8-e405-bc31-3863-d543e3cc89e5",
          "52929a27-6838-1d08-d6fc-7905ed9413a0" );
        if( trans_edge )
        {
          rc_ptr<Transformation3d> trans = tgraph.transformation( trans_edge );
        }

    see also:
    https://hbp-spatial-backend.apps.hbp.eu/swagger-ui#/api_v1/get_v1_transform_point
  */
  class TransformationGraph3d : public Graph
  {
  public:
    TransformationGraph3d();
    virtual ~TransformationGraph3d();

    void updateMaps();
    Vertex* referentialById( const std::string & id ) const;
    Edge* transformationById( const std::string & id ) const;
    static carto::rc_ptr<soma::Transformation3d> transformation(
      const Edge *edge );
    carto::rc_ptr<soma::Transformation3d> transformation(
      const std::string & id ) const;

    /** Get the transformation between source_ref and des_ref. If it is not a
        registered transformation, return 0.
    */
    Edge* getTransformation( const std::string & src_ref,
                             const std::string dst_ref ) const;
    /** Get the transformation between source_ref and des_ref. If it is not a
        registered transformation, return 0.
        If allow_compose is True and the transformation is not found, then a
        transformations chain path is looked for. If a matching one is found,
        it is then registered in the graph to allow fast access later. The
        drawback of this register operation is that it cannot react to changes
        in the transformation chain (if a new transformation is added and could
        make a shorter path, it will not be taken into account).
    */
    Edge* getTransformation( const std::string & src_ref,
                             const std::string dst_ref,
                             bool allow_compose );
    /** Get the transformation between source_ref and des_ref. If it is not a
        registered transformation, return 0.
    */
    Edge* getTransformation( const Vertex *src_ref,
                             const Vertex *dst_ref ) const;
    /** Get the transformation between source_ref and des_ref. If it is not a
        registered transformation, return 0.
        If allow_compose is True and the transformation is not found, then a
        transformations chain path is looked for. If a matching one is found,
        it is then registered in the graph to allow fast access later. The
        drawback of this register operation is that it cannot react to changes
        in the transformation chain (if a new transformation is added and could
        make a shorter path, it will not be taken into account).
    */
    Edge* getTransformation( Vertex *src_ref,
                             Vertex *dst_ref,
                             bool allow_compose );

    carto::rc_ptr<soma::Transformation3d> getTransformChain(
      const Vertex *src_ref, const Vertex *dst_ref ) const;
    carto::rc_ptr<soma::Transformation3d> getTransformChain(
      const std::string & src_ref, const std::string & dst_ref ) const;

    /** Add (or register) the given transformation in the transformations
        graph.

        transform may be a filename, or a Transformation instance. a
        Transformation is supposed to be a non-linear FFD (Free Form
        Deformation), not a linear transformation. If transform is a string, it
        may be:
        - an empty string will be an identity transformation
        - a .trm file will be an affine transformation, and will be passed to
          the builtin Anatomist system
        - another file (.ima) will be a FFD transformation file and will be
          registered as its filename. Loading will happen only when the
          transformation is used, in geTransformation().
    */
    Edge *registerTransformation( const std::string & src_ref,
                                  const std::string & dst_ref,
                                  carto::rc_ptr<soma::Transformation3d>,
                                  bool deduced=false );
    Edge *registerTransformation( Vertex *src_ref, Vertex *dst_ref,
                                  carto::rc_ptr<soma::Transformation3d>,
                                  bool deduced=false );
    Edge *registerTransformation( const std::string & src_ref,
                                  const std::string & dst_ref,
                                  const std::string & filename );
    Edge *registerTransformation( Vertex *src_ref, Vertex *dst_ref,
                                  const std::string & filename );

    /** Load a full transformations graph from a dict-like object.

        The description dict may typically be read from a .json or a .yaml
        file.

        The description is organized as a 2-level dictionary:

            {
                source_ref_id: {
                    dest_ref_id1: transformation_filename1,
                    dest_ref_id2: transformation_filename2,
                },
                ...
            }

        A referential ID is a string which may represent an UUID, or a name.

        A transformatioon filename may be an affine (.trm) or non-linear (.ima)
        transformation file, or an empty string to represent an identity
        transformation.

        The transformation files are found relatively to the directory given as
        the dirname parameter.
    */
    void loadTransformationsGraph( carto::Object desc,
                                   const std::string & dirname );
    /// remove deduced transformations (built from composition)
    void clearCache();
    /** register inverse transformations when thay can be obtained.

        This can be done only for loaded transformations. So the lazy loading
        feature will prevent for it to work on not already loaded
        transformations, since we cannot know if they are invertible before
        they are loaded.
    */
    void registerInverseTransformations();

  private:
    mutable std::map<std::string, Vertex *> _refs_by_id;
    mutable std::map<std::string, Edge *> _tr_by_id;
    std::set<std::pair<const Vertex *, const Vertex *> > _disconnected;

    carto::rc_ptr<soma::Transformation3d>
      loadTransformation( Edge *edge ) const;
  };

}

namespace carto
{
  DECLARE_GENERIC_OBJECT_TYPE( carto::rc_ptr<soma::Transformation3d> )
}

#endif

