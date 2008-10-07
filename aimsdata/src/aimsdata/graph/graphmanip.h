/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_GRAPH_GRAPHMANIP_H
#define AIMS_GRAPH_GRAPHMANIP_H

#include <cartobase/object/object.h>
#include <cartobase/type/types.h>
#include <cartobase/object/property.h>

class Graph;
class GraphObject;
class Motion;
class Void;
template<typename T> class AimsData;
class AimsRGB;
class AimsRGBA;

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

    /// extract Talairach transformation (graph -> Talairach)
    static Motion talairach( const Graph & g );
    /// stores Talairach transformation in graph
    static void storeTalairach( Graph & g, const Motion & m );
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
    static Graph* graphFromVolume( const AimsData<short> & vol, 
				   short background = 0, 
				   std::map<short,std::string> *trans = 0 );
    /// builds a ROI graph from a volume of labels
    static void graphFromVolume( const AimsData<short> & vol , Graph & g, 
				 short background = 0, 
				 std::map<short,std::string> *trans = 0,
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

