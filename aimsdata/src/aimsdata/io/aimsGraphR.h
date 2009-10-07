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

#ifndef AIMS_IO_AIMSGRAPHR_H
#define AIMS_IO_AIMSGRAPHR_H

#include <aims/io/process.h>
#include <aims/bucket/bucket.h>
#include <cartobase/object/attributed.h>
#include <set>

class Graph;

namespace aims
{

  namespace internal
  {
    struct AimsGraphReader_Private;
    struct AimsGraphReader_ElemStruct;
  }

  /**	Reader / postprocessor for graph reading

	Reads Aims objects referred in the input graph and appropriately stores
	them in the graph vertices / edges
  */
  class AimsGraphReader : public Process
  {
  public:
    friend struct internal::AimsGraphReader_Private;

    struct Wrapper
    {
      friend class AimsGraphReader;

      virtual ~Wrapper() {}
      virtual void destroy() {}

      virtual std::string objectType() const = 0;
      virtual std::string dataType() const = 0;

    private:
      virtual Wrapper *extract( int index ) = 0;
    };

    template<typename T> struct ObjectWrapper : public Wrapper
    {
      ObjectWrapper( T* x ) : data( x ) {}
      virtual ~ObjectWrapper() {}
      virtual void destroy() { delete data; }

      virtual std::string objectType() const;
      virtual std::string dataType() const;

      T	*data;

    private:
      virtual Wrapper *extract( int index );
    };

    struct ElementInfo
    {
      Graph			*graph;
      carto::AttributedObject	*element;
      std::string		attribute;
      Wrapper			*object;
    };

    class PostProcessor : public Process
    {
    public:
      friend class AimsGraphReader;
      PostProcessor();
      virtual ~PostProcessor();
      const ElementInfo & elementInfo() const { return( info ); }
      ElementInfo & elementInfo() { return( info ); }

    private:
      ElementInfo	info;
    };

    AimsGraphReader( const std::string & filename );
    virtual ~AimsGraphReader();

    /**	Reads internal objects and stores them in the graph.

	Objects are stored by an insertion function that you can change by 
	setInsertionFunction, the default function stores reference counting 
	pointers to the Aims objects in graph elements attributes.

	Some tables are stored in the graph to help finding the loaded objects 
	in the graph elements. These tables are global graph attributes:
	- \c object_attributes_colors of type \c 
	  rc_ptr<map<string, vector<int> > > is only used in graphical 
	  applications (Anatomist...) and affects a color to an attribute 
	  (attributes in graph elements)

	Another table describes how things are actually loaded and stored in 
	the graph:
	- \c aims_objects_table of type 
	  \c rc_ptr<GraphElementTable> 
	  ( map<string,map<string,GraphElementCode> > ) gives the 
	  correspondance for each syntactic type of an identifier and a 
	  decriptor of Aims object. The identifier is made from the attribute 
	  name used in the graph elements and \e tries to be the same if the 
	  graph has been saved in "global objects mode" and in "local objects 
	  mode". To do so, any \c _filename or \c _label suffixes are removed 
	  from the attribute name as these are the suffixes used resp. in 
	  local and global saving mode

        The \c mask parameter specifies which elements have to be read: 
        - 0: none
        - 1: nodes
        - 2: relations
        - 3: both
     */
    virtual void readElements( Graph & g, int mask = 1 );
    /**	Filters objects IDs in the graph file to be read as Aims objects. 
	Only objects listed here will be read. These IDs are those used in 
	the \c aims_objects_table attribute (see readElements) ans correspond 
	to attributes describing a file (in local objects mode) or an index in 
	a global object, with the suffix \c "_filename" or \c "_index" removed.
	Ex: \c "ss" for \c "ss_filename" (in cortical folds graphs, local 
	mode) or \c "roi" for \c "roi_label" (in ROI graphs, global mode).

	setReadFilter and setExcludeFilter are mututally exclusive: when you 
	use setReadFilter, exclusion filters are erased and vice versa
    */
    void setReadFilter( const std::set<std::string> & toread );
    /**	Filters objects IDs in the graph file to be read as Aims objects. 
	Objects listed here will \b NOT be read. setReadFilter and 
	setExcludeFilter are mututally exclusive: when you use 
	setExcludeFilter, read filters are erased and vice versa
    */
    void setExcludeFilter( const std::set<std::string> & toexclude );
    /**	Registers a post-processing function.
	After loading an Aims object, it has to be inserted in the graph 
	element (node, edge). The default behaviour is to insert a 
	ref-counting pointer to it in the attribute like described in 
	readElements() for the \c aims_objects_table graph attribute. 
	If you need to do something else with the new objects, you can use 
	your own function instead.

	The registration procedure is exactly like the 
	Process::registerProcessType mechanism: in fact we use a hidden 
	Process (PostProcessor) to do so.

	You can access current graph, graph element and data via the 
	PostProcessor::elementInfo() structure
     */
    void setInsertionFunction( const std::string & objType, 
			       const std::string & dataType, 
			       ProcFunc procFunc );
    /** Aims object reading function - called by the Process mechanism. 
	By default only BucketMap<Void> and AimsSurfaceTriangle objects 
	are read. If you need other types, use the registerProcessType function
	with appropriate types. In this case, you'll have to include 
	\c <aims/io/aimsGraphR_d.h> to allow the template functions to be 
	compiled on the new types.

	You never need to call this function, but you shoud register its 
	variant on new types by calling registerProcessType() and passing a 
	pointer to it
    */
    template<class T> static 
    bool read( Process &, const std::string &, Finder & );

    /// convenient function to load everything that can be loaded.
    static void loadAllMissingElements( Graph & );

  private:
    template<class T> static 
    bool defaultInsertFunction( Process &, const std::string &, Finder & );
    virtual void readElement( carto::AttributedObject* ao, 
			      internal::AimsGraphReader_ElemStruct & es );

    internal::AimsGraphReader_Private	*_priv;
  };

}


#endif

