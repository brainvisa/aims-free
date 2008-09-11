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


#include <aims/getopt/getopt.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/math/math_g.h>
#include <cartobase/stream/directory.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace aims;
using namespace carto;
using namespace std;


BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsHJEvolution3d  -i[nput] <filein>                                      ",
  "                   -o[output] <fileout>                                   ",
  "                  [-d[istmap] <filedist>                                  ",
  "                   -e[poch] <epoch>                                       ",
  "                  [-t[ime] <timestep>]                                    ",
  "                  [-f[0] <F0>]                                            ",
  "                   -F[1] <F1>                                             ",
  "                  [-s[tep] <savingstep>]                                  ",
  "                  [-u[radius] <tuberadius>]                               ",
  "                  [-c[urvature] <typeCurv>]                               ",
  "                  [-m[orphology] <typeMorpho>]                            ",
  "                  [-h[elp]]                                               ",
  "--------------------------------------------------------------------------",
  "3D Hamilton-Jacobi evolution of a binary short image f(x,y,z)             ",
  "Fast tubular method                                                       ",
  "--------------------------------------------------------------------------",
  "  filein    : input SHORT binary volume                                   ",
  "  fileout   : prefix name for output SHORT volumes                        ",
  "  filedist  : distance map volume                                         ",
  "  epoch     : number of evolving iterations to proceed                    ",
  "  timestep  : time step in seconds [default=0.1]                          ",
  "  F0,F1     : speed function F = F0 + F1.K [default F0=-1.0 / F1=-1.0]    ",
  "              F0<0 : erosion  / F0>0 : dilation                           ",
  "  savingtep : number of time iterations between 2 backups [default=1]     ",
  "  uradius   : radius of the evolving tube [default=12]                    ",
  "  typeCurv  : mean, gaussian, mean-gaussian, min, max                     ",
  "                                            [default=mean-gaussian]       ",
  "  typeMorpho: type of morphologic evolution                               ",
  "                   0 -> -F0.||DPhi||                                      ",
  "                   1 -> -F0.||DPhi||.dist(x,y,z)                          ",
  "                   2 -> -F0.||DPhi||.(1.0 + 0.5 * dist(x,y,z))            ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


inline float MINABS( float x, float y )
{
  if ( x * y < 0)
    return 0.0;
  else
  {
    if ( fabs( x ) <= fabs( y ) )
      return x;
    else
      return y;
  }
}


