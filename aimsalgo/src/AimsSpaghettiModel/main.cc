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


#include <aims/vector/vector.h>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>
#include <iostream>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsSpaghettiModel -i[nput] <filein>                            ",
  "                   -o[utput] <fileout>                          ",
  "                   [--sx <sx> --sy <sy> --sz <sz>]              ",
  "                   [-h[elp]]                                    ",
  "----------------------------------------------------------------",
  "creates tensor map with elementary geometrical objects          ",
  "----------------------------------------------------------------",
  "     filein  : description file                                 ",
  "               +---------------------------------------+        ",
  "               | number of objects                     |        ",
  "               | dimx dimy dimz                        |        ",
  "            -->| CYLINDER                              |        ",
  "               | origin  0 5 17                        |        ",
  "               | axe     0 1 0                         |        ",
  "               | dir     1 1 0                         |        ",
  "               | radius  4                             |        ",
  "               | length  10                            |        ",
  "               | aniso   0.3                           |        ",
  "               | lambda1 6.5                           |        ",
  "               | deltaL  0.5                           |        ",
  "               | deltaA  30                            |        ",
  "               | random  yes                           |        ",
  "            -->| TORUS                                 |        ",
  "               | origin  5 9 17                        |        ",
  "               | axe     0 0 1                         |        ",
  "               | Radius  8                             |        ",
  "               | radius  3                             |        ",
  "               | aniso   0.8                           |        ",
  "               | lambda1 23.5                          |        ",
  "               | deltaL  0.4                           |        ",
  "               | deltaA  20                            |        ",
  "               | random  no                            |        ",
  "            -->| CUBE                                  |        ",
  "               | p0      0 0 0                         |        ",
  "               | p1      10 10 10                      |        ",
  "               | axe     1 0 0                         |        ",
  "               | aniso   0.1                           |        ",
  "               | lambda1 1.0                           |        ",
  "               | deltaL  0.1                           |        ",
  "               | deltaA  30                            |        ",
  "               | random  yes                           |        ",
  "            -->| PARALLELEPIPEDE                       |        ",
  "               | origin  0 0 0                         |        ",
  "               | v1      1 0 0                         |        ",
  "               | v2      0 1 0                         |        ",
  "               | L1      4                             |        ",
  "               | L2      8                             |        ",
  "               | L3      6                             |        ",
  "               | axe     1 0 0                         |        ",
  "               | aniso   0.1                           |        ",
  "               | lambda1 1.0                           |        ",
  "               | deltaL  0.1                           |        ",
  "               | deltaA  30                            |        ",
  "               | random  no                            |        ",
  "               +---------------------------------------+        ",
  "     fileout : output volume sequence file                      ",
  "     sx,sy,sz: voxel size [default=1.0,1.0,1.0]                 ",
  "----------------------------------------------------------------",
END_USAGE

enum Type {
  AIMS_CUBE,
  AIMS_CYLINDER,
  AIMS_TORUS,
  AIMS_PARALLELEPIPEDE
};

