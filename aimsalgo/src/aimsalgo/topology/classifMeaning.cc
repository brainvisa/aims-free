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
