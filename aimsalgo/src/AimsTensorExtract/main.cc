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


#include <aims/data/data_g.h>
#include <aims/bucket/bucket_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>
#include <aims/rgb/rgb.h>
#include <iostream>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsTensorExtract [-h[elp]]                                     ",
  "                   -i[nput] <filein>                            ", 
  "                  [-o[utput] <fileout>]                         ",
  "                   -x[dim] <dimX>                               ",
  "                   -y[dim] <dimY>                               ",
  "                   -z[dim] <dimZ>                               ",
  "----------------------------------------------------------------",
  "Extract anisotropy,trace,directions from dtitensor bucket       ",
  "----------------------------------------------------------------",
  "     filein     : source dtitensor bucket                       ",
  "     fileout    : destination base file name                    ",
  "                                  fileout_traceTC.ima           ",
  "                                  fileout_traceEV.ima           ",
  "                                  fileout_VR.ima                ",
  "                                  fileout_FA.ima                ",
  "                                  fileout_dir.ima               ",
  "                                  fileout_dirRGB.ima            ",
  "                  [default=filein]                              ",
  "     dimX,dimY,dimZ : dimensions of output images               ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char **argv )
{
  char *filein = NULL, *fileout = NULL;
  int dimX = 0, dimY = 0, dimZ = 0;

  AimsOption opt[] = {
  { 'h',"help"       ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input"      ,AIMS_OPT_STRING,&filein    ,0                ,1},
  { 'o',"output"     ,AIMS_OPT_STRING,&fileout   ,0                ,0},
  { 'x',"xdim"       ,AIMS_OPT_INT   ,&dimX      ,0                ,1},
  { 'y',"ydim"       ,AIMS_OPT_INT   ,&dimY      ,0                ,1},
  { 'z',"zdim"       ,AIMS_OPT_INT   ,&dimZ      ,0                ,1},
  { 0  ,0            ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if ( fileout == NULL )
    fileout = filein;

  AimsBucket<DtiTensor> bucket;
  list< AimsBucketItem<DtiTensor> >::iterator it;

  cout << "reading tensor bucket : " << flush;
  Reader<AimsBucket<DtiTensor> >	bucketR( filein );
  bucketR >> bucket;
  cout << "done" << endl;

  cout << "saving trace  : " << flush;
  Writer<AimsData<float> > dataW0( string( fileout ) + "_trace" );
  AimsData<float>* trace = new AimsData<float>( dimX, dimY, dimZ );
  trace->setSizeX( bucket.sizeX() );
  trace->setSizeY( bucket.sizeY() );
  trace->setSizeZ( bucket.sizeZ() );
  *trace = 0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*trace)( it->location().item(0),
                it->location().item(1),
                it->location().item(2) ) = it->value().base().trace();
  dataW0 << *trace;
  delete trace;
  cout << "done" << endl;

  cout << "saving volume ratio : " << flush;
  Writer<AimsData<float> > dataW2( string( fileout ) + "_VR" );
  AimsData<float>* vr = new AimsData<float>( dimX, dimY, dimZ );
  vr->setSizeX( bucket.sizeX() );
  vr->setSizeY( bucket.sizeY() );
  vr->setSizeZ( bucket.sizeZ() );
  *vr = 0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*vr)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = it->value().anisotropyVR();
  dataW2 << *vr;
  delete vr;
  cout << "done" << endl;

  cout << "saving fractional anisotropy : " << flush;
  Writer<AimsData<float> > dataW3( string( fileout ) + "_FA" );
  AimsData<float>* fa = new AimsData<float>( dimX, dimY, dimZ );
  fa->setSizeX( bucket.sizeX() );
  fa->setSizeY( bucket.sizeY() );
  fa->setSizeZ( bucket.sizeZ() );
  *fa = 0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*fa)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = it->value().anisotropyFA();
  dataW3 << *fa;
  delete fa;
  cout << "done" << endl;

  cout << "saving direction : " << flush;
  Writer<AimsData<Point3df> > dataW4( string( fileout ) + "_dir" );
  AimsData<Point3df>* dir = new AimsData<Point3df>( dimX, dimY, dimZ );
  dir->setSizeX( bucket.sizeX() );
  dir->setSizeY( bucket.sizeY() );
  dir->setSizeZ( bucket.sizeZ() );
  *dir = Point3df(0,0,0);
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*dir)( it->location().item(0),
              it->location().item(1),
              it->location().item(2) ) = it->value().dir();
  dataW4 << *dir;
  delete dir;
  cout << "done" << endl;

  cout << "saving direction in RGB : " << flush;
  Writer<AimsData<AimsRGB> > dataW5( string( fileout ) + "_dirRGB" );
  AimsData<AimsRGB>* dirRGB = new AimsData<AimsRGB>( dimX, dimY, dimZ );
  dirRGB->setSizeX( bucket.sizeX() );
  dirRGB->setSizeY( bucket.sizeY() );
  dirRGB->setSizeZ( bucket.sizeZ() );
  *dirRGB = AimsRGB(0,0,0);
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
    {
      (*dirRGB)( it->location().item(0),
                 it->location().item(1),
                 it->location().item(2) ).red() = 
              byte( fabs( it->value().dir().item(0) ) * 255.0 );
      (*dirRGB)( it->location().item(0),
                 it->location().item(1),
                 it->location().item(2) ).green() = 
              byte( fabs( it->value().dir().item(1) ) * 255.0 );
      (*dirRGB)( it->location().item(0),
                 it->location().item(1),
                 it->location().item(2) ).blue() = 
              byte( fabs( it->value().dir().item(2) ) * 255.0 );
    }
  dataW5 << *dirRGB;
  delete dirRGB;
  cout << "done" << endl;

  return EXIT_SUCCESS;
}
