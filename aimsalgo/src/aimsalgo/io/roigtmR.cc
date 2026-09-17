
#include <cartobase/stream/fileutil.h>
#include <aims/io/roigtmR.h>
#include <aims/io/aimsGraphR.h>
#include <aims/io/reader.h>
#include <graph/graph/greader.h>

using namespace aims;
using namespace std;
using namespace carto;

RoiGtmReader::RoiGtmReader( const string& name, ReadMode readRoi )
  : _name( name ),
    _readRoi( readRoi )
{
}


RoiGtmReader::~RoiGtmReader()
{
}


string RoiGtmReader::removeExtension( const string& name )
{
  string res = name;
  string ext="",ext2="";
  if ( res.length() > 4 )
    ext = res.substr( int(res.length() - 4), 4 );
  if (ext == string(".arg") )
    res = res.substr( 0, res.length() - 4 );
  return res;
}


void RoiGtmReader::read( RoiGtm& thing )
{ 
  // Read the graph
  GraphReader	gr( _name, thing.syntax() );
  if(thing.hasProperty("roi_basename") )
    thing.removeProperty("roi_basename") ;
  if(thing.hasProperty("selector_basename") )
    thing.removeProperty("selector_basename") ;
  
  cerr << "Reading" << endl ;
  gr.read( thing );
  cerr << "Read" << endl ;
  thing.editionSet();

  // If necessary read Roi file
  if( ReadRoi() )
    {
      string maskname; thing.getProperty("gtm_mask_basename", maskname);
      cout << "Mask name is " << maskname << endl ;
      cout << "File base name is " <<  FileUtil::dirname( _name ) << endl ;
      
      AimsRoi *roi = new AimsRoi;
      Reader<Graph> rrd( FileUtil::dirname( _name ) + "/" + maskname );
      rrd.read( *roi );
      thing.setMaskRoi( roi );
    }
}
