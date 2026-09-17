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
                        SavingMode mode = Keep,
                        bool saveOnlyModified = true ) = 0;
    /** get / reconstruct the minf header from the graph.
        The default implementation takes it from the "header" property of the
        graph (if any), and tries to rebuild referentials / transformations
        properties from the graph properties if possible.
    */
    virtual carto::Object getMinf( const Graph & g );
  };

  /* This class is a wrapper for several GraphWriters: standard GraphWriter, 
     FoldWriter, FRGWriter (defined in sigraph library), etc. New writers can
     be added as plugins.
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
    bool write( Graph &, SavingMode mode = Keep,
                bool saveOnlyModified = true ) const;
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
                        SavingMode mode = Keep, bool saveOnlyModified = true );

  private:
    carto::SyntaxSet	*_syntax;
  };

}

#endif

