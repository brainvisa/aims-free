
#include <aims/topology/classifMeaning.h>
#include <string>

using namespace std;

const char* TopoNames[4][4] =
{
  { "Incoherent topology", "Isolated point", "Isolated point", 
    "Isolated point" },
  { "Interior point", "Border(Simple) point", "Surface point", 
    "Surfaces junction" },
  { "Interior point", "Curve point", "Surface-curve(s) junction", 
    "Surfaces-curve(s) junction" },
  { "Interior point", "Curves junction", "Surface-curve(s) junction", 
    "Surfaces-curve(s) junction" },
};


const int TopoClassif[4][4] = 
{
  { 
     -1 ,
     TopologicalClassificationMeaning::TopoB,
     TopologicalClassificationMeaning::TopoB,
     TopologicalClassificationMeaning::TopoB
  },
  { 
     TopologicalClassificationMeaning::TopoA,
     TopologicalClassificationMeaning::TopoC,
     TopologicalClassificationMeaning::TopoF,
     TopologicalClassificationMeaning::TopoH
  },
  { 
     TopologicalClassificationMeaning::TopoA,
     TopologicalClassificationMeaning::TopoD,
     TopologicalClassificationMeaning::TopoG,
     TopologicalClassificationMeaning::TopoI
  },
  { 
     TopologicalClassificationMeaning::TopoA,
     TopologicalClassificationMeaning::TopoE,
     TopologicalClassificationMeaning::TopoG,
     TopologicalClassificationMeaning::TopoI
  }
};


string TopologicalClassificationMeaning::stringFromDefine( int d )
{
  string s;

  switch( d )
    {
    case TopoA:
      s = TopoNames[ 1 ][ 0 ];
      break;
    case TopoB:
      s = TopoNames[ 0 ][ 1 ];
      break;
    case TopoC:
      s = TopoNames[ 1 ][ 1 ];
      break;
    case TopoD:
      s = TopoNames[ 2 ][ 1 ];
      break;
    case TopoE:
      s = TopoNames[ 3 ][ 1 ];
      break;
    case TopoF:
      s = TopoNames[ 1 ][ 2 ];
      break;
    case TopoG:
      s = TopoNames[ 2 ][ 2 ];
      break;
    case TopoH:
      s = TopoNames[ 1 ][ 3 ];
      break;
    case TopoI:
      s = TopoNames[ 2 ][ 3 ];
      break;
    }

  return s;
}


string TopologicalClassificationMeaning::name( int cstar, int cbar )
{
  if ( cstar > 3 )  cstar = 3;
  if ( cbar > 3 )  cbar = 3;

  return TopoNames[ cstar ][ cbar ];
}


int TopologicalClassificationMeaning::classification( int cstar, int cbar )
{
  if ( cstar > 3 )  cstar = 3;
  if ( cbar > 3 )  cbar = 3;

  return TopoClassif[ cstar ][ cbar ];
}
