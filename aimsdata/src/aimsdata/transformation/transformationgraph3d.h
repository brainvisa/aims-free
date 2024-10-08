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
#include <cartobase/allocator/allocator.h>


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

    /** Update the internal UUIDs cache. Needs to be done after changes in
        elements UUIDs or insertion / deletions
    */
    void updateIds();
    /** Get the vertex (referential) with given ID.

        The id may be a referential ID or name, or a code identifying a
        particular referential, like:
        - "mni", or "mni152", or "NIFTI_XFORM_MNI_152" or
          "Talairach-MNI template-SPM"
          (StandardReferentials::mniTemplateReferential()), to designate the
          ICBM152 standard referential
        - "acpc", or "talairach", or "NIFTI_XFORM_TALAIRACH", or
          "Talairach-AC/PC-Anatomist"
          (StandardReferentials::acPcReferential()), to designate AIMS
          Talairach referential
    */
    Vertex* referentialById( const std::string & id ) const;
    /** Same as referentialById( const string & id ), but with the help of
        an object header, can look for additional codes:

        - "AIMS": the internal AIMS referential of the object
        - "first": first referential in the transformations destinations in the
          given header
        - "last": last referential in the header
        - a numpber or lexical number as string to designate the referential in
          referenced transformations destinations in the object header        -
          "auto": last referential in header, if one is present, or fallback to
          the internal AIMS referential of the object otherwise.
        - "aligned", or "NIFTI_XFORM_ALIGNED_ANAT", or
          "Coordinates aligned to another file or to anatomical truth", to
          designate a transformation (NIFTI-like) going to another object space
          as described this way
        - "NIFTI_XFORM_TEMPLATE_OTHER" to designate a different template as
          described in the object header (NIFTI-like)
        - "disk": the "disk space" orientation: voxels in the order they are
          stored on disk. Some software are working in this referential. Note
          that this referential is only available in the graph if the object
          header has been included using updateFromObjectHeader() with the
          parameter includeDiskSpace set to true.
        - other codes may be supported later, like "qform", "sform", "ITK",
          "ANTS", but are not implemented yet.

        The object header should have been included in the transformations
        graph using updateFromObjectHeader() before calling this method.

        The object header is still needed here because looking for the
        specified transformations are linked tot the order the specific header
        and the order they are given in it.

        The additional parameter, refs, should be the list returned by
        updateFromObjectHeader(). It is the list of destinaton referentials in
        the object header, but with possibly different codes or IDs because the
        original header may use ambiguous, non-specific identifiers.

        If not found for a known reason, the function will throw a
        runtime_error exception with a message. Otherwise 0 might be returned.
    */
    Vertex* referentialByCode( const std::string & id,
                               carto::Object header,
                               const std::vector<std::string> & refs ) const;
    /** same as referentialByCode( const string, Object ) but with another way
        of passing the header */
    Vertex* referentialByCode( const std::string & id,
                               const carto::DictionaryInterface *header,
                               const std::vector<std::string> & refs ) const;
    /// Get the edge (transformation) with given ID
    Edge* transformationById( const std::string & id ) const;
    /// Get the referential ID in the Vertex (its uuid property)
    static std::string referential( const Vertex *v );
    /** Get the Transformation3d object inside an edge.
        The returned reference counting pointer may contain a null pointer
        (lazy loading is *not* performed in transformaton(), contrarily to
        getTransformation()).
    */
    static carto::rc_ptr<soma::Transformation3d> transformation(
      const Edge *edge );
    /** Get the Transformation3d object for the given UUID.
        The returned reference counting pointer may contain a null pointer
        (lazy loading is *not* performed in transformaton(), contrarily to
        getTransformation()).
    */
    carto::rc_ptr<soma::Transformation3d> transformation(
      const std::string & id ) const;

    /// Allocator context used in transformations reading operations.
    const carto::AllocatorContext & allocatorContext() const
    { return _allocator; }
    /** Set the allocator context used in all transformations reading
        operations.

        The allocator context may be used to push the use of memory mapping,
        for isntance.
    */
    void setAllocatorContext( const carto::AllocatorContext & alloc )
    { _allocator = alloc; }

    /** Get the transformation between source_ref and dst_ref. If it is not a
        registered transformation, return 0. Lazy loading will take place,
        however.
    */
    Edge* getTransformation( const std::string & src_ref,
                             const std::string & dst_ref ) const;
    /** Get the transformation between source_ref and dst_ref. If it is not a
        registered transformation, return 0. Lazy loading will take place.
        If allow_compose is True and the transformation is not found, then a
        transformations chain path is looked for. If a matching one is found,
        it is then registered in the graph to allow fast access later. The
        drawback of this register operation is that it cannot react to changes
        in the transformation chain (if a new transformation is added and could
        make a shorter path, it will not be taken into account).
    */
    Edge* getTransformation( const std::string & src_ref,
                             const std::string & dst_ref,
                             bool allow_compose );
    /** Get the transformation between source_ref and dst_ref. If it is not a
        registered transformation, return 0. Lazy loading will take place,
        however.
    */
    Edge* getTransformation( const Vertex *src_ref,
                             const Vertex *dst_ref ) const;
    /** Get the transformation between source_ref and dst_ref. If it is not a
        registered transformation, return 0. Lazy loading will take place.
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

    /// same as getTransformation() but don't perform lazy loading.
    Edge *getTransformation_raw( const Vertex *src_ref,
                                 const Vertex *dst_ref ) const;
    /// same as getTransformation() but don't perform lazy loading.
    Edge *getTransformation_raw( const std::string & src_ref,
                                 const std::string & dst_ref ) const;

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
    /** registers a different referential, additional UUID for an existing
        referential.

        If the new UUID corresponds to an existing referential also, both are
        merged into the same vertex.
    */
    void registerReferentialAlias( const std::string & uuid,
                                   const std::string & existing_id );
    /** Merge two existing referentials into a single one (the second).

        The first UUID will be kept as an alias in the second.
    */
    void mergeReferentials( const std::string & uuid,
                            const std::string & existing_id );

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

        Alternatively to file names, an affine transformation may be given
        directly as its 4x4 matrix, written as a line vector (all 16
        coefficients in a vecotr, describing the matrix horizontally). Ex:

            {
                source_ref_id: {
                    dest_ref_id1: [1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1],
                },
                ...
            }

        Or, a transformation may be a more complete description as a
        dictionary, containing an affine matrix as above, and possibly a
        header:

            {
                "source_ref": {
                    "dest_ref": {
                        "affine": [1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1],
                        "header": {
                            "free_field": "free value"
                        }
                    }
                }
            }

    */
    void loadTransformationsGraph( carto::Object desc,
                                   const std::string & dirname );
    /** convert to a dict-like structure compatible with
        loadTransformationsGraph()

        Options:
        - affine_only: if true, inclue only affine transformations
        - allow_read: if true, and if affine_only or embed_affines is also
          true, all affine transformations will be read in order to determine
          which are affine.
          Otherwise only those already loaded in memory (via the lazy reading
          mechanism) will be known to be affine. This parameter has no effect
          on non-affine transformations.
        - embed_affines: if true, affine transformations are not recorded as a
          file name (.trm format), but as an embedded 4x4 matrix.
        - rel_to_path: if not empty, filenames will be relative to this base
          directory (normally where the graph will be stored)
    */
    carto::Object asDict( bool affine_only = false, bool allow_read = false,
                          bool embed_affines = false,
                          const std::string & rel_to_path = "" ) const;
    /** remove deduced transformations (built from composition or inversion).

        If chain_only is true, then inverses of direct transforms are not
        removed.
    */
    void clearCache( bool chain_only=false );
    /** register inverse transformations when they can be obtained.

        This can be done only for loaded transformations. So the lazy loading
        feature will prevent for it to work on not already loaded
        transformations, since we cannot know if they are invertible before
        they are loaded.

        For this reason, the loadAffines parameter allows to load affine
        transforms and register their inverses.
    */
    void registerInverseTransformations( bool loadAffines = false );
    std::list<Edge *> loadAffineTransformations();

    /** Get a transformation chain between two vertices.

        You normally don't need to call this method directly, il is called
        through getTransformation( src_ref, dst_ref, true ). This method
        however does not modify the transformations graph and does not register
        the combined transformation.
        It will, however, load lazy-loading transformations.
    */
    carto::rc_ptr<soma::Transformation3d> getTransformChain(
      const Vertex *src_ref, const Vertex *dst_ref ) const;
    carto::rc_ptr<soma::Transformation3d> getTransformChain(
      const std::string & src_ref, const std::string & dst_ref ) const;

    carto::rc_ptr<soma::Transformation3d>
      loadTransformation( Edge *edge, bool affine_only=false ) const;

    /** Insert all transformations / referentials found in an object header.

        Returns the translated referentials names in the order they are in the
        object header, beginning with the object referential itself.
        For instance for a header with the properties:
        {
          "referential": "A",
          "referentials": ["B", "Talairach-MNI template-SPM",
                           "Scanner-based anatomical coordinates"],
          "transformations": [...]  // list of 3 transformations
        }

        it will return:
        ["A", "B", "803552a6-ac4d-491d-99f5-b938392b674b",
         "Scanner-based anatomical coordinates_A"]

        If the header does not contain a "referential" property, then a new
        UUID will be generated for it.

        If the optional parameter includeDiskSpace is set to true, then
        transformations to disk orientation (in mm) will also be included.

        The method may be called with a null header parameter, in which case a
        new referential UUID will be created and returned by the funciton.
    */
    std::vector<std::string> updateFromObjectHeader(
      carto::Object header, bool includeDiskSpace = false );
    /// Insert all transformations / referentials found in an object header
    std::vector<std::string>  updateFromObjectHeader(
      const carto::DictionaryInterface *header,
      bool includeDiskSpace = false );

    /** static helper function which extracts the transformation from the
        AIMS referential to disk space orientation of the image voxels, if
        such inforoamtion is present in the header. The returned transform is in mm (not in voxels).
    */
    static carto::rc_ptr<soma::Transformation3d> transformToDiskSpace(
      const carto::DictionaryInterface *header );

  private:
    mutable std::map<std::string, Vertex *> _refs_by_id;
    mutable std::map<std::string, Edge *> _tr_by_id;
    std::set<std::pair<const Vertex *, const Vertex *> > _disconnected;
    carto::AllocatorContext _allocator;

  };

}

namespace carto
{

  template <> inline
  std::string DataTypeCode<aims::TransformationGraph3d>::objectType()
  { return "TransformationGraph3d"; }

  template <> inline
  std::string DataTypeCode<aims::TransformationGraph3d>::dataType()
  { return "VOID"; }

  template <> inline
  std::string DataTypeCode<aims::TransformationGraph3d>::name()
  {
    return "TransformationGraph3d";
  }

  DECLARE_GENERIC_OBJECT_TYPE( carto::rc_ptr<soma::Transformation3d> )
}

#endif

