

#include <aims/roi/roiselector.h>

using namespace aims;
using namespace carto;

RoiSelector::RoiSelector(  ) : Tree()
{
  syntaxRoiSelector["selector"]["graph_syntax"].type = "string";
  syntaxRoiSelector["selector"]["graph_syntax"].needed = true;
  
  syntaxRoiSelector[ "subset"][ "surname"].type = "string";
  syntaxRoiSelector[ "subset"][ "surname"].needed = false;
  
  syntaxRoiSelector[ "subset"]["nomenName"].type = "string";
  syntaxRoiSelector[ "subset"]["nomenName"].needed = false;

  setSyntax( "selector" );

}


RoiSelector::~RoiSelector()
{
}


SyntaxSet& RoiSelector::syntax() 
{
  return syntaxRoiSelector;
}
