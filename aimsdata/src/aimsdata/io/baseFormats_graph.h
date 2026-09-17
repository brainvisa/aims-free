#ifndef AIMS_IO_BASEFORMATS_GRAPH_H
#define AIMS_IO_BASEFORMATS_GRAPH_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>

class Graph;

namespace aims
{
  
  class ArgFormat : public FileFormat<Graph>
  {
  public:
    virtual ~ArgFormat();

    virtual bool read( const std::string & filename, Graph & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual Graph* read( const std::string & filename, 
                         const carto::AllocatorContext & context, 
                         carto::Object options );
    /** the \c forceglobal parameter is a hack that replaces the usual 
        \c ascii flag. If set, internal objects are saved in "global" 
        mode (see AimsGraphWriter)
    */
    virtual bool write( const std::string & filename, const Graph & obj, 
                        carto::Object options = carto::none() );
  };
  
  
  class GraphVolumeFormat : public FileFormat<Graph>
  {
  public:
    virtual ~GraphVolumeFormat();

    virtual bool read( const std::string & filename, Graph & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual Graph* read( const std::string & filename, 
                         const carto::AllocatorContext & context, 
                         carto::Object options );
  };
  
}


#endif
