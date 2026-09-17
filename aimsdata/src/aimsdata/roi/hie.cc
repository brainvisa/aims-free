
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/roi/hie.h>

using namespace aims;
using namespace carto;

Hierarchy::Hierarchy(  ) : Tree()
{
  sHie[ "hierarchy" ][ "graph_syntax" ].type = "string";
  sHie[ "hierarchy" ][ "graph_syntax" ].needed = true;
  
  sHie[ "fold_name" ][ "name" ].type = "string";
  sHie[ "fold_name" ][ "name" ].needed = true;
  
  sHie[ "fold_name" ][ "color" ].type = "int_vector";
  sHie[ "fold_name" ][ "color" ].needed = false;
  
  sHie[ "fold_name" ][ "label" ].type = "string";
  sHie[ "fold_name" ][ "label" ].needed = false;
  
  sHie[ "fold_name" ][ "roi_label" ].type = "int";
  sHie[ "fold_name" ][ "roi_label" ].needed = false;
  
  setSyntax( "QUOI" );

}


Hierarchy::~Hierarchy()
{
}


SyntaxSet& Hierarchy::syntax() 
{
  return sHie;
}
