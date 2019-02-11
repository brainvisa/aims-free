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

/*
 *  Data reader class
 */
#ifndef AIMS_IO_ARGR_H
#define AIMS_IO_ARGR_H

#include <cartobase/object/syntax.h>
#include <aims/data/pheader.h>
#include <set>

class Graph;

namespace aims
{

  /// Wrapper for a specific GraphReader
  class LowLevelArgReader
  {
  public:
    LowLevelArgReader();
    virtual ~LowLevelArgReader();
    /// in case of failure, either raise an exception or return 0
    virtual Graph* read( const std::string & filename, 
                         int subobjectsfilter = -1 ) = 0;
    /** set / merge .minf header into the graph.

        This means set it into a graph property (typically "header"), but
        also update some graph properties according to the contents of this
        header.

        The default implementation just copies the minf properties into a
        dictionary stored in the "header" property of the graph.
    */
    virtual void mergeMinf( Graph & g, const PythonHeader & hdr );

  protected:
    /** Store referentials, transformations,
        and update the Talairach transformation in the graph, if such
        information is present in the minf header.

        This static method is not called by LowLevelArgReader::mergeMinf()
        by default, this method is just provided so it can be called by
        concrete implementations of LowLevelArgReader (such as
        LowLevelStandardArgReader).
    */
    static void mergeTransformations( Graph & g, const PythonHeader & hdr );
  };

  /* This class is a wrapper for several GraphReaders: standard GraphReader, 
     FoldReader, FRGReader (defined in sigraph library), RoiArgReader (defined
     in aimsalgo), etc. New readers can be added as plugins.
   */
  class ArgReader
  {
  public:
    ArgReader( const std::string& filename );
    ~ArgReader();
    Graph *read( int subobjectsfilter = -1 ) const;
    /** sets the reader to use for graphs of a specific syntax. The reader 
	then belongs to ArgReader (never delete it) */
    static void registerReader( const std::string & syntax, 
				LowLevelArgReader *reader );
    static bool hasReader( const std::string & syntax );
    /// removes and deletes a reader
    static void deleteReader( const std::string & syntax );
    static std::set<std::string> readers();
    /// called once from the main thread first at init time.
    static void initLowLevelReaders();

  private:
    struct StaticPrivate;
    static StaticPrivate* staticPrivate();

    std::string	_name;
  };

  /// Wrapper for the standard GraphReader
  class LowLevelStandardArgReader : public LowLevelArgReader
  {
  public:
    LowLevelStandardArgReader();
    virtual ~LowLevelStandardArgReader();
    virtual Graph* read( const std::string & filename, 
                         int subobjectsfilter = -1 );

  protected:
    carto::SyntaxSet	*_syntax;
  };

}

#endif
