#include <graph/tree/tparser.h>
#include <soma-io/datasource/streamdatasource.h>

using namespace carto;
using namespace std;

TreeParser::TreeParser()
{
}


TreeParser::~TreeParser()
{
}


string TreeParser::name() const
{
  const_ref<DataSource>	ds = dataSource();
  if( ds.isNull() )
    return string();
  return ds->url();
}