struct Object
{
  Type type;
  AimsVector<float,3> orig,         // origin of cylinder or torus
                      p0,           // first point of cube
                      p1,           // second point of cube
                      axe,          // main axis of the object
                      dir,          // direction of spin in cylinder
                      v1,v2;        // axis of parallelepipede
  float Radius,                     // big radius of the torus
        radius,                     // small radius of the torus or cylinder
        length,                     // length of the cylinder
        l1,l2,l3;                   // sizes of parallelepipede
  float aniso,                      // anisotropy 
        lambda1,                    // mean first eigen value
        deltaL,                     // variation of eigen values
        deltaA;                     // variation of trieder angle
  bool  random;
};


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char **argv )
{
  char *filein, *fileout = NULL;
  float sx = 1.0, sy = 1.0, sz = 1.0;

  AimsOption opt[] = {
  { 'h',"help"   ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input"  ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'o',"output" ,AIMS_OPT_STRING,&fileout    ,0                ,1},
  { ' ',"sx"     ,AIMS_OPT_FLOAT ,&sx         ,0                ,0},
  { ' ',"sy"     ,AIMS_OPT_FLOAT ,&sy         ,0                ,0},
  { ' ',"sz"     ,AIMS_OPT_FLOAT ,&sz         ,0                ,0},
  { 0  ,0        ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );


  int nObject, dimx, dimy, dimz;
  Object *vec;

  ifstream is( filein );
  ASSERT( is );

  string tmp;
  is >> nObject >> dimx >> dimy >> dimz;
  cout << nObject << " object(s)" << endl;
  cout << "dimx=" << dimx << " dimy=" << dimy << " dimz=" << dimz <<
           endl << flush;
  vec = new Object[ nObject ];
  for ( int n = 0; n < nObject; n++ )
  {
    is >> tmp;
    if ( tmp == "CYLINDER" )
    { vec[n].type = AIMS_CYLINDER;
      is >> tmp;
      if ( tmp != "origin" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].orig.item( 0 ) >> vec[n].orig.item(1) >> vec[n].orig.item(2);
      is >> tmp;
      if ( tmp != "axe" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].axe.item( 0 ) >> vec[n].axe.item(1) >> vec[n].axe.item(2);
      vec[n].axe /= norm(vec[n].axe);
      is >> tmp;
      if ( tmp != "dir" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].dir.item( 0 ) >> vec[n].dir.item(1) >> vec[n].dir.item(2);
      vec[n].dir /= norm(vec[n].dir);
      is >> tmp;
      if ( tmp != "radius" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].radius;
      is >> tmp;
      if ( tmp != "length" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].length;
    }
    else if ( tmp == "TORUS" )
    { vec[n].type = AIMS_TORUS;
      is >> tmp;
      if ( tmp != "origin" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].orig.item( 0 ) >> vec[n].orig.item(1) >> vec[n].orig.item(2);
      is >> tmp;
      if ( tmp != "axe" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].axe.item( 0 ) >> vec[n].axe.item(1) >> vec[n].axe.item(2);
      vec[n].axe /= norm(vec[n].axe);
      is >> tmp;
      if ( tmp != "Radius" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].Radius;
      is >> tmp;
      if ( tmp != "radius" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].radius;
    }
    else if ( tmp == "CUBE" )
    { vec[n].type = AIMS_CUBE;
      is >> tmp;
      if ( tmp != "p0" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].p0.item( 0 ) >> vec[n].p0.item(1) >> vec[n].p0.item(2);
      is >> tmp;
      if ( tmp != "p1" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].p1.item( 0 ) >> vec[n].p1.item(1) >> vec[n].p1.item(2);
      is >> tmp;
      if ( tmp != "axe" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].axe.item( 0 ) >> vec[n].axe.item(1) >> vec[n].axe.item(2);
      vec[n].axe /= norm(vec[n].axe);
    }
    else if ( tmp == "PARALLELEPIPEDE" )
    { vec[n].type = AIMS_PARALLELEPIPEDE;
      is >> tmp;
      if ( tmp != "origin" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].orig.item( 0 ) >> vec[n].orig.item(1) >> vec[n].orig.item(2);
      is >> tmp;
      if ( tmp != "v1" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].v1.item( 0 ) >> vec[n].v1.item(1) >> vec[n].v1.item(2);
      vec[n].v1 /= norm(vec[n].v1);
      is >> tmp;
      if ( tmp != "v2" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].v2.item( 0 ) >> vec[n].v2.item(1) >> vec[n].v2.item(2);
      vec[n].v2 /= norm(vec[n].v2);
      is >> tmp;
      if ( tmp != "L1" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].l1;
      is >> tmp;
      if ( tmp != "L2" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].l2;
      is >> tmp;
      if ( tmp != "L3" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].l3;
      is >> tmp;
      if ( tmp != "axe" ) AimsError("AimsSpaghettiModel : error in object" );
      is >> vec[n].axe.item( 0 ) >> vec[n].axe.item(1) >> vec[n].axe.item(2);
      vec[n].axe /= norm(vec[n].axe);
    }
    else
      AimsError("AimsSpaghettiModel : bad object" );

    is >> tmp;
    if ( tmp != "aniso" ) AimsError("AimsSpaghettiModel : error in object" );
    is >> vec[n].aniso;
    is >> tmp;
    if ( tmp != "lambda1" ) AimsError("AimsSpaghettiModel : error in object" );
    is >> vec[n].lambda1;
    is >> tmp;
    if ( tmp != "deltaL" ) AimsError("AimsSpaghettiModel : error in object" );
    is >> vec[n].deltaL;
    is >> tmp;
    if ( tmp != "deltaA" ) AimsError("AimsSpaghettiModel : error in object" );
    is >> vec[n].deltaA;
    is >> tmp;
    if ( tmp != "random" ) AimsError("AimsSpaghettiModel : error in object" );
    is >> tmp;
    if ( tmp == "yes" ) vec[n].random = true;
    else              vec[n].random = false;
  }


  AimsData<float> sqv(dimx,dimy,dimz,6);
  AimsData<byte> mask(dimx,dimy,dimz);
  Trieder trieder;
  AimsVector<float,3> dir;
  float l1,l2,l3;
  AimsData<float> tensor;

  sqv.setSizeXYZT( sx, sy, sz, 1.0 );
  mask.setSizeXYZT( sx, sy, sz, 1.0 );

  mask = 0;

  for (int n=0;n<nObject;n++)
  { switch (vec[n].type)
    { case AIMS_CYLINDER : 

        for (int z=0;z<dimz;z++)
          for (int y=0;y<dimy;y++)
            for (int x=0;x<dimx;x++)
              if (AimsIsInCylinder(AimsVector<float,3>(x,y,z),
                                   vec[n].orig,vec[n].axe,
                                   vec[n].radius,vec[n].length) == true)
              { if (vec[n].random == true)
                { dir.item( 0 ) = UniformRandom(-1.0,1.0);
                  dir.item(1) = UniformRandom(-1.0,1.0);
                  dir.item(2) = UniformRandom(-1.0,1.0);
                  dir /= norm(dir);
                }
                else
                  dir = vec[n].dir;
                l2 = l3 = (vec[n].lambda1 / 2.0) * 
                          (1.0 / (2.0*vec[n].aniso/3.0 + 1.0/3.0) - 1.0);
                l1 = max(vec[n].lambda1 +
                         UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l2 = max(l2 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l3 = max(l3 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                dir = AimsPointInSolidAngle(dir,vec[n].deltaA * M_PI / 180.0);
                trieder = AimsOrthonormalTrieder(dir);
                tensor = AimsEigenToMatrix(trieder,l1,l2,l3);
                sqv(x,y,z,0) = tensor(0,0);
                sqv(x,y,z,1) = tensor(1,0);
                sqv(x,y,z,2) = tensor(2,0);
                sqv(x,y,z,3) = tensor(1,1);
                sqv(x,y,z,4) = tensor(2,1);
                sqv(x,y,z,5) = tensor(2,2);
                mask(x,y,z)  = (byte)(n+1);
              }
        break;

      case AIMS_TORUS    : 

        for (int z=0;z<dimz;z++)
          for (int y=0;y<dimy;y++)
            for (int x=0;x<dimx;x++)
              if (AimsIsInTorus(AimsVector<float,3>(x,y,z),
                                   vec[n].orig,vec[n].axe,
                                   vec[n].Radius,vec[n].radius,&dir) == true)
              { if (vec[n].random == true)
                { dir.item( 0 ) = UniformRandom(-1.0,1.0);
                  dir.item(1) = UniformRandom(-1.0,1.0);
                  dir.item(2) = UniformRandom(-1.0,1.0);
                  dir /= norm(dir);
                }
                l2 = l3 = (vec[n].lambda1 / 2.0) * 
                          (1.0 / (2.0*vec[n].aniso/3.0 + 1.0/3.0) - 1.0);
                l1 = max(vec[n].lambda1 +
                         UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l2 = max(l2 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l3 = max(l3 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                dir = AimsPointInSolidAngle(dir,vec[n].deltaA * M_PI / 180.0);
                trieder = AimsOrthonormalTrieder(dir);
                tensor = AimsEigenToMatrix(trieder,l1,l2,l3);
                sqv(x,y,z,0) = tensor(0,0);
                sqv(x,y,z,1) = tensor(0,1);
                sqv(x,y,z,2) = tensor(0,2);
                sqv(x,y,z,3) = tensor(1,1);
                sqv(x,y,z,4) = tensor(1,2);
                sqv(x,y,z,5) = tensor(2,2);
                mask(x,y,z)  = (byte)(n+1);
              }
        break;

      case AIMS_CUBE     : 

        for (int z=0;z<dimz;z++)
          for (int y=0;y<dimy;y++)
            for (int x=0;x<dimx;x++)
              if (AimsIsInCube(AimsVector<float,3>(x,y,z),
                               vec[n].p0,vec[n].p1)      == true)
              { if (vec[n].random == true)
                { dir.item( 0 ) = UniformRandom(-1.0,1.0);
                  dir.item(1) = UniformRandom(-1.0,1.0);
                  dir.item(2) = UniformRandom(-1.0,1.0);
                  dir /= norm(dir);
                }
                else
                  dir = vec[n].axe;
                l2 = l3 = (vec[n].lambda1 / 2.0) * 
                          (1.0 / (2.0*vec[n].aniso/3.0 + 1.0/3.0) - 1.0);
                l1 = max(vec[n].lambda1 +
                         UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l2 = max(l2 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l3 = max(l3 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                dir = AimsPointInSolidAngle(dir,vec[n].deltaA * M_PI / 180.0);
                trieder = AimsOrthonormalTrieder(dir);
                tensor = AimsEigenToMatrix(trieder,l1,l2,l3);
                sqv(x,y,z,0) = tensor(0,0);
                sqv(x,y,z,1) = tensor(0,1);
                sqv(x,y,z,2) = tensor(0,2);
                sqv(x,y,z,3) = tensor(1,1);
                sqv(x,y,z,4) = tensor(1,2);
                sqv(x,y,z,5) = tensor(2,2);
                mask(x,y,z)  = (byte)(n+1);
              }
        break;

      case AIMS_PARALLELEPIPEDE     : 

        for (int z=0;z<dimz;z++)
          for (int y=0;y<dimy;y++)
            for (int x=0;x<dimx;x++)
              if (AimsIsInParallelepipede(AimsVector<float,3>(x,y,z),
                                          vec[n].orig,
                                          vec[n].v1,vec[n].v2,
                                          vec[n].l1,vec[n].l2,
                                          vec[n].l3) == true)
              { if (vec[n].random == true)
                { dir.item( 0 ) = UniformRandom(-1.0,1.0);
                  dir.item(1) = UniformRandom(-1.0,1.0);
                  dir.item(2) = UniformRandom(-1.0,1.0);
                  dir /= norm(dir);
                }
                else
                  dir = vec[n].axe;
                l2 = l3 = (vec[n].lambda1 / 2.0) * 
                          (1.0 / (2.0*vec[n].aniso/3.0 + 1.0/3.0) - 1.0);
                l1 = max(vec[n].lambda1 +
                         UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l2 = max(l2 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                l3 = max(l3 + UniformRandom(-vec[n].deltaL,vec[n].deltaL),
                         (float)0.0);
                dir = AimsPointInSolidAngle(dir,vec[n].deltaA * M_PI / 180.0);
                trieder = AimsOrthonormalTrieder(dir);
                tensor = AimsEigenToMatrix(trieder,l1,l2,l3);
                sqv(x,y,z,0) = tensor(0,0);
                sqv(x,y,z,1) = tensor(0,1);
                sqv(x,y,z,2) = tensor(0,2);
                sqv(x,y,z,3) = tensor(1,1);
                sqv(x,y,z,4) = tensor(1,2);
                sqv(x,y,z,5) = tensor(2,2);
                mask(x,y,z)  = (byte)(n+1);
              }
        break;
    }
  }

  Writer<AimsData<float> > writer1(fileout);
  writer1 << sqv;
  Writer<AimsData<byte> > writer2(string(fileout)+"_mask" );
  writer2 << mask;

  delete[](vec);

  return EXIT_SUCCESS;
}
