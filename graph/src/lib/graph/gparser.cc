#include <graph/graph/gparser.h>
#include <soma-io/datasource/streamdatasource.h>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================


GraphParser::GraphParser() 
{
}


GraphParser::~GraphParser()
{
}


void GraphParser::attach( rc_ptr<DataSource> ds )
{
  _datasource = ds;
}


void GraphParser::detach()
{
  _datasource.reset( 0 );
}


string GraphParser::name() const
{
  const_ref<DataSource>	ds = dataSource();
  if( ds.isNull() )
    return string();
  return ds->url();
}


rc_ptr<DataSource> GraphParser::dataSource()
{
  return _datasource;
}


