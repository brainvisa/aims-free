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

#ifndef AIMS_GRAPH_GRAPHMANIP_H
#define AIMS_GRAPH_GRAPHMANIP_H

#include <aims/rgb/rgb.h>
#include <cartodata/volume/volume.h>
#include <cartobase/object/object.h>
#include <cartobase/type/types.h>
#include <cartobase/object/property.h>

class Graph;
class GraphObject;
namespace aims
{
  class AffineTransformation3d;
}
class Void;

namespace aims
{

  template<typename T> class BucketMap;

  /**	Describe how Aims objects are stored in graph objects.

	These codes are built by AimsGraphReader and stored in the graph 
	global tables. They must be maintained otherwise Aims objects 
	stored in graphs won't be saved correctly
	\sa AimsGraphReader, GraphManip
   */
  struct GraphElementCode
  {
    enum StorageType
      {
	Local,
	Global,
	GlobalPacked
      };

    GraphElementCode() {}
    GraphElementCode( const GraphElementCode & x );
    GraphElementCode( const std::string & attrib, const std::string & ot, 
		      const std::string & dt, StorageType st );
    GraphElementCode & operator = ( const GraphElementCode & x );

    /// ID for object: filename (globals) or attribute for filename (locals)
    std::string	id;
    /** attribute found in graph objects, this attribute is a
	reference-counting pointer (carto::rc_ptr<T>) to the correct aims 
	object type */
    std::string	attribute;
    /// object type as in aims Finder: "Volume", "Bucket", "Mesh", ...
    std::string	objectType;
    /// data type as in Aims Finder: "S16", "U8", "FLOAT", ...
    std::string	dataType;
    StorageType	storageType;
    std::string	local_file_attribute;
    std::string global_index_attribute;
    std::string global_filename;
    std::string global_attribute;
    ///	syntactic attribute of attributed objects
    std::string	syntax;
  };

  /** GraphElementCode table, stored in graphs in attribute 
      "aims_objects_table" (carto::rc_ptr<GraphElementTable>) */
  typedef std::map<std::string, std::map<std::string,GraphElementCode> > 
  GraphElementTable;


  namespace DataManip
  {

    /** utility function to copy voxel sizes from an object to another 
	(if applicable) */
    template <typename T> void adjustVoxelSize( T &, const T & );

    /** insert a sub-element in a container data (time bucket/mesh/texture, 
	or volume of labels...) */
    template <typename T> 
    void insertElement( T & dest, int index, const T & src );
    template <typename T>
    carto::PropertySet & getHeader( T & obj );
    template <typename T>
    void setHeaderInfo( T & obj, const carto::GenericObject & srcheader );
  }


  /** Various manipulations on graphs */
  class GraphManip
  {
  public:
    typedef BucketMap<Void>* 
    (*CreateBucketFunc)( GraphObject*, const std::string & );

    /** Extract Talairach transformation (graph -> Talairach)

        The transformation is returned from one of these sources, in order of
        priority:

        1. The "transformations" attribute, corresponding to the
           "Talairach-AC/PC-Anatomist" referential.

        2. The legacy "Talairach_translation", "Talairach_rotation", and
           "Talairach_scale" attributes.

        3. Otherwise, identity is returned.
     */
    static AffineTransformation3d talairach( const Graph & g );
    /** Store Talairach transformation in graph

        The transformation is written in the modern attributes ("referentials"
        and "transformations"). It is also written to the legacy attributes
        ("Talairach_translation", "Talairach_rotation", and "Talairach_scale"),
        if these are already present, or if force_old_attributes is true.
     */
    static void storeTalairach( Graph & g, const AffineTransformation3d & m,
                                bool force_old_attributes=false);
    /** Test if a Graph has the old Talairach attributes

        These attributes are "Talairach_translation", "Talairach_rotation"
        (both required), and "Talairach_scale" (optional). The modern way of
        storing the transformation to Talairach is in the "referentials" and
        "transformations" attributes.
     */
    static bool hasOldTalairachTransform( const Graph & g );
    /** Extract the transformation to the MNI ICBM152 space. This space has
        a fixed transformation with our Aims Talairach.
    */
    static AffineTransformation3d getICBMTransform( const Graph & g );
    /**   Extract the transformation to the MNI ICBM152 space, shifted to the
          "most standard" field of view of the template image, the one from the
          ICBM2009c_nlin_asym template from the MNI. Actually we find various
          fields of view for the templates. The one we use here is
          (193, 229, 193).
          The transform to the "real" ICBM space (0 roughly at AC) is provided
          in the output transformation header.
          This space has a fixed transformation with our Aims Talairach, along
          with the template volume size and voxel size.
          Includes shifts and axes inversions.
      */
    static AffineTransformation3d
      getICBM2009cTemplateTransform( const Graph & g );
    /** converts internal representations of graph nodes from buckets to
        volume
        \return false if it fails: if all buckets have not been read from disk
    */
    static bool buckets2Volume( Graph & g );
    /** converts internal representations of graph nodes from volume to 
        buckets.
        \return false if it fails: if all volumes have not been read from disk
    */
    static bool volume2Buckets( Graph & g, CreateBucketFunc f = 0 );
    static GraphElementCode & 
    graphElementCode( Graph &, const std::string & syntax, 
		      const std::string & id );
    static GraphElementCode & 
    graphElementCodeByAtt( Graph &, const std::string & syntax, 
			   const std::string & attrib );
    /** This template function stores the given object in the given 
        Vertex / Edge of the graph and takes care of IO information to 
        maintain in the graph.
        More precisely, it calls GraphObject::setProperty and fills 
        the GraphElementTable accordingly in the graph */
    template<typename T> 
    static void storeAims( Graph & graph, GraphObject* vertex, 
                           const std::string & attribute, 
                           carto::rc_ptr<T> obj );
    /// builds a ROI graph from a volume of labels
    template <typename T>
    static Graph* graphFromVolume( const carto::VolumeRef<T> & vol,
                                   T background = 0,
                                   std::map<T, std::string> *trans = 0 );
    /// builds a ROI graph from a volume of labels
    template <typename T>
    static void graphFromVolume( const carto::VolumeRef<T> & vol , Graph & g,
                                 T background = 0,
                                 std::map<T,std::string> *trans = 0,
                                 bool automaticBackgroundSearch = true );
    static void setAttributeColor( Graph & graph, const std::string & att,
                                   const AimsRGB & );
    static void setAttributeColor( Graph & graph, const std::string & att,
                                   const AimsRGBA & );
    static void setAttributeColor( Graph & graph, const std::string & att,
                                   const std::vector<int> & );
    std::vector<int> attributeColor( const Graph & graph,
                                     const std::string & att );
    /** Completes folds graph information.
	Adds missing information to sulcal nodes, like meshes surface...
    */
    static void completeGraph( Graph & );

    //Merge two graph
    //If they contain some mesh, they can be 
    //either merged 
    //or just copied
    //invNormal enable to inverse the mesh normal
    static Graph* mergeGraph( const std::string &,  Graph &,  Graph &,
			       bool mergeMesh = true, bool invNormal = false);
    static void printGraphElementTable( const Graph &,
                                        std::ostream & ostr = std::cout );
    static std::string
      defaultExtensionForObjectType( const std::string & otype,
        const std::string & dtype );
  };

}


namespace carto
{

  template<> inline 
  std::string DataTypeCode<aims::GraphElementCode>::dataType()
  {
    return "GraphElementCode";
  }
  
#define _TMP_ rc_ptr<std::map<std::string,std::map<std::string,aims::GraphElementCode> > > 
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

} // namespace carto

#endif