int main( int argc, char *argv[] )
{
  char  *filein=NULL,*fileout=NULL,*filedist=NULL;
  int   N=0;
  float deltat=0.1;
  float F0=-1.0;
  float F1=-1.0;
  int   step=1;
  float tubeRadius=12.0;
  int   x=0,y=0,z=0,n=0,xx=0,yy=0,zz=0;
  char* typeCurv=NULL;
  int   typeMorpho=0;
  bool  meanCurv=false, gaussianCurv=false, meanGaussianCurv=false,
        minCurv=false, maxCurv=false;
  float h=0.0, h2=0.0, criterion=0.0, Deltat=0.0, hh=0.0;

  //
  // Getting options
  //
  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"     ,AIMS_OPT_STRING,&filein    ,0                ,1},
  { 'o',"output"    ,AIMS_OPT_STRING,&fileout   ,0                ,1},
  { 'd',"distmap"   ,AIMS_OPT_STRING,&filedist  ,0                ,0},
  { 'e',"epoch"     ,AIMS_OPT_INT   ,&N         ,0                ,1},
  { 't',"t"         ,AIMS_OPT_FLOAT ,&deltat    ,0                ,0},
  { 'f',"f0"        ,AIMS_OPT_FLOAT ,&F0        ,0                ,0},
  { 'F',"F1"        ,AIMS_OPT_FLOAT ,&F1        ,0                ,0},
  { 's',"step"      ,AIMS_OPT_INT   ,&step      ,0                ,0},
  { 'u',"uradius"   ,AIMS_OPT_FLOAT ,&tubeRadius,0                ,0},
  { 'c',"curvature" ,AIMS_OPT_STRING,&typeCurv  ,0                ,0},
  { 'm',"morphology",AIMS_OPT_INT   ,&typeMorpho,0                ,0},
  { 0  ,0           ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);

  ASSERT( F1 <= 0.0 );
  ASSERT( typeMorpho>=0 && typeMorpho<=2 );

  if ( typeCurv != NULL )
  {
    ASSERT( string( typeCurv ) == "mean"         ||
            string( typeCurv ) == "gaussian"     ||
            string( typeCurv ) == "mean-gaussian"||
            string( typeCurv ) == "min"          ||
            string( typeCurv ) == "max"            );
    if ( string( typeCurv ) == "mean" )
      meanCurv = true;
    else if ( string( typeCurv ) == "gaussian" )
      gaussianCurv = true;
    else if ( string( typeCurv ) == "mean-gaussian" )
      meanGaussianCurv = true;
    else if ( string( typeCurv ) == "min" )
      minCurv = true;
    else if ( string( typeCurv ) == "max" )
      maxCurv = true;
  }
  else meanGaussianCurv = true;

  //
  // reads image and binarizes it
  //
  AimsData<short> *bin = new AimsData<short>;

  Reader<AimsData<short> > reader(filein);
  if( !reader.read(*bin,1) )
    AimsUsage(usage);

  bin->fillBorder(0);
  int  X=0,Y=0,Z=0;
  X = bin->dimX();
  Y = bin->dimY();
  Z = bin->dimZ();
  float DX=0,DY=0,DZ=0;
  DX = bin->sizeX();
  DY = bin->sizeY();
  DZ = bin->sizeZ();

  h = ( DX + DY + DZ ) / 3.0;
  h2 = sqr( h );
  Deltat = h / 10.0;
  criterion = Deltat * cub( h );

  for (z=0;z<bin->dimZ();z++)
    for (y=0;y<bin->dimY();y++)
      for (x=0;x<bin->dimX();x++)
        (*bin)(x,y,z) = ((*bin)(x,y,z)<1) ? 0 : 32767;


  //
  // read distance map
  //
  AimsData<float>* dist = NULL;
  if ( typeMorpho == 1 || typeMorpho == 2 )
  {
    ASSERT( filedist );
    dist = new AimsData<float>;
    Reader<AimsData<float> > dataR( filedist );
    dataR >> *dist;
    ASSERT( dist->dimX() == X &&
            dist->dimY() == Y &&
            dist->dimZ() == Z &&
            dist->dimT() == 1   );
  }


  //
  // Gets the signed distance map
  //
  cout << endl << "processing distance map" << endl;
  AimsData<float> *f = new AimsData<float>;
  f->setSizeX(bin->sizeX());
  f->setSizeY(bin->sizeY());
  f->setSizeZ(bin->sizeZ());
  *f = AimsFloatSignedChamferDistanceMap(*bin);
  f->setSizeX(bin->sizeX());
  f->setSizeY(bin->sizeY());
  f->setSizeZ(bin->sizeZ());
  ForEach3d( (*f), x , y , z )
    (*f)(x,y,z) = -(*f)(x,y,z);


  //
  // Deletes binary input image
  //
  delete(bin);


  //
  // Prepares a temporary result image
  //
  AimsData<float> *newf = new AimsData<float>(X,Y,Z);
  newf->setSizeX(f->sizeX());
  newf->setSizeY(f->sizeY());
  newf->setSizeZ(f->sizeZ());
  for (z=0;z<Z;z++)
    for (y=0;y<Y;y++)
      for (x=0;x<X;x++)
         if ( x<2    || y<2    || z<2   ||
              x>=X-2 || y>=Y-2 || z>=Z-2 )
            (*newf)(x,y,z) = (*f)(x,y,z);

  //
  // Prepares a initial image for distance recovery
  //
  AimsData<float> *f0 = new AimsData<float>(X,Y,Z);
  f0->setSizeX(f->sizeX());
  f0->setSizeY(f->sizeY());
  f0->setSizeZ(f->sizeZ());
  

  //
  // Binary BYTE image to set points that are in the tube
  //
  int count = 0;
  AimsData<byte>  *flag = new AimsData<byte>(X,Y,Z);
  for (z=0;z<Z;z++)
    for (y=0;y<Y;y++)
      for (x=0;x<X;x++)
        if (fabs((*f)(x,y,z))<tubeRadius)
        { (*flag)(x,y,z) = 1;
          count++;
        }
        else (*flag)(x,y,z) = 0;
  cout << endl << count << " points in the tube" << endl << flush;


  //
  // Evolution loop
  //
  AimsData<float> *tmp;
  float dx =0,dy =0,dz =0,
        dxx=0,dyy=0,dzz=0,
        dxy=0,dxz=0,dyz=0,
        curvature=0,curvature_m=0,curvature_g=0,diffusion=0;
  float Dx_m1=0,Dx_m2=0,Dx_p1=0,Dx_p2=0,
        Dy_m1=0,Dy_m2=0,Dy_p1=0,Dy_p2=0,
        Dz_m1=0,Dz_m2=0,Dz_p1=0,Dz_p2=0;
  float Dx_m1_x_m1=0,Dx_p1_x_p1=0,Dx_m1_x_p1=0,
        Dy_m1_y_m1=0,Dy_p1_y_p1=0,Dy_m1_y_p1=0,
        Dz_m1_z_m1=0,Dz_p1_z_p1=0,Dz_m1_z_p1=0;
  float aa=0,bb=0,cc=0,dd=0,ee=0,ff=0;
  float A=0,B=0,C=0,D=0,E=0,F=0;
  float delta_plus=0,delta_minus=0,morpho=0;
  float S=0,G=0;
  int   t=0;
  float error=0.0;

  cout << "\nEuler integration time step : " << setw(6) << 0 
                                             << "  ..." << flush;

  for (n=0;n<N;n++)
  {
    cout << "\b\b\b\b\b\b\b\b\b\b\b" << setw(6) << n+1 << "  ..." << flush;

    //
    // Gets evolution
    //
    for (z=2;z<Z-2;z++)
      for (y=2;y<Y-2;y++)
        for (x=2;x<X-2;x++)
          // If point is in the tube
          if ((*flag)(x,y,z)) 
          { //
            // Diffusion term
            //
            if ((*f)(x-1,y  ,z  ) != (*f)(x+1,y  ,z  ) || 
                (*f)(x  ,y-1,z  ) != (*f)(x  ,y+1,z  ) || 
                (*f)(x  ,y  ,z-1) != (*f)(x  ,y  ,z+1)   )
            {
              dx    = ( (*f)(x+1,y  ,z  ) - (*f)(x-1,y  ,z  ) ) / ( 2.0 * DX );
              dy    = ( (*f)(x  ,y+1,z  ) - (*f)(x  ,y-1,z  ) ) / ( 2.0 * DY );
              dz    = ( (*f)(x  ,y  ,z+1) - (*f)(x  ,y  ,z-1) ) / ( 2.0 * DZ );

              dxx   = ( (*f)(x+2,y,z) - 2.0 * (*f)(x,y,z) + 
                        (*f)(x-2,y,z) ) / ( 4.0 * DX * DX );
              dyy   = ( (*f)(x,y+2,z) - 2.0 * (*f)(x,y,z) + 
                        (*f)(x,y-2,z) ) / ( 4.0 * DY * DY );
              dzz   = ( (*f)(x,y,z+2) - 2.0 * (*f)(x,y,z) + 
                        (*f)(x,y,z-2) ) / ( 4.0 * DZ * DZ );
              dxy   = ( (*f)(x+1,y+1,z) - (*f)(x-1,y+1,z) - 
                        (*f)(x+1,y-1,z) + (*f)(x-1,y-1,z) ) /
                      ( 4.0 * DX * DY );
              dxz   = ( (*f)(x+1,y,z+1) - (*f)(x-1,y,z+1) - 
                        (*f)(x+1,y,z-1) + (*f)(x-1,y,z-1) ) /
                      ( 4.0 * DX * DZ );
              dyz   = ( (*f)(x,y+1,z+1) - (*f)(x,y-1,z+1) - 
                        (*f)(x,y+1,z-1) + (*f)(x,y-1,z-1) ) /
                      ( 4.0 * DY * DZ );
              if ( meanCurv )
              {
                hh = sqrt( dx*dx + dy*dy + dz*dz ); 
                curvature = ( (dyy+dzz)*dx*dx +
                              (dxx+dzz)*dy*dy +
                              (dxx+dyy)*dz*dz
                              - 2*dx*dy*dxy
                              - 2*dx*dz*dxz
                              - 2*dy*dz*dyz ) / ( 2.0 * cub( hh ) );
              }
              else if ( minCurv )
              {
                hh = sqrt( dx*dx + dy*dy + dz*dz ); 
                curvature = ( (dyy+dzz)*dx*dx +
                              (dxx+dzz)*dy*dy +
                              (dxx+dyy)*dz*dz
                              - 2*dx*dy*dxy
                              - 2*dx*dz*dxz
                              - 2*dy*dz*dyz ) / ( 2.0 * cub( hh ) );
                curvature = min( curvature, 0.0f );
              }
              else if ( maxCurv )
              {
                hh = sqrt( dx*dx + dy*dy + dz*dz ); 
                curvature = ( (dyy+dzz)*dx*dx +
                              (dxx+dzz)*dy*dy +
                              (dxx+dyy)*dz*dz
                              - 2*dx*dy*dxy
                              - 2*dx*dz*dxz
                              - 2*dy*dz*dyz ) / ( 2.0 * cub( hh ) );
                curvature = max( curvature, 0.0f );
              }
              else if ( gaussianCurv )
              {
                curvature = ( dx*dx*(dyy*dzz-dyz*dyz) +
                              dy*dy*(dxx*dzz-dxz*dxz) + 
                              dz*dz*(dxx*dyy-dxy*dxy) + 2.0 * 
                              ( dx*dy*(dxz*dyz-dxy*dzz) +
                                dy*dz*(dxy*dxz-dyz*dxx) +
                                dx*dz*(dxy*dyz-dxz*dyy) ) ) /
                            sqr( dx*dx + dy*dy + dz*dz );
              }
              else
              {
                hh = sqrt( dx*dx + dy*dy + dz*dz ); 
                curvature_m = ( (dyy+dzz)*dx*dx +
                                (dxx+dzz)*dy*dy +
                                (dxx+dyy)*dz*dz
                                - 2*dx*dy*dxy
                                - 2*dx*dz*dxz
                                - 2*dy*dz*dyz ) / ( 2.0 * cub( hh ) );
                curvature_g = ( dx*dx*(dyy*dzz-dyz*dyz) +
                                dy*dy*(dxx*dzz-dxz*dxz) + 
                                dz*dz*(dxx*dyy-dxy*dxy) + 2.0 * 
                                ( dx*dy*(dxz*dyz-dxy*dzz) +
                                  dy*dz*(dxy*dxz-dyz*dxx) +
                                  dx*dz*(dxy*dyz-dxz*dyy) ) ) /
                              sqr( dx*dx + dy*dy + dz*dz );
                curvature = ( curvature_m < 0.0 ? -1.0 : +1.0 ) *
                            ( curvature_g < 0.0 ? 0.0 : sqrt(2.0*curvature_g) ); 
              }
            }
            else curvature = 0.0;

            diffusion = - F1 * curvature * sqrt(dx*dx + dy*dy + dz*dz);


            //
            // Morphological term
            //
            Dx_m1 = ( (*f)(x  ,y,z) - (*f)(x-1,y,z) ) / DX;
            Dx_m2 = ( (*f)(x-1,y,z) - (*f)(x-2,y,z) ) / DX;
            Dx_p1 = ( (*f)(x+1,y,z) - (*f)(x  ,y,z) ) / DX;
            Dx_p2 = ( (*f)(x+2,y,z) - (*f)(x+1,y,z) ) / DX;
          
            Dy_m1 = ( (*f)(x,y  ,z) - (*f)(x,y-1,z) ) / DY;
            Dy_m2 = ( (*f)(x,y-1,z) - (*f)(x,y-2,z) ) / DY;
            Dy_p1 = ( (*f)(x,y+1,z) - (*f)(x,y  ,z) ) / DY;
            Dy_p2 = ( (*f)(x,y+2,z) - (*f)(x,y+1,z) ) / DY;

            Dz_m1 = ( (*f)(x,y,z  ) - (*f)(x,y,z-1) ) / DZ;
            Dz_m2 = ( (*f)(x,y,z-1) - (*f)(x,y,z-2) ) / DZ;
            Dz_p1 = ( (*f)(x,y,z+1) - (*f)(x,y,z  ) ) / DZ;
            Dz_p2 = ( (*f)(x,y,z+2) - (*f)(x,y,z+1) ) / DZ;


            Dx_m1_x_m1 = ( Dx_m1 - Dx_m2 ) / DX;
            Dx_p1_x_p1 = ( Dx_p2 - Dx_p1 ) / DX;
            Dx_m1_x_p1 = ( Dx_p1 - Dx_m1 ) / DX;

            Dy_m1_y_m1 = ( Dy_m1 - Dy_m2 ) / DY;
            Dy_p1_y_p1 = ( Dy_p2 - Dy_p1 ) / DY;
            Dy_m1_y_p1 = ( Dy_p1 - Dy_m1 ) / DY;

            Dz_m1_z_m1 = ( Dz_m1 - Dz_m2 ) / DZ;
            Dz_p1_z_p1 = ( Dz_p2 - Dz_p1 ) / DZ;
            Dz_m1_z_p1 = ( Dz_p1 - Dz_m1 ) / DZ;


            A = Dx_m1 + 0.5 * MINABS( Dx_m1_x_m1 , Dx_m1_x_p1 );
            B = Dx_p1 + 0.5 * MINABS( Dx_p1_x_p1 , Dx_m1_x_p1 );
            C = Dy_m1 + 0.5 * MINABS( Dy_m1_y_m1 , Dy_m1_y_p1 );
            D = Dy_p1 + 0.5 * MINABS( Dy_p1_y_p1 , Dy_m1_y_p1 );
            E = Dz_m1 + 0.5 * MINABS( Dz_m1_z_m1 , Dz_m1_z_p1 );
            F = Dz_p1 + 0.5 * MINABS( Dz_p1_z_p1 , Dz_m1_z_p1 );


            if (F0 > 0)
            {
              aa = (A>0 ? A : 0);
              bb = (B<0 ? B : 0);
              cc = (C>0 ? C : 0);
              dd = (D<0 ? D : 0);
              ee = (E>0 ? E : 0);
              ff = (F<0 ? F : 0);

              delta_plus = sqrt(aa*aa + bb*bb + cc*cc + dd*dd + ee*ee + ff*ff);
              morpho = -F0 * delta_plus;
            }
            else
            { 
              aa = (A<0 ? A : 0);
              bb = (B>0 ? B : 0);
              cc = (C<0 ? C : 0);
              dd = (D>0 ? D : 0);
              ee = (E<0 ? E : 0);
              ff = (F>0 ? F : 0);

              delta_minus = sqrt(aa*aa + bb*bb + cc*cc + dd*dd + ee*ee + ff*ff);

              morpho = -F0 * delta_minus;
            }
            if ( typeMorpho == 1 )
              morpho *= (*dist)( x, y, z );
            else if ( typeMorpho == 2 )
              morpho *= ( 1.0 + 0.5 * (*dist)( x, y, z ) ); 

            //
            // All evolution
          
            (*newf)(x,y,z) = (*f)(x,y,z) + deltat * (diffusion + morpho);
         
            if ( fabs( (*newf)(x,y,z) ) > tubeRadius )
              (*flag)(x,y,z) = 0;
            if ( fabs( (*newf)(x,y,z) ) < tubeRadius )
              for (zz=-1;zz<2;zz++)
                for (yy=-1;yy<2;yy++)
                  for (xx=-1;xx<2;xx++)
                    (*flag)(x+xx,y+yy,z+zz) = 1;
          }
          // If point is not in the tube
          else
            (*newf)(x,y,z) = (*f)(x,y,z);

    tmp  = f;
    f    = newf;
    newf = tmp;

    ForEach3d( (*f0), x, y, z )
      (*f0)( x, y, z ) = (*f)( x, y, z );


    //
    // Reinitializes the distance map on the tube
    //
    t = 0;
    do
    { cout << "\b\b\b\b\b" << "  " << setw(3) << t+1 << flush;
      for (z=2;z<Z-2;z++)
        for (y=2;y<Y-2;y++)
          for (x=2;x<X-2;x++)
            // If point is in the tube
            if ((*flag)(x,y,z))
            { 
              //
              // Estimates the sethian/Osher gradient of phi
              // 2nd order ENO scheme
              //
              Dx_m1 = ( (*f)(x,y,z)   - (*f)(x-1,y,z) ) / DX;
              Dx_m2 = ( (*f)(x-1,y,z) - (*f)(x-2,y,z) ) / DX;
              Dx_p1 = ( (*f)(x+1,y,z) - (*f)(x,y,z)   ) / DX;
              Dx_p2 = ( (*f)(x+2,y,z) - (*f)(x+1,y,z) ) / DX;
          
              Dy_m1 = ( (*f)(x,y,z)   - (*f)(x,y-1,z) ) / DY;
              Dy_m2 = ( (*f)(x,y-1,z) - (*f)(x,y-2,z) ) / DY;
              Dy_p1 = ( (*f)(x,y+1,z) - (*f)(x,y,z)   ) / DY;
              Dy_p2 = ( (*f)(x,y+2,z) - (*f)(x,y+1,z) ) / DY;

              Dz_m1 = ( (*f)(x,y,z)   - (*f)(x,y,z-1) ) / DZ;
              Dz_m2 = ( (*f)(x,y,z-1) - (*f)(x,y,z-2) ) / DZ;
              Dz_p1 = ( (*f)(x,y,z+1) - (*f)(x,y,z)   ) / DZ;
              Dz_p2 = ( (*f)(x,y,z+2) - (*f)(x,y,z+1) ) / DZ;


              Dx_m1_x_m1 = ( Dx_m1 - Dx_m2 ) / DX;
              Dx_p1_x_p1 = ( Dx_p2 - Dx_p1 ) / DX;
              Dx_m1_x_p1 = ( Dx_p1 - Dx_m1 ) / DX;

              Dy_m1_y_m1 = ( Dy_m1 - Dy_m2 ) / DY;
              Dy_p1_y_p1 = ( Dy_p2 - Dy_p1 ) / DY;
              Dy_m1_y_p1 = ( Dy_p1 - Dy_m1 ) / DY;

              Dz_m1_z_m1 = ( Dz_m1 - Dz_m2 ) / DZ;
              Dz_p1_z_p1 = ( Dz_p2 - Dz_p1 ) / DZ;
              Dz_m1_z_p1 = ( Dz_p1 - Dz_m1 ) / DZ;


              A = Dx_m1 + 0.5 * MINABS( Dx_m1_x_m1 , Dx_m1_x_p1 );
              B = Dx_p1 + 0.5 * MINABS( Dx_p1_x_p1 , Dx_m1_x_p1 );
              C = Dy_m1 + 0.5 * MINABS( Dy_m1_y_m1 , Dy_m1_y_p1 );
              D = Dy_p1 + 0.5 * MINABS( Dy_p1_y_p1 , Dy_m1_y_p1 );
              E = Dz_m1 + 0.5 * MINABS( Dz_m1_z_m1 , Dz_m1_z_p1 );
              F = Dz_p1 + 0.5 * MINABS( Dz_p1_z_p1 , Dz_m1_z_p1 );


              if ( (*f0)( x, y, z ) > 0)
              {
                aa = (A>0 ? A : 0);
                bb = (B<0 ? B : 0);
                cc = (C>0 ? C : 0);
                dd = (D<0 ? D : 0);
                ee = (E>0 ? E : 0);
                ff = (F<0 ? F : 0);

                G = sqrt(aa*aa + bb*bb + cc*cc + dd*dd + ee*ee + ff*ff) - 1.0;
              }
              else if ( (*f0)( x, y, z ) < 0)
              { 
                aa = (A<0 ? A : 0);
                bb = (B>0 ? B : 0);
                cc = (C<0 ? C : 0);
                dd = (D>0 ? D : 0);
                ee = (E<0 ? E : 0);
                ff = (F>0 ? F : 0);

                G = sqrt(aa*aa + bb*bb + cc*cc + dd*dd + ee*ee + ff*ff) - 1.0;
              }
              else
                G = 0.0;

              S = (*f0)(x,y,z) / sqrt( sqr( (*f0)(x,y,z) ) + h2 );

              (*newf)(x,y,z) = (*f)(x,y,z) - Deltat * S * G;
            }
      error = 0.0;
      count = 0;
      for (z=2;z<Z-2;z++)
        for (y=2;y<Y-2;y++)
          for (x=2;x<X-2;x++)
            if ((*flag)(x,y,z))
            { count++;
              error += fabs((*newf)(x,y,z) - (*f)(x,y,z));
            }
      error /= float(count);
      tmp  = f;
      f    = newf;
      newf = tmp;
      t++;
    }
    while ( error >= criterion );


    //
    // Backup of the result all step iterations
    //
    if ((n+1)%step == 0 || (n+1==N))
    { string name(fileout);
      char suffix[16];
      sprintf(suffix,"_%d",n);
      AimsData<short> vol(X,Y,Z);
      vol.setSizeX( DX );
      vol.setSizeY( DY );
      vol.setSizeZ( DZ );
      for (z=0;z<Z;z++)
        for (y=0;y<Y;y++)
          for (x=0;x<X;x++)
            vol(x,y,z) = ((*f)(x,y,z)>0) ? 0 : 255;
      string path = string("time") + string(suffix);
      Directory	dir( path );
      dir.mkdir();

      Writer<AimsData<short> > writer(path+string("/")+name+string(suffix));
      writer << vol;
    }
  }  

  cout << endl;

  delete f0;
  if ( dist ) delete dist;
  delete f;
  delete newf;

  return EXIT_SUCCESS;
}
