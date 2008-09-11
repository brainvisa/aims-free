/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/mesh/mesher.h>

using namespace std;

struct PreNeighType
{
  short nPreNeigh[ 4 ];
  Facet* preNeigh[ 4 ][ 7 ];
};

typedef map< uint, PreNeighType*, less<int> > MapOfPreNeigh;


#define NMAXLOOKUPTRIANGLES 256

// Pre-triangulated configurations
struct LocalTriangulation
{
  int   code;
  short cycle[ 7 ];
  short nCycle;
  // table qui contient les indices des voisins a inserer
  // dans chaque voisinage des faces du cycle, en fonction
  // de leur decalage .
  // le premier indice identifie le decalage et le second 
  // donne les indices des faces voisines dans l'ordre
  short nNeighbor[ 7 ];
  short tabNeighbor[ 7 ][ 7 ];
};


void Mesher::getExtendedNeighOfFacet( MapOfFacet &thing )
{ 
  Point3d LookUpVertex[ 6 ][ 4 ];
  LocalTriangulation LookUpLocalTriang[ NMAXLOOKUPTRIANGLES ];
  int nblookuptriang = 0;
  
  LookUpVertex[0][0] = Point3d(0,1,0);
  LookUpVertex[0][1] = Point3d(0,1,1);
  LookUpVertex[0][2] = Point3d(0,0,1);
  LookUpVertex[0][3] = Point3d(0,0,0);

  LookUpVertex[1][0] = Point3d(1,0,0);
  LookUpVertex[1][1] = Point3d(0,0,0);
  LookUpVertex[1][2] = Point3d(0,0,1);
  LookUpVertex[1][3] = Point3d(1,0,1);

  LookUpVertex[2][0] = Point3d(1,1,0);
  LookUpVertex[2][1] = Point3d(0,1,0);
  LookUpVertex[2][2] = Point3d(0,0,0);
  LookUpVertex[2][3] = Point3d(1,0,0);

  LookUpVertex[3][0] = Point3d(0,1,1);
  LookUpVertex[3][1] = Point3d(1,1,1);
  LookUpVertex[3][2] = Point3d(1,0,1);
  LookUpVertex[3][3] = Point3d(0,0,1);

  LookUpVertex[4][0] = Point3d(1,1,1);
  LookUpVertex[4][1] = Point3d(0,1,1);
  LookUpVertex[4][2] = Point3d(0,1,0);
  LookUpVertex[4][3] = Point3d(1,1,0);

  LookUpVertex[5][0] = Point3d(1,1,1);
  LookUpVertex[5][1] = Point3d(1,1,0);
  LookUpVertex[5][2] = Point3d(1,0,0);
  LookUpVertex[5][3] = Point3d(1,0,1);

  // Local neighbors configurations arround a face  
  // edge and associated triangulation for extended
  // neighborhood construction
  //              _____________                                       
  //  Exemple :  /     /       |                                      
  //            /_____/______/ |                                     
  //           |      |      | |                                     
  //           |   0  |   3  | |                                     
  //           |      |      |/                                      
  //           |______X______/
  //           /      /      /                                       
  //          /   1  /   2  /                                           
  //         /______/______/                                            
  //       
  //        number of faces : 4
  //
  //        Cycle : faces cycle (0 1 2 3) 
  //
  //                                ||                  
  //                                \/                  
  //
  //                coded cycle (2 1 2 1) coded with : back  = 0 
  //                                                   same  = 1 
  //                                                   front = 2 
  //                                                   
  //                                ||                  
  //                                \/                  
  //
  //                identifier     153    coded by binary conversion
  //                                                   
  //                                                           
  //        Triangles : (0 1 2) (2 3 0)  
  //                           
  //        3 neighboors for 0 : 1,2,3                                   
  //        2 neighboors for 1 : 2,0                                         
  //        3 neighboors for 2 : 3,0,1
  //        2 neighboors for 3 : 0,2  
  //                                                                  
  //                                                                  
  // basics configurations
  // ---------------------
  //
  LookUpLocalTriang[0].nCycle = 3;
  LookUpLocalTriang[0].cycle[0] = 0;
  LookUpLocalTriang[0].cycle[1] = 0;
  LookUpLocalTriang[0].cycle[2] = 0;
  LookUpLocalTriang[0].nNeighbor[0] = 2;
  LookUpLocalTriang[0].tabNeighbor[0][1] = 1;
  LookUpLocalTriang[0].tabNeighbor[0][0] = 2;
  LookUpLocalTriang[0].nNeighbor[1] = 2;
  LookUpLocalTriang[0].tabNeighbor[1][1] = 2;
  LookUpLocalTriang[0].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[0].nNeighbor[2] = 2;
  LookUpLocalTriang[0].tabNeighbor[2][1] = 0;
  LookUpLocalTriang[0].tabNeighbor[2][0] = 1;
  //
  LookUpLocalTriang[1].nCycle = 4;
  LookUpLocalTriang[1].cycle[0] = 1;
  LookUpLocalTriang[1].cycle[1] = 0;
  LookUpLocalTriang[1].cycle[2] = 1;
  LookUpLocalTriang[1].cycle[3] = 0;
  LookUpLocalTriang[1].nNeighbor[0] = 2;
  LookUpLocalTriang[1].tabNeighbor[0][1] = 1;
  LookUpLocalTriang[1].tabNeighbor[0][0] = 3;
  LookUpLocalTriang[1].nNeighbor[1] = 3;
  LookUpLocalTriang[1].tabNeighbor[1][2] = 2;
  LookUpLocalTriang[1].tabNeighbor[1][1] = 3;
  LookUpLocalTriang[1].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[1].nNeighbor[2] = 2;
  LookUpLocalTriang[1].tabNeighbor[2][1] = 3;
  LookUpLocalTriang[1].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[1].nNeighbor[3] = 3;
  LookUpLocalTriang[1].tabNeighbor[3][2] = 0;
  LookUpLocalTriang[1].tabNeighbor[3][1] = 1;
  LookUpLocalTriang[1].tabNeighbor[3][0] = 2;
  //
  LookUpLocalTriang[2].nCycle = 6;
  LookUpLocalTriang[2].cycle[0] = 2;
  LookUpLocalTriang[2].cycle[1] = 0;
  LookUpLocalTriang[2].cycle[2] = 0;
  LookUpLocalTriang[2].cycle[3] = 2;
  LookUpLocalTriang[2].cycle[4] = 0;
  LookUpLocalTriang[2].cycle[5] = 0;
  LookUpLocalTriang[2].nNeighbor[0] = 2;
  LookUpLocalTriang[2].tabNeighbor[0][1] = 1;
  LookUpLocalTriang[2].tabNeighbor[0][0] = 5;
  LookUpLocalTriang[2].nNeighbor[1] = 3;
  LookUpLocalTriang[2].tabNeighbor[1][2] = 2;
  LookUpLocalTriang[2].tabNeighbor[1][1] = 5;
  LookUpLocalTriang[2].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[2].nNeighbor[2] = 4;
  LookUpLocalTriang[2].tabNeighbor[2][3] = 3;
  LookUpLocalTriang[2].tabNeighbor[2][2] = 4;
  LookUpLocalTriang[2].tabNeighbor[2][1] = 5;
  LookUpLocalTriang[2].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[2].nNeighbor[3] = 2;
  LookUpLocalTriang[2].tabNeighbor[3][1] = 4;
  LookUpLocalTriang[2].tabNeighbor[3][0] = 2;
  LookUpLocalTriang[2].nNeighbor[4] = 3;
  LookUpLocalTriang[2].tabNeighbor[4][2] = 5;
  LookUpLocalTriang[2].tabNeighbor[4][1] = 2;
  LookUpLocalTriang[2].tabNeighbor[4][0] = 3;
  LookUpLocalTriang[2].nNeighbor[5] = 4;
  LookUpLocalTriang[2].tabNeighbor[5][3] = 0;
  LookUpLocalTriang[2].tabNeighbor[5][2] = 1;
  LookUpLocalTriang[2].tabNeighbor[5][1] = 2;
  LookUpLocalTriang[2].tabNeighbor[5][0] = 4;
  //
  LookUpLocalTriang[3].nCycle = 5;
  LookUpLocalTriang[3].cycle[0] = 2;
  LookUpLocalTriang[3].cycle[1] = 0;
  LookUpLocalTriang[3].cycle[2] = 1;
  LookUpLocalTriang[3].cycle[3] = 1;
  LookUpLocalTriang[3].cycle[4] = 0;
  LookUpLocalTriang[3].nNeighbor[0] = 3;
  LookUpLocalTriang[3].tabNeighbor[0][2] = 1;
  LookUpLocalTriang[3].tabNeighbor[0][1] = 3;
  LookUpLocalTriang[3].tabNeighbor[0][0] = 4;
  LookUpLocalTriang[3].nNeighbor[1] = 3;
  LookUpLocalTriang[3].tabNeighbor[1][2] = 2;
  LookUpLocalTriang[3].tabNeighbor[1][1] = 3;
  LookUpLocalTriang[3].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[3].nNeighbor[2] = 2;
  LookUpLocalTriang[3].tabNeighbor[2][1] = 3;
  LookUpLocalTriang[3].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[3].nNeighbor[3] = 4;
  LookUpLocalTriang[3].tabNeighbor[3][3] = 4;
  LookUpLocalTriang[3].tabNeighbor[3][2] = 0;
  LookUpLocalTriang[3].tabNeighbor[3][1] = 1;
  LookUpLocalTriang[3].tabNeighbor[3][0] = 2;
  LookUpLocalTriang[3].nNeighbor[4] = 2;
  LookUpLocalTriang[3].tabNeighbor[4][1] = 0;
  LookUpLocalTriang[3].tabNeighbor[4][0] = 3;
  //
  LookUpLocalTriang[4].nCycle = 6;
  LookUpLocalTriang[4].cycle[0] = 2;
  LookUpLocalTriang[4].cycle[1] = 0;
  LookUpLocalTriang[4].cycle[2] = 2;
  LookUpLocalTriang[4].cycle[3] = 0;
  LookUpLocalTriang[4].cycle[4] = 2;
  LookUpLocalTriang[4].cycle[5] = 0;
  LookUpLocalTriang[4].nNeighbor[0] = 2;
  LookUpLocalTriang[4].tabNeighbor[0][1] = 1;
  LookUpLocalTriang[4].tabNeighbor[0][0] = 5;
  LookUpLocalTriang[4].nNeighbor[1] = 4;
  LookUpLocalTriang[4].tabNeighbor[1][3] = 2;
  LookUpLocalTriang[4].tabNeighbor[1][2] = 3;
  LookUpLocalTriang[4].tabNeighbor[1][1] = 5;
  LookUpLocalTriang[4].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[4].nNeighbor[2] = 2;
  LookUpLocalTriang[4].tabNeighbor[2][1] = 3;
  LookUpLocalTriang[4].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[4].nNeighbor[3] = 4;
  LookUpLocalTriang[4].tabNeighbor[3][3] = 4;
  LookUpLocalTriang[4].tabNeighbor[3][2] = 5;
  LookUpLocalTriang[4].tabNeighbor[3][1] = 1;
  LookUpLocalTriang[4].tabNeighbor[3][0] = 2;
  LookUpLocalTriang[4].nNeighbor[4] = 2;
  LookUpLocalTriang[4].tabNeighbor[4][1] = 5;
  LookUpLocalTriang[4].tabNeighbor[4][0] = 3;
  LookUpLocalTriang[4].nNeighbor[5] = 4;
  LookUpLocalTriang[4].tabNeighbor[5][3] = 0;
  LookUpLocalTriang[4].tabNeighbor[5][2] = 1;
  LookUpLocalTriang[4].tabNeighbor[5][1] = 3;
  LookUpLocalTriang[4].tabNeighbor[5][0] = 4;
  //
  LookUpLocalTriang[5].nCycle = 6;
  LookUpLocalTriang[5].cycle[0] = 2;
  LookUpLocalTriang[5].cycle[1] = 1;
  LookUpLocalTriang[5].cycle[2] = 0;
  LookUpLocalTriang[5].cycle[3] = 2;
  LookUpLocalTriang[5].cycle[4] = 1;
  LookUpLocalTriang[5].cycle[5] = 0;
  LookUpLocalTriang[5].nNeighbor[0] = 3;
  LookUpLocalTriang[5].tabNeighbor[0][2] = 1;
  LookUpLocalTriang[5].tabNeighbor[0][1] = 2;
  LookUpLocalTriang[5].tabNeighbor[0][0] = 5;
  LookUpLocalTriang[5].nNeighbor[1] = 2;
  LookUpLocalTriang[5].tabNeighbor[1][1] = 2;
  LookUpLocalTriang[5].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[5].nNeighbor[2] = 4;
  LookUpLocalTriang[5].tabNeighbor[2][3] = 3;
  LookUpLocalTriang[5].tabNeighbor[2][2] = 5;
  LookUpLocalTriang[5].tabNeighbor[2][1] = 0;
  LookUpLocalTriang[5].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[5].nNeighbor[3] = 3;
  LookUpLocalTriang[5].tabNeighbor[3][2] = 4;
  LookUpLocalTriang[5].tabNeighbor[3][1] = 5;
  LookUpLocalTriang[5].tabNeighbor[3][0] = 2;
  LookUpLocalTriang[5].nNeighbor[4] = 2;
  LookUpLocalTriang[5].tabNeighbor[4][1] = 5;
  LookUpLocalTriang[5].tabNeighbor[4][0] = 3;
  LookUpLocalTriang[5].nNeighbor[5] = 4;
  LookUpLocalTriang[5].tabNeighbor[5][3] = 0;
  LookUpLocalTriang[5].tabNeighbor[5][2] = 2;
  LookUpLocalTriang[5].tabNeighbor[5][1] = 3;
  LookUpLocalTriang[5].tabNeighbor[5][0] = 4;
  //
  LookUpLocalTriang[6].nCycle = 5;
  LookUpLocalTriang[6].cycle[0] = 2;
  LookUpLocalTriang[6].cycle[1] = 1;
  LookUpLocalTriang[6].cycle[2] = 1;
  LookUpLocalTriang[6].cycle[3] = 2;
  LookUpLocalTriang[6].cycle[4] = 0;
  LookUpLocalTriang[6].nNeighbor[0] = 3;
  LookUpLocalTriang[6].tabNeighbor[0][2] = 1;
  LookUpLocalTriang[6].tabNeighbor[0][1] = 2;
  LookUpLocalTriang[6].tabNeighbor[0][0] = 4;
  LookUpLocalTriang[6].nNeighbor[1] = 2;
  LookUpLocalTriang[6].tabNeighbor[1][1] = 2;
  LookUpLocalTriang[6].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[6].nNeighbor[2] = 4;
  LookUpLocalTriang[6].tabNeighbor[2][3] = 3;
  LookUpLocalTriang[6].tabNeighbor[2][2] = 4;
  LookUpLocalTriang[6].tabNeighbor[2][1] = 0;
  LookUpLocalTriang[6].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[6].nNeighbor[3] = 2;
  LookUpLocalTriang[6].tabNeighbor[3][1] = 4;
  LookUpLocalTriang[6].tabNeighbor[3][0] = 2;
  LookUpLocalTriang[6].nNeighbor[4] = 3;
  LookUpLocalTriang[6].tabNeighbor[4][2] = 0;
  LookUpLocalTriang[6].tabNeighbor[4][1] = 2;
  LookUpLocalTriang[6].tabNeighbor[4][0] = 3;
  //
  LookUpLocalTriang[7].nCycle = 4;
  LookUpLocalTriang[7].cycle[0] = 2;
  LookUpLocalTriang[7].cycle[1] = 1;
  LookUpLocalTriang[7].cycle[2] = 2;
  LookUpLocalTriang[7].cycle[3] = 1;
  LookUpLocalTriang[7].nNeighbor[0] = 3;
  LookUpLocalTriang[7].tabNeighbor[0][2] = 1;
  LookUpLocalTriang[7].tabNeighbor[0][1] = 2;
  LookUpLocalTriang[7].tabNeighbor[0][0] = 3;
  LookUpLocalTriang[7].nNeighbor[1] = 2;
  LookUpLocalTriang[7].tabNeighbor[1][1] = 2;
  LookUpLocalTriang[7].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[7].nNeighbor[2] = 3;
  LookUpLocalTriang[7].tabNeighbor[2][2] = 3;
  LookUpLocalTriang[7].tabNeighbor[2][1] = 0;
  LookUpLocalTriang[7].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[7].nNeighbor[3] = 2;
  LookUpLocalTriang[7].tabNeighbor[3][1] = 0;
  LookUpLocalTriang[7].tabNeighbor[3][0] = 2;
  //
  LookUpLocalTriang[8].nCycle = 3;
  LookUpLocalTriang[8].cycle[0] = 2;
  LookUpLocalTriang[8].cycle[1] = 2;
  LookUpLocalTriang[8].cycle[2] = 2;
  LookUpLocalTriang[8].nNeighbor[0] = 2;
  LookUpLocalTriang[8].tabNeighbor[0][1] = 1;
  LookUpLocalTriang[8].tabNeighbor[0][0] = 2;
  LookUpLocalTriang[8].nNeighbor[1] = 2;
  LookUpLocalTriang[8].tabNeighbor[1][1] = 2;
  LookUpLocalTriang[8].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[8].nNeighbor[2] = 2;
  LookUpLocalTriang[8].tabNeighbor[2][1] = 0;
  LookUpLocalTriang[8].tabNeighbor[2][0] = 1;
  //
  LookUpLocalTriang[9].nCycle = 7;
  LookUpLocalTriang[9].cycle[0] = 1;
  LookUpLocalTriang[9].cycle[1] = 2;
  LookUpLocalTriang[9].cycle[2] = 0;
  LookUpLocalTriang[9].cycle[3] = 0;
  LookUpLocalTriang[9].cycle[4] = 2;
  LookUpLocalTriang[9].cycle[5] = 1;
  LookUpLocalTriang[9].cycle[6] = 0;
  LookUpLocalTriang[9].nNeighbor[0] = 4;
  LookUpLocalTriang[9].tabNeighbor[0][3] = 1;
  LookUpLocalTriang[9].tabNeighbor[0][2] = 2;
  LookUpLocalTriang[9].tabNeighbor[0][1] = 3;
  LookUpLocalTriang[9].tabNeighbor[0][0] = 6;
  LookUpLocalTriang[9].nNeighbor[1] = 2;
  LookUpLocalTriang[9].tabNeighbor[1][1] = 2;
  LookUpLocalTriang[9].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[9].nNeighbor[2] = 3;
  LookUpLocalTriang[9].tabNeighbor[2][2] = 3;
  LookUpLocalTriang[9].tabNeighbor[2][1] = 0;
  LookUpLocalTriang[9].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[9].nNeighbor[3] = 4;
  LookUpLocalTriang[9].tabNeighbor[3][3] = 4;
  LookUpLocalTriang[9].tabNeighbor[3][2] = 6;
  LookUpLocalTriang[9].tabNeighbor[3][1] = 0;
  LookUpLocalTriang[9].tabNeighbor[3][0] = 2;
  LookUpLocalTriang[9].nNeighbor[4] = 3;
  LookUpLocalTriang[9].tabNeighbor[4][2] = 5;
  LookUpLocalTriang[9].tabNeighbor[4][1] = 6;
  LookUpLocalTriang[9].tabNeighbor[4][0] = 3;
  LookUpLocalTriang[9].nNeighbor[5] = 2;
  LookUpLocalTriang[9].tabNeighbor[5][1] = 6;
  LookUpLocalTriang[9].tabNeighbor[5][0] = 4;
  LookUpLocalTriang[9].nNeighbor[6] = 4;
  LookUpLocalTriang[9].tabNeighbor[6][3] = 0;
  LookUpLocalTriang[9].tabNeighbor[6][2] = 3;
  LookUpLocalTriang[9].tabNeighbor[6][1] = 4;
  LookUpLocalTriang[9].tabNeighbor[6][0] = 5;
  //
  LookUpLocalTriang[10].nCycle = 4;
  LookUpLocalTriang[10].cycle[0] = 1;
  LookUpLocalTriang[10].cycle[1] = 1;
  LookUpLocalTriang[10].cycle[2] = 1;
  LookUpLocalTriang[10].cycle[3] = 1;
  LookUpLocalTriang[10].nNeighbor[0] = 2;
  LookUpLocalTriang[10].tabNeighbor[0][1] = 1;
  LookUpLocalTriang[10].tabNeighbor[0][0] = 3;
  LookUpLocalTriang[10].nNeighbor[1] = 3;
  LookUpLocalTriang[10].tabNeighbor[1][2] = 2;
  LookUpLocalTriang[10].tabNeighbor[1][1] = 3;
  LookUpLocalTriang[10].tabNeighbor[1][0] = 0;
  LookUpLocalTriang[10].nNeighbor[2] = 2;
  LookUpLocalTriang[10].tabNeighbor[2][1] = 3;
  LookUpLocalTriang[10].tabNeighbor[2][0] = 1;
  LookUpLocalTriang[10].nNeighbor[3] = 3;
  LookUpLocalTriang[10].tabNeighbor[3][2] = 0;
  LookUpLocalTriang[10].tabNeighbor[3][1] = 1;
  LookUpLocalTriang[10].tabNeighbor[3][0] = 2;
  
  //
  // derivated configurations
  // ------------------------
  //
  nblookuptriang = 11;
  //
  // symetricals configurations
  //
  int iFin = nblookuptriang;
  int	i, j, k, l;
  for ( i = 0; i < iFin; i++ )
  {
    // some configurations are unchanged
    if ( i ==  0 ) continue;
    if ( i ==  1 ) continue;
    if ( i ==  7 ) continue;
    if ( i ==  8 ) continue;
    if ( i == 10 ) continue;
    // cycle
    LookUpLocalTriang[ nblookuptriang ].nCycle = LookUpLocalTriang[ i ].nCycle;
    for ( j = 0; j < LookUpLocalTriang[ i ].nCycle; j++ )
    {
      int jj = LookUpLocalTriang[ i ].nCycle - 1 - j;
      LookUpLocalTriang[ nblookuptriang ].cycle[ j ] =
        LookUpLocalTriang[ i ].cycle[ jj ];
    }
    // neighbors table
    for ( j = 0; j < LookUpLocalTriang[ i ].nCycle; j++ )
    {
      int jj = ( LookUpLocalTriang[ i ].nCycle - j )
               % LookUpLocalTriang[ i ].nCycle;
      LookUpLocalTriang[ nblookuptriang ].nNeighbor[ j ] =
        LookUpLocalTriang[ i ].nNeighbor[ jj ];
      for ( k = 0; k < LookUpLocalTriang[ i ].nNeighbor[ jj ]; k++ )
      {
        int kk = LookUpLocalTriang[ i ].nNeighbor[ jj ] - 1 - k;
        LookUpLocalTriang[ nblookuptriang ].tabNeighbor[ j ][ k ] =
          ( LookUpLocalTriang[ i ].nCycle -
            LookUpLocalTriang[ i ].tabNeighbor[ jj ][ kk ] )
          % LookUpLocalTriang[ i ].nCycle;
      }
    }
    nblookuptriang ++;
  }

  //
  // rotated configurations 
  //
  iFin = nblookuptriang;
  for ( i = 0; i < iFin; i++ )
  {
    // some configurations are unchanged
    if ( i ==  0 ) continue;
    if ( i ==  8 ) continue;
    if ( i == 10 ) continue;
    for ( l = 1; l < LookUpLocalTriang[ i ].nCycle; l++ )
    {
      // cycle
      LookUpLocalTriang[ nblookuptriang ].nCycle =
        LookUpLocalTriang[ i ].nCycle;
      for ( j = 0; j < LookUpLocalTriang[ i ].nCycle; j++ )
      {
        int jj = ( j + l ) % LookUpLocalTriang[ i ].nCycle;
        LookUpLocalTriang[ nblookuptriang ].cycle[ j ] =
          LookUpLocalTriang[ i ].cycle[ jj ];
      }
      // neighbours table
      for ( j = 0; j < LookUpLocalTriang[ i ].nCycle; j++ )
      {
        int jj = ( j + l ) % LookUpLocalTriang[ i ].nCycle;
        LookUpLocalTriang[ nblookuptriang ].nNeighbor[ j ] =
          LookUpLocalTriang[ i ].nNeighbor[ jj ];
        for ( k = 0; k < LookUpLocalTriang[ i ].nNeighbor[ jj ]; k++ )
        {
          LookUpLocalTriang[ nblookuptriang ].tabNeighbor[ j ][ k ] =
            ( LookUpLocalTriang[ i ].tabNeighbor[ jj ][ k ] + 
              LookUpLocalTriang[ i ].nCycle - l )
            % LookUpLocalTriang[ i ].nCycle;
        }
      }
      nblookuptriang ++;
    }
  }

  //
  // computing configurations id
  // by binary conversion of the coded face cycle
  //
  
  for ( i = 0; i < nblookuptriang; i++ )
  {
    LookUpLocalTriang[ i ].code = 4;
    for ( j = 0; j < LookUpLocalTriang[ i ].nCycle; j++,
          LookUpLocalTriang[ i ].code = LookUpLocalTriang[ i ].code << 2 )
    {
      if ( LookUpLocalTriang[ i ].cycle[ j ] == 0 )
        continue;
      if ( LookUpLocalTriang[ i ].cycle[ j ] == 1 )
      {
        LookUpLocalTriang[ i ].code += 1;
        continue;
      }
      LookUpLocalTriang[ i ].code += 2; 
    }
  }

  //
  // End of LookUpLocalTriang initialization
  //

  // extended neighborhood of a facet is computed by computation of
  // its 4 verteces extended neighborhood
  Point3d a0,a1,a2;
  MapOfFacet::iterator it;
  Facet *facet = NULL, *f1 = NULL, *f2 = NULL, *g = NULL, *h = NULL, *u = NULL;
  MapOfPreNeigh mpreneigh;
  PreNeighType* preNeigh = NULL;
  int n, counter;
  Facet* cycle[ 7 ];
  short cyclevertexn[ 7 ];
  short ncycle;
  int code;
  LocalTriangulation* ltri;
  // pre-neighborhood map initialization
  for ( it = thing.begin(); it != thing.end(); it++ )
  { 
    preNeigh = new PreNeighType;
    preNeigh->nPreNeigh[ 0 ] = 0;
    preNeigh->nPreNeigh[ 1 ] = 0;
    preNeigh->nPreNeigh[ 2 ] = 0;
    preNeigh->nPreNeigh[ 3 ] = 0;
    mpreneigh[ it->first ] = preNeigh;
  }
  
  for ( it = thing.begin(); it != thing.end(); it++ )
  {
    facet = it->second;
    /* everything here is temporary debug */
//     cout << "DEBUG : location=" << facet->location() << endl;
//     cout << "DEBUG : nNeighbor=" << facet->nNeighbor() << endl;
//     cout << "DEBUG : neighbors are :" << endl;
//     cout << "DEBUG : " << facet->pNeighbor( 0 )->location() << endl;
//     cout << "DEBUG : " << facet->pNeighbor( 1 )->location() << endl;
//     cout << "DEBUG : " << facet->pNeighbor( 2 )->location() << endl;
//     cout << "DEBUG : " << facet->pNeighbor( 3 )->location() << endl;
 
    /*-------------------------------------*/
    
/*    cout << "\nDEBUG : HERE WE GO : (" << facet << "," << it->first << ")" << endl;*/
    ASSERT( facet->nNeighbor() == 4 );
    // Corrupted neighborood in facets mesh

    for (j=0;j<4;j++)
    { 
//           cout << "DEBUG : j=" << j << endl;

      //
      // f1 and f2 are 2 successive neighbors
      // [f1,f2] = [ (j+3)%4 , (j+4)%4 ] = [3,0],[0,1],[1,2],[2,3]
      // when j=0,1,2,3
      //
      f1 = thing[ facet->pNeighbor( ( j + 3 ) % 4 )->id() ];
      f2 = thing[ facet->pNeighbor( ( j + 4 ) % 4 )->id() ];
      
//       cout << "\tDEBUG : f1=" << f1 << endl;
//       cout << "\tDEBUG : f2=" << f2 << endl;
//       //
      // determines a0, the common vertex of f1 and f2
      //
      for ( k = 0; k < 4; k++ )
      {
//                 cout << "\tDEBUG : k=" << k << endl;

        // kth vertex of facet
        a0 = facet->location() + LookUpVertex[ facet->type() ][ k ];
        for ( l = 0; l < facet->nNeighbor(); l++ )
        {
//                   cout << "\t\tDEBUG : l=" << l << endl;

          // lth vertex of f1
          a1 = f1->location() + LookUpVertex[ f1->type() ][ l ];
          if ( a0 == a1 )
            for ( n = 0; n < 4; n++ )
            {
//                cout << "\t\t\tDEBUG : a0=a1, n=" << n << endl;
              // nth vertex of f2
              Point3d test1=f2->location();
//               cout << "\t\t\t\tDEBUG : f2->location OK : " << test1 << endl;
              Point3d test2=LookUpVertex[ f2->type() ][ n ];
//               cout << "\t\t\t\tDEBUG : LookUp OK : " << test2 << endl;
              a2 = f2->location() + LookUpVertex[ f2->type() ][ n ];
//               cout << "\t\t\tDEBUG : Does it get there ?" << endl;
              if ( a0 == a2 )
              {
//                 cout << "\t\t\t\tDEBUG : a0=a2" << endl;
                // save the facet indice of a0 for later use (*)
                cyclevertexn[ 0 ] = k;
                // save the f1 indice of a0 for later use (*)
                cyclevertexn[ 1 ] = l;
                // go out of the 3 loops
                n = l = k = 4;
//                 cout << "\t\t\t\tDEBUG : OUT NOW" << endl;
              }
            }
        }
      }

      // cheks if vertex a0 has already been triangulated
      if ( mpreneigh[ facet->id() ]->nPreNeigh[ cyclevertexn[ 0 ] ] != 0 )
        continue;

      // first member of the cycle of facets surrounding a0 is f
      // (*) its a0 indice is already saved
      ncycle = 0;
      cycle[ ncycle ] = facet;
      ncycle ++;

      // second member of the cycle of facets surrounding a0 is f1
      // (*) its a0 indice is already saved
      cycle[ ncycle ] = f1;
      ncycle ++;

      g = facet;

      //
      // looks for the neighbor of f1 which contains a0 but
      // that is different from g(=facet)
      //
      do
      { 
        for ( k = 0; k < 4; k++ )
        {
          h = thing[ f1->pNeighbor( k )->id() ];
          if ( h->equal(*g) == false )
            for ( l = 0; l < 4; l++ )
            {
              // lth vertex of h
              a1 = h->location() + LookUpVertex[ h->type() ][ l ];
              if ( a0 == a1 )
              {
                u = f1->pNeighbor( k );
                // save the u indice of a0 (**)
                cyclevertexn[ ncycle ] = l;
                // go out of the 2 loops
                l = k = 4;
              }
            }
        }

        // save u has a member of the cycle surrounding a0
        // (**) its indice of a0 vertex is already saved
        cycle[ ncycle ] = u;
        ncycle ++;
        g  = f1;
        f1 =  h; 
      }
      while ( f1->equal( *f2 ) == false );


      // looks for a pre-computed triangulation corresponding
      // to the surrounding cycle of facet

      // computes configuration id
      code = 4;
      for ( l = 1; l <= ncycle; l++, code = code << 2 )
      {
        // check for BACK code
        if ( thing[ cycle[ l % ncycle ]->id() ]->offset() == 
             thing[ cycle[ ( l - 1 ) % ncycle ]->id() ]->offset() )
          continue;
        // check for SAME code
        if ( thing[ cycle[ l % ncycle ]->id() ]->type() == 
             thing[ cycle[ ( l - 1 ) % ncycle ]->id() ]->type() )
        { 
          code += 1;
          continue;    
        }
        // FRONT code
        code += 2;
      }

      // looks for a configuration with same id
      ltri = NULL;
      for ( l = 0; l < nblookuptriang; l++ )
      {
        if ( LookUpLocalTriang[ l ].code == code )
        { 
          ltri = &LookUpLocalTriang[ l ];
          break;
        }
      }
      ASSERT( ltri != NULL );
      //Unknown local configuration arround a facet vertex

      // local triangulation of the surrounding cycle of facets
      // using the pre-computed triangulation
      for ( l = 0; l < ncycle; l++ )
      {
        mpreneigh[ cycle[ l ]->id() ]->nPreNeigh[ cyclevertexn[ l ] ] =
          ltri->nNeighbor[ l ];
        for ( k = 0; k < ltri->nNeighbor[ l ]; k++ )
          mpreneigh[ cycle[ l ]->id() ]->preNeigh[ cyclevertexn[ l ] ][ k ] =
            thing[ cycle[ ltri->tabNeighbor[ l ][ k ] ]->id() ];
      }
    }
  }  
//      cout << "DEBUG : 6th part" << endl;

  // computing final neighborood from vertex-dependent neighborood
  // for all faces
  for ( it = thing.begin(); it != thing.end(); it++ )
  { 
    facet = it->second;

    int nNeighbor = 0;
    for ( j = 0; j < 4; j++ )
      nNeighbor += mpreneigh[ facet->id() ]->nPreNeigh[ j ] - 1;

    facet->setNeighbor( nNeighbor );

    counter = facet->nNeighbor() - 1;

    for ( j = 0; j < 4; j++ )
      for ( k = 0; k < mpreneigh[ facet->id() ]->nPreNeigh[ j ] - 1; k++ )
      {
        facet->pNeighbor( counter ) = mpreneigh[ facet->id() ]->preNeigh[j][k];
        counter --;
      }
  }
//        cout << "DEBUG : fin" << endl;

}
