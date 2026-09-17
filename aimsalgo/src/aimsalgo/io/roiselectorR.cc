
#include <aims/roi/roiselector.h>
#include <aims/io/roiselectorR.h>
#include <aims/selection/selection.h>
#include <aims/io/selectionr.h>

using namespace aims;
using namespace carto;
using namespace std;


RoiSelectorReader::RoiSelectorReader(  const string& name, 
                                       const SyntaxSet& /* stx */ )
  : _name( name )
{
}


RoiSelectorReader::~RoiSelectorReader()
{
}


string RoiSelectorReader::removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 4 )
    ext = res.substr( int(res.length() - 4), 4 );
  if (ext ==  string(".sel") )
    res = res.substr( 0, res.length() - 4 );
  return res;
}


void  RoiSelectorReader::read(RoiSelector& thing)
{
  SelectionSet selset;
  try
  {
    SelectionReader sr( _name );
    sr.read( selset );
  }
  catch( ... )
  {
    // maybe wrong extension
    SelectionReader sr( removeExtension( _name ) + ".sel" );
    sr.read( selset );
  }

  // convert to RoiSelector / Tree
  SelectionSet::iterator iss, ess = selset.end();
  Selection::iterator is, es;
  for( iss=selset.begin(); iss!=ess; ++iss )
  {
    Tree *tr = new Tree( true, "subset" );
    thing.insert( tr );
    tr->setProperty( "surname", iss->name() );
    for( is=iss->begin(), es=iss->end(); is!=es; ++is )
    {
      Tree *tr2 = new Tree( true, "subset" );
      tr->insert( tr2 );
      tr2->setProperty( "nomenName", *is );
    }
  }
}
