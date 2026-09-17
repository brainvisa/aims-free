
#include <aims/io/roigtmW.h>
#include <aims/io/writer.h>
#include <graph/graph/gwriter.h>

using namespace aims;
using namespace std;

RoiGtmWriter::RoiGtmWriter( const string& name ) 
  : _name( RoiGtmWriter::removeExtension(name) )
{
}


RoiGtmWriter::~RoiGtmWriter()
{
}

string RoiGtmWriter::removeExtension( const string& name )
{
  string res = name;
  string ext="",ext2="";
  if ( res.length() > 4 )
    ext = res.substr( int(res.length() - 4), 4 );
  if (ext == ".arg")
    res = res.substr( 0, res.length() - 4 );
  return res;
}

void RoiGtmWriter::write( RoiGtm& thing )
{ 
  // Write the graph
  // Fix name pb
  string namemask = _name + "_MASK.arg";
  thing.setProperty( "gtm_mask_basename", namemask );
  _name += ".arg";
  
  if( !thing.hasProperty("selector_basename") )
    thing.setProperty("selector_basename", string("all") ) ;
  string selbasename ;
  thing.getProperty("selector_basename", selbasename) ;
  if( selbasename == "" )
    thing.setProperty("selector_basename", string("all") ) ;
  
  GraphWriter	gw( _name, thing.syntax() );
  gw.write( thing );

  //Write the graph and bucket of _maskRoi
  Writer<Graph> rwt( namemask );
  rwt.write( thing.getMaskRoi() );
}
