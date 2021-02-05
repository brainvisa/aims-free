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
 *  Data writer class
 */
#ifndef AIMS_IO_ARGW_H
#define AIMS_IO_ARGW_H

#include <cartobase/object/syntax.h>
#include <cartobase/object/object.h>

class Graph;

namespace aims
{

  /// Wrapper for a specific GraphWriter
  class LowLevelArgWriter
  {
  public:
    enum SavingMode
    {
      Keep,
      Global,
      Local
    };

    LowLevelArgWriter();
    virtual ~LowLevelArgWriter();
    /// in case of failure, raise an exception
    virtual void write( const std::string & filename, Graph & graph, 
                        SavingMode mode = Keep ) = 0;
    /** get / reconstruct the minf header from the graph.
        The default implementation takes it from the "header" property of the
        graph (if any), and tries to rebuild referentials / transformations
        properties from the graph properties if possible.
    */
    virtual carto::Object getMinf( const Graph & g );
  };

  /* This class is a wrapper for several GraphWriters: standard GraphWriter, 
     FoldWriter, FRGWriter (defined in sigraph library), RoiArgWriter (defined
     in aimsalgo), etc. New writers can be added as plugins.
   */
  class ArgWriter
  {
  public:
    enum SavingMode
    {
      Keep,
      Global,
      Local
    };

    ArgWriter( const std::string& filename );
    ~ArgWriter();
    bool write( Graph &, SavingMode mode = Keep ) const;
    /** sets the writer to use for graphs of a specific syntax. The writer 
	then belongs to ArgWriter (never delete it) */
    static void registerWriter( const std::string & syntax, 
				LowLevelArgWriter *writer );
    static bool hasWriter( const std::string & syntax );
    /// removes and deletes a writer
    static void deleteWriter( const std::string & syntax );

  private:
    struct StaticPrivate;
    static StaticPrivate* staticPrivate();

    std::string	_name;
  };

  /// Wrapper for the standard GraphWriter
  class LowLevelStandardArgWriter : public LowLevelArgWriter
  {
  public:
    LowLevelStandardArgWriter();
    virtual ~LowLevelStandardArgWriter();
    virtual void write( const std::string & filename, Graph & graph, 
                        SavingMode mode = Keep );

  private:
    carto::SyntaxSet	*_syntax;
  };

}

#endif

