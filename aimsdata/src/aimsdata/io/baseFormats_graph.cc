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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/baseFormats_graph.h>
#include <aims/io/argR.h>
#include <aims/io/argW.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <cartodata/volume/volume.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>

using namespace carto;
using namespace aims;
using namespace std;

ArgFormat::~ArgFormat()
{
}


bool ArgFormat::read( const string & filename, Graph & obj, 
		      const AllocatorContext & /*context*/, Object options )
{
  int subobjectsfilter = -1;
  try
  {
    Object filt = options->getProperty( "subobjectsfilter" );
    subobjectsfilter = (int) filt->getScalar();
  }
  catch( ... )
  {
  }
  ArgReader	r( filename );
  Graph	*g = r.read( subobjectsfilter );
  if( !g )
    return( false );
  obj.clear();
  obj.clearProperties();
  obj.setSyntax( g->getSyntax() );
  g->extract( obj, g->begin(), g->end() );
  obj.copyProperties( Object::reference
                      ( g->value<AttributedObject::ContentType>() ) );
  delete g;
  return( true );
}


Graph* ArgFormat::read( const string & filename, 
                        const AllocatorContext & /*context*/, Object options )
{
  int subobjectsfilter = -1;
  try
  {
    Object filt = options->getProperty( "subobjectsfilter" );
    subobjectsfilter = (int) filt->getScalar();
  }
  catch( ... )
  {
  }
  ArgReader	r( filename );
  return r.read( subobjectsfilter );
}


bool ArgFormat::write( const string & filename, const Graph & obj, 
                       carto::Object options )
{
  //cout << "ArgFormat::write " << filename << endl;
  ArgWriter	w( filename );
  ArgWriter::SavingMode mode = ArgWriter::Keep;
  bool saveOnlyModified = true;

  if( !options.isNull() )
  {
    try
    {
      carto::Object aso = options->getProperty( "force_global" );
      if( !aso.isNull() )
        if( (bool) aso->getScalar() )
          mode = ArgWriter::Global;
    }
    catch( ... )
    {
      try
      {
        carto::Object aso = options->getProperty( "saving_mode" );
        if( !aso.isNull() )
        {
          string sm = aso->getString();
          if( sm == "local" )
            mode = ArgWriter::Local;
          else if( sm == "global" )
            mode = ArgWriter::Global;
        }
      }
      catch( ... )
      {
      }
    }

    try
    {
      carto::Object som = options->getProperty( "save_only_modified" );
      if( !som.isNull() )
        saveOnlyModified = bool( som->getScalar() );
    }
    catch( ... )
    {
    }
  }
  return w.write( (Graph &) obj, mode, saveOnlyModified );
}


GraphVolumeFormat::~GraphVolumeFormat()
{
}


namespace
{

  class GraphVolumeFormat_VolReader : public Process
  {
  public:
    GraphVolumeFormat_VolReader( const AllocatorContext &, Object,
                                 Graph *graph = 0 );
    virtual ~GraphVolumeFormat_VolReader();
    template <typename T>
    static bool read( Process &, const string &, Finder & );

    Graph *graph;
    const AllocatorContext & context;
    Object options;
  };

  GraphVolumeFormat_VolReader::GraphVolumeFormat_VolReader(
    const AllocatorContext & context, Object options, Graph *graph )
    : Process(), graph( graph ), context( context ), options( options )
  {
    registerProcessType( "Volume", "S16", &read<int16_t> );
    registerProcessType( "Volume", "S32", &read<int32_t> );
  }


  GraphVolumeFormat_VolReader::~GraphVolumeFormat_VolReader()
  {
  }


  template <typename T>
  bool GraphVolumeFormat_VolReader::read(
    Process & p, const string & filename, Finder & )
  {
    GraphVolumeFormat_VolReader
      & gvr = static_cast<GraphVolumeFormat_VolReader &>( p );
    Reader<Volume<T> >      r( filename );
    r.setOptions( gvr.options );
    r.setAllocatorContext( gvr.context );
    VolumeRef<T> data = r.read();
    if( gvr.graph )
      GraphManip::graphFromVolume( data, *gvr.graph );
    else
      gvr.graph = GraphManip::graphFromVolume( data );
    return true;
  }

}


bool GraphVolumeFormat::read( const string & filename, Graph & obj,
                              const AllocatorContext & context,
                              Object options )
{
  GraphVolumeFormat_VolReader p( context, options, &obj );
  return p.execute( filename );
}


Graph* GraphVolumeFormat::read( const string & filename, 
                                const carto::AllocatorContext & context, 
                                carto::Object options )
{
  GraphVolumeFormat_VolReader p( context, options );
  if( !p.execute( filename ) )
    return 0;
  return p.graph;
}
