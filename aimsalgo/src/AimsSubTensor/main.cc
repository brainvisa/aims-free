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


#include <aims/bucket/bucket_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>
#include <iomanip>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsSubTensor [-h[elp]]                                         ",
  "               -i[nput] <filein>                                ", 
  "               -o[utput] <fileout>                              ",
  "              [-x <xmin> -y <ymin> -z <zmin>                    ",
  "               -X <xmax> -Y <ymax> -Z <zmax>]                   ",
  "              [-m<ask> <bucket>]                                ",
  "              [-a[scii]]                                        ",
  "----------------------------------------------------------------",
  "Extracting a subpart from a DtiTensor bucket                    ",
  "----------------------------------------------------------------",
  "     filein         : source tensor bucket                      ",
  "     fileout        : destination bucket of sub-dtitensor       ",
  "                      generates    fileout.bck                  ",
  "     xmin,ymin,zmin : inferior point of the box sub-part        ",
  "     xmax,ymax,zmax : superior point of the box sub-part        ",
  "     bucket         : Void bucket mask                          ",
  "     ascii          : write in ascii mode rather than binary    ",
  "                      [default=binary]                          ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}



int main( int argc, char **argv )
{
  char *filein = NULL, *fileout = NULL, *fileMask = NULL;
  short xmin = -1, xmax = -1, ymin = -1, ymax = -1, zmin = -1, zmax = -1;
  int ascii = 0;

  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input"     ,AIMS_OPT_STRING,&filein    ,0                ,1},
  { 'o',"output"    ,AIMS_OPT_STRING,&fileout   ,0                ,1},
  { 'x',"x"         ,AIMS_OPT_SHORT ,&xmin      ,0                ,0},
  { 'y',"y"         ,AIMS_OPT_SHORT ,&ymin      ,0                ,0},
  { 'z',"z"         ,AIMS_OPT_SHORT ,&zmin      ,0                ,0},
  { 'X',"X"         ,AIMS_OPT_SHORT ,&xmax      ,0                ,0},
  { 'Y',"Y"         ,AIMS_OPT_SHORT ,&ymax      ,0                ,0},
  { 'Z',"Z"         ,AIMS_OPT_SHORT ,&zmax      ,0                ,0},
  { 'm',"mask"      ,AIMS_OPT_STRING,&fileMask  ,0                ,0},
  { 'a',"ascii"     ,AIMS_OPT_FLAG  ,&ascii     ,0                ,0},
  { 0  ,0           ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  cout << "reading " << filein << " bucket : " << flush;
  AimsBucket<DtiTensor> bucket;
  Reader<AimsBucket<DtiTensor> > bucketR( filein );
  bucketR >> bucket;
  cout << "done" << endl;



  AimsData<byte>* mask = NULL;
  int dimX = 0, dimY = 0, dimZ = 0;

  if ( fileMask )
  {
    cout << "reading " << fileMask << " void bucket : " << flush;
    AimsBucket<Void> bucketM;
    Reader<AimsBucket<Void> > bucketRM( fileMask );
    bucketRM >> bucketM;
    list< AimsBucketItem<DtiTensor> >::iterator it;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    {
      if ( it->location().item(0) > dimX )
        dimX = it->location().item(0);
      if ( it->location().item(1) > dimY )
        dimY = it->location().item(1);
      if ( it->location().item(2) > dimZ )
        dimZ = it->location().item(2);
    }
    mask = new AimsData<byte>( ++dimX, ++dimY, ++dimZ );
    *mask = false;
    list< AimsBucketItem<Void> >::iterator vi;
    for ( vi = bucketM[0].begin(); vi != bucketM[0].end(); vi++ )
      (*mask)( vi->location().item(0),
               vi->location().item(1),
               vi->location().item(2) ) = true;

    cout << "done" << endl;
  }
  cout << "dimX=" << dimX << endl
       << "dimY=" << dimY << endl
       << "dimZ=" << dimZ << endl;
  
  AimsBucket<DtiTensor> subpart;

  subpart.setSizeX( bucket.sizeX() );
  subpart.setSizeY( bucket.sizeY() );
  subpart.setSizeZ( bucket.sizeZ() );
  subpart.setSizeT( bucket.sizeT() );


  cout << "filling subpart : " << flush;
  if ( fileMask == NULL )
  {
    list< AimsBucketItem<DtiTensor> >::iterator it;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( it->location().item(0) >= xmin &&
           it->location().item(0) <= xmax &&
           it->location().item(1) >= ymin &&
           it->location().item(1) <= ymax &&
           it->location().item(2) >= zmin &&
           it->location().item(2) <= zmax   )
        subpart.push_back( *it );
  }
  else
  {
    list< AimsBucketItem<DtiTensor> >::iterator it;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( (*mask)( it->location().item(0),
                    it->location().item(1),
                    it->location().item(2) ) == true )
        subpart.push_back( *it );
    delete mask;
  }
  cout << "done" << endl;


  cout << "writing " << fileout << " bucket : " << flush;
  Writer<AimsBucket<DtiTensor> > bucketW( fileout );
  bucketW.write( subpart, ascii );
  cout << "done" << endl;


  return EXIT_SUCCESS;
}
