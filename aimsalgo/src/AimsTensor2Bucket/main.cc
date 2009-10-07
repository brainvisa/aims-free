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


#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/bucket/bucket_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>
#include <aims/rgb/rgb.h>
#include <iomanip>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsTensor2Bucket [-h[elp]]                                     ",
  "                   -i[nput] <filein>                            ", 
  "                   -o[utput] <fileout>                          ",
  "                   -m[ask] <brain>                              ",
  "                  [-a[scii]]                                    ",
  "                  [-s[upplement]]                               ",
  "                  [-r[eplace]]                                  ",
  "                  [-u[ncorrected]]                              ",
  "----------------------------------------------------------------",
  "Transformation of 4D tensor image to tensor bucket              ",
  "----------------------------------------------------------------",
  "     filein     : source echo data (GIS)                        ",
  "     fileout    : destination bucket of dtitensor               ",
  "                  generates    fileout.bck                      ",
  "     brain      : U8 mask of the brain or white matter          ",
  "     ascii      : save bucket in ASCII mode                     ",
  "     supplement : save also files fileout_traceTC.ima           ",
  "                                  fileout_traceEV.ima           ",
  "                                  fileout_VR.ima                ",
  "                                  fileout_FA.ima                ",
  "                                  fileout_dir.ima               ",
  "                                  fileout_dirRGB.ima            ",
  "     replace    : when a point is NOT_POSITIVE_MATRIX, replace  ",
  "                  by an isotropic tensor which mean diffusivity ",
  "                  is estimated on the N26 neighborhood of the   ",
  "                  point rather than the mean diffusivity equal  ",
  "                  to traceTC / 3.0                              ",
  "     uncorrected: let traceEV and traceTC not corrected         ",
  "                  (they may become negative with noise)         ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main(int argc,char **argv)
{
  char *filein,*fileout,*filemask;
  int ascii=0,supplement=0,replace=0,uncorrect=0;

  AimsOption opt[] = {
  { 'h',"help"       ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input"      ,AIMS_OPT_STRING,&filein    ,0                ,1},
  { 'o',"output"     ,AIMS_OPT_STRING,&fileout   ,0                ,1},
  { 'm',"mask"       ,AIMS_OPT_STRING,&filemask  ,0                ,1},
  { 'a',"ascii"      ,AIMS_OPT_FLAG  ,&ascii     ,0                ,0},
  { 's',"supplement" ,AIMS_OPT_FLAG  ,&supplement,0                ,0},
  { 'r',"replace"    ,AIMS_OPT_FLAG  ,&replace   ,0                ,0},
  { 'u',"uncorrected",AIMS_OPT_FLAG  ,&uncorrect ,0                ,0},
  { 0  ,0            ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);


  AimsData<float>  *ima   = new AimsData<float>;
  AimsData<uint8_t> *mask = new AimsData<uint8_t>;

  Reader<AimsData<float> > dataR1( filein );
  dataR1 >> *ima;

  Reader<AimsData<uint8_t> > dataR2(filemask);
  dataR2 >> *mask;

  AimsData<float> fit,eigenvalue,matrix(3,3);

  AimsBucket<DtiTensor> bucket;
  bucket.setSizeX( ima->sizeX() );
  bucket.setSizeY( ima->sizeY() );
  bucket.setSizeZ( ima->sizeZ() );

  int dimX = ima->dimX();
  int dimY = ima->dimY();
  int dimZ = ima->dimZ();

  AimsBucketItem<DtiTensor> item;
  int x,y,z;

  int nPointNoProblem=0;
  int nPointNotPositiveMatrix=0;

  cout << "Slice : " << setw(3) << 0 << flush;
  for ( z=0; z < ima->dimZ(); z++ )
  {
    cout << "\b\b\b" << setw(3) << z+1 << flush;

    for ( y=0; y < ima->dimY(); y++ )
      for ( x=0; x < ima->dimX(); x++ )
        if ( (*mask)(x,y,z) )
        {
          item.location().item( 0 ) = short( x );
          item.location().item( 1 ) = short( y );
          item.location().item( 2 ) = short( z );

          item.value().base().coef().item(0) = (*ima)(x,y,z,0);
          item.value().base().coef().item(1) = (*ima)(x,y,z,1);
          item.value().base().coef().item(2) = (*ima)(x,y,z,2);
          item.value().base().coef().item(3) = (*ima)(x,y,z,3);
          item.value().base().coef().item(4) = (*ima)(x,y,z,4);
          item.value().base().coef().item(5) = (*ima)(x,y,z,5);

          matrix(0,0) = (*ima)(x,y,z,0);
          matrix(0,1) = (*ima)(x,y,z,1);
          matrix(0,2) = (*ima)(x,y,z,2);
          matrix(1,0) = (*ima)(x,y,z,1);
          matrix(1,1) = (*ima)(x,y,z,3);
          matrix(1,2) = (*ima)(x,y,z,4);
          matrix(2,0) = (*ima)(x,y,z,2);
          matrix(2,1) = (*ima)(x,y,z,4);
          matrix(2,2) = (*ima)(x,y,z,5);

	  AimsEigen< float > eigen;
	  eigenvalue = undiag( eigen.doit( matrix ) );

          item.value().base().eigenvalue().item(0) = eigenvalue( 0 );
          item.value().base().eigenvalue().item(1) = eigenvalue( 1 );
          item.value().base().eigenvalue().item(2) = eigenvalue( 2 );

          item.value().base().trieder().dirX().item( 0 ) = matrix( 0, 0 );
          item.value().base().trieder().dirX().item( 1 ) = matrix( 1, 0 );
          item.value().base().trieder().dirX().item( 2 ) = matrix( 2, 0 );
          item.value().base().trieder().dirY().item( 0 ) = matrix( 0, 1 );
          item.value().base().trieder().dirY().item( 1 ) = matrix( 1, 1 );
          item.value().base().trieder().dirY().item( 2 ) = matrix( 2, 1 );
          item.value().base().trieder().dirZ().item( 0 ) = matrix( 0, 2 );
          item.value().base().trieder().dirZ().item( 1 ) = matrix( 1, 2 );
          item.value().base().trieder().dirZ().item( 2 ) = matrix( 2, 2 );

          item.value().base().trace() = item.value().base().coef().item(0) +
                                        item.value().base().coef().item(3) +
                                        item.value().base().coef().item(5);
                                              
          if ( eigenvalue(0)<=0 || eigenvalue(1)<=0 || eigenvalue(2)<=0 )
          {
            item.value().location() = Point3df( 0.0, 0.0, 0.0 );
            item.value().category() = DtiTensor::NOT_POSITIVE_MATRIX;
            nPointNotPositiveMatrix++;
          }
          else
          {
            if ( eigenvalue( 0 ) > eigenvalue( 1 ) &&
                 eigenvalue( 0 ) > eigenvalue( 2 )  )
              item.value().dir() = item.value().base().trieder().dirX();
            else if ( eigenvalue( 1 ) > eigenvalue( 0 ) &&
                      eigenvalue( 1 ) > eigenvalue( 2 )  )
              item.value().dir() = item.value().base().trieder().dirY();
            else if ( eigenvalue( 2 ) > eigenvalue( 0 ) &&
                      eigenvalue( 2 ) > eigenvalue( 1)  )
              item.value().dir() = item.value().base().trieder().dirZ();


            item.value().anisotropyVR() = volumeRatio( item.value() );
            item.value().anisotropyFA() = fractionalAniso( item.value() );
            item.value().index() = -1;
            item.value().location() = Point3df( 0.0, 0.0, 0.0 );
            item.value().category() = DtiTensor::NO_PROBLEM;
            nPointNoProblem++;
          }
          bucket.push_back( item );
        }
  }
  cout << endl;

  delete mask;
  delete ima;

  cout << "total number of points : " << setw(5) << bucket[0].size() << endl;
  cout << "no problem             : " << setw(5) << nPointNoProblem  << endl;
  cout << "not positive matrix    : " << setw(5) << nPointNotPositiveMatrix 
                                                 << endl;

  cout << "building lut : " << flush;
  AimsData<DtiTensor*>* lut = new AimsData<DtiTensor*>( dimX, dimY, dimZ );
  *lut = NULL;

  list< AimsBucketItem<DtiTensor> >::iterator it;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    (*lut)( it->location().item(0),
            it->location().item(1),
            it->location().item(2) ) = &it->value();
  cout << "done" << endl;


  cout << "correcting bad points : " << flush;
  int ox=0, oy=0, oz=0, nNeigh=0;
  float meanDif=0;

  int nPointCorrectedN26=0, nPointCorrectedTC=0;
  int nPointToCorrect = nPointNotPositiveMatrix;

  int step=0;
  cout << setw(3) << step << flush;
  while ( nPointCorrectedN26 + nPointCorrectedTC != nPointToCorrect   )
  {
    cout << "\b\b\b" << setw(3) << step+1 << flush;
    for ( z = 0; z < lut->dimZ(); z++ )  
      for ( y = 0; y < lut->dimY(); y++ )  
        for ( x = 0; x < lut->dimX(); x++ )  
          if ( (*lut)(x,y,z) )
            if ( (*lut)(x,y,z)->category() > DtiTensor::PROBLEM )
            { 
              if ( ( (*lut)(x,y,z)->category() == 
                     DtiTensor::NOT_POSITIVE_MATRIX && replace)||
                   ( (*lut)(x,y,z)->category() == 
                     DtiTensor::NOT_POSITIVE_MATRIX &&
                     (*lut)(x,y,z)->base().trace() < 0) )
              {
                nNeigh = 0;
                meanDif = 0.0;
                for ( oz = -1; oz < 2; oz++ )
                  for ( oy = -1; oy < 2; oy++ )
                    for ( ox = -1; ox < 2; ox++ )
                      if ( x+ox>=0  && x+ox<dimX &&
                           y+oy>=0  && y+oy<dimY &&
                           z+oz>=0  && z+oz<dimZ &&
                           !(ox==0 && oy==0 && oz==0) )
                        if ( (*lut)( x+ox, y+oy, z+oz ) )
                        {
                          if ( (*lut)(x+ox,y+oy,z+oz)->category() <=
                               DtiTensor::NO_PROBLEM )
                          {
                            meanDif += (*lut)(x+ox,y+oy,z+oz)->base().trace();
                            nNeigh++;
                          }
                        }
                if ( nNeigh )
                {
                  if ( (*lut)(x,y,z)->category() == 
                       DtiTensor::NOT_POSITIVE_MATRIX )
                    nPointNotPositiveMatrix--;
                  (*lut)(x,y,z)->category() = DtiTensor::CORRECTED_N26;
                  nPointCorrectedN26++;
                }
                else
                {
                  cerr << endl << "warning ! : isolated point "
                       << Point3d(x,y,z) << endl;
                }
              }
              else
              {
                nNeigh  = 1;
                meanDif = (*lut)(x,y,z)->base().trace();
                (*lut)(x,y,z)->category() = DtiTensor::CORRECTED_TC;
                nPointNotPositiveMatrix--;
                nPointCorrectedTC++;
              }

              if ( nNeigh )
              {
                // replace by an isotropic tensor with eigen value equal
                // to the mean diffusivity over the N26 neighborhood
                meanDif /= 3.0 * float( nNeigh );
                (*lut)(x,y,z)->base().coef().item(0) = meanDif;
                (*lut)(x,y,z)->base().coef().item(1) = 0;
                (*lut)(x,y,z)->base().coef().item(2) = 0;
                (*lut)(x,y,z)->base().coef().item(3) = meanDif;
                (*lut)(x,y,z)->base().coef().item(4) = 0;
                (*lut)(x,y,z)->base().coef().item(5) = meanDif;

                (*lut)(x,y,z)->base().eigenvalue().item(0) = meanDif;
                (*lut)(x,y,z)->base().eigenvalue().item(1) = meanDif;
                (*lut)(x,y,z)->base().eigenvalue().item(2) = meanDif;

                (*lut)(x,y,z)->anisotropyVR() = 0.0;
                (*lut)(x,y,z)->anisotropyFA() = 0.0;

                if ( !uncorrect )
                {
                  (*lut)(x,y,z)->base().trace() = 3.0 * meanDif;
                }

                (*lut)(x,y,z)->dir() = Point3df(2*UniformRandom()-1,
                                                2*UniformRandom()-1,
                                                2*UniformRandom()-1);
                (*lut)(x,y,z)->dir() /= norm( (*lut)(x,y,z)->dir() );
                (*lut)(x,y,z)->base().trieder() = AimsOrthonormalTrieder(
                                                  (*lut)(x,y,z)->dir() );

                (*lut)(x,y,z)->index() = -1;
              }
            }
    step++;
  }
  cout << endl;


  cout << "total number of points : " << setw(5) << bucket[0].size() << endl;
  cout << "no problem             : " << setw(5) << nPointNoProblem  << endl;
  cout << "not positive matrix    : " << setw(5) << nPointNotPositiveMatrix 
                                                 << endl;
  cout << "corrected with N26     : " << setw(5) << nPointCorrectedN26 << endl;
  cout << "corrected with TC      : " << setw(5) << nPointCorrectedTC  << endl;

  delete lut;

  cout << "saving results : " << flush;
  Writer<AimsBucket<DtiTensor> > bucketW( fileout );
  bucketW.write( bucket, ascii );
  cout << "done" << endl;

  if ( supplement )
  {
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
  }

  return EXIT_SUCCESS;
}
