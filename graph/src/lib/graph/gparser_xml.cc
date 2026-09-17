#include <graph/graph/gparser.h>
#include <graph/graph/gparser_xml.h>
#include <graph/graph/graph.h>
#include <cartobase/exception/parse.h>
#include <iostream>

using namespace std;
using namespace carto;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphParser_xml::GraphParser_xml()
  : GraphParser()
{
  //FIXME: all needed by xml parsing
}


GraphParser_xml::~GraphParser_xml()
{
}

int
GraphParser_xml::read(Graph&)
{
  return (-1);
}

