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
#include <aims/getopt/getopt.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/math/math_g.h>
#include <cartobase/stream/directory.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

using namespace aims;
using namespace carto;
using namespace std;


BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsHJEvolution2d -i[nput]   <filein>                                     ",
  "                  -o[output] <fileout>                                    ",
  "                  -e[poch]   <epoch>                                      ",
  "                  [-t[ime]   <timestep>]                                  ",
  "                  [-f[0]     <F0>]                                        ",
  "                  -F[1]      <F1>                                         ",
  "                  [-s[tep]   <savingstep>]                                ",
  "                  [-u[radius] <tuberadius>]                               ",
  "                  [-m[orphology] <typeMorpho>]                            ",
  "                  [-h[elp]]                                               ",
  "--------------------------------------------------------------------------",
  "2D Hamilton-Jacobi evolution of a binary short image f(x,y)               ",
  "Fast tubular method                                                       ",
  "--------------------------------------------------------------------------",
  "  filein    : input SHORT binary data                                     ",
  "  fileout   : prefix name for output SHORT data                           ",
  "  epoch     : number of evolving iterations to proceed                    ",
  "  timestep  : time step in seconds [default=0.1]                          ",
  "  F0,F1     : speed function F = F0 + F1.K [default F0=-1.0 / F1=-1.0]    ",
  "              F0<0 : erosion  / F0>0 : dilation                           ",
  "  savingtep : number of time iterations between 2 backups [default=1]     ",
  "  uradius   : radius of the evolving tube [default=12]                    ",
  "  typeMorpho: type of morphologic evolution                               ",
  "                   0 -> -F0.||DPhi||                                      ",
  "                   1 -> -F0.||DPhi||.dist(x,y)                            ",
  "                   2 -> -F0.||DPhi||.(1.0 + 0.5 * dist(x,y))              ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


inline float MINABS(float x,float y)
{ if (x*y < 0) return 0.0;
  else
  { if (fabs(x) <= fabs(y)) return x;
    else                    return y;
  }
}


int main( int argc, char *argv[] )
{
  char *filein = NULL, *fileout = NULL, *filedist = NULL;
  int N = 0;
  float deltat = 0.1;
  float F0 = -1.0;
  float F1 = -1.0;
  int  step = 1;
  float tubeRadius = 12.0;
  int  x = 0, y = 0, n = 0, xx = 0, yy = 0;
  int  typeMorpho = 0;
  float h = 0.0, h2 = 0.0, criterion = 0.0, Deltat = 0.0, hh = 0.0;

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
  { 'm',"morphology",AIMS_OPT_INT   ,&typeMorpho,0                ,0},
  { 0  ,0           ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  ASSERT( F1 <= 0.0 );
  ASSERT( typeMorpho>=0 && typeMorpho<=2 );

  //
  // reads image and binarizes it
  //
  AimsData<short> *bin = new AimsData<short>;

  Reader<AimsData<short> > reader( filein );
  if( !reader.read( *bin, 1 ) )
    AimsUsage( usage );

  ASSERT( bin->dimZ() == 1 );

  bin->fillBorder( 0 );
  int  X = 0,Y = 0;
  X = bin->dimX();
  Y = bin->dimY();
  float DX = 0, DY = 0;
  DX = bin->sizeX();
  DY = bin->sizeX();

  h = ( DX + DY ) / 2.0;
  h2 = sqr( h );
  Deltat = h / 10.0;
  criterion = Deltat * h2;

  for (y=0;y<bin->dimY();y++)
    for (x=0;x<bin->dimX();x++)
      (*bin)(x,y) = ((*bin)(x,y)<1) ? 0 : 32767;


  //
  // read distance map
  //
  AimsData<float>* dist = NULL;
  if ( typeMorpho ==1 || typeMorpho == 2 )
  {
    ASSERT( filedist );
    dist = new AimsData<float>;
    Reader<AimsData<float> > dataR( filedist );
    dataR >> *dist;
    ASSERT( dist->dimX() == X &&
            dist->dimY() == Y &&
            dist->dimZ() == 1 &&
            dist->dimT() == 1   );
  }


  //
  // Gets the signed distance map
  //
  cout << endl << "processing distance map" << endl;
  AimsData<float> *f = new AimsData<float>;
  *f = AimsFloatSignedChamferDistanceMap( *bin );
  f->setSizeX( bin->sizeX() );
  f->setSizeY( bin->sizeY() );
  ForEach2d( (*f), x , y )
    (*f)( x, y ) = -(*f)( x, y );


  //
  // Deletes binary input image
  //
  delete bin;


  //
  // Prepares a temporary result image
  //
  AimsData<float> *newf = new AimsData<float>(X,Y);
  newf->setSizeX(f->sizeX());
  newf->setSizeY(f->sizeY());
  for (y=0;y<Y;y++)
    for (x=0;x<X;x++)
       if ( x<2    || y<2    ||
            x>=X-2 || y>=Y-2  )
            (*newf)(x,y) = (*f)(x,y);


  //
  //
  // Prepares a initial image for distance recovery
  //
  AimsData<float> *f0 = new AimsData<float>(X,Y);
  f0->setSizeX(f->sizeX());
  f0->setSizeY(f->sizeY());
  

  // Binary BYTE image to set points that are in the tube
  //
  int count = 0;
  AimsData<byte>  *flag = new AimsData<byte>(X,Y);
  for (y=0;y<Y;y++)
    for (x=0;x<X;x++)
      if (fabs((*f)(x,y))<tubeRadius)
      { (*flag)(x,y) = 1;
        count++;
      }
      else (*flag)(x,y) = 0;
  cout << endl << count << " points in the tube" << endl << flush;


  //
  // Evolution loop
  //
  AimsData<float> *tmp;
  float dx=0,dy=0,dxx=0,dyy=0,dxy=0,curvature=0,diffusion=0;
  float Dx_m1=0,Dx_m2=0,Dx_p1=0,Dx_p2=0,
        Dy_m1=0,Dy_m2=0,Dy_p1=0,Dy_p2=0;
  float Dx_m1_x_m1=0,Dx_p1_x_p1=0,Dx_m1_x_p1=0,
        Dy_m1_y_m1=0,Dy_p1_y_p1=0,Dy_m1_y_p1=0;
  float aa=0,bb=0,cc=0,dd=0;
  float A=0,B=0,C=0,D=0;
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
    for (y=2;y<Y-2;y++)
      for (x=2;x<X-2;x++)
        // If point is in the tube
        if ((*flag)(x,y)) 
        {
          //
          // Diffusion term
          //
          if ((*f)(x-1,y  ) != (*f)(x+1,y  ) || 
              (*f)(x  ,y-1) != (*f)(x  ,y+1)   )
          {
            dx    = ( (*f)(x+1,y  ) - (*f)(x-1,y  ) ) / ( 2.0 * DX );
            dy    = ( (*f)(x  ,y+1) - (*f)(x  ,y-1) ) / ( 2.0 * DY );

            dxx   = ( (*f)(x+2,y) - 2.0 * (*f)(x,y) + 
                      (*f)(x-2,y) ) / ( 4.0 * DX * DX );
            dyy   = ( (*f)(x,y+2) - 2.0 * (*f)(x,y) + 
                      (*f)(x,y-2) ) / ( 4.0 * DY * DY );
            dxy   = ( (*f)(x+1,y+1) - (*f)(x-1,y+1) - 
                      (*f)(x+1,y-1) + (*f)(x-1,y-1) ) /
                    ( 4.0 * DX * DY );

            hh = sqrt( dx*dx + dy*dy );
            curvature = ( dxx*dy*dy - 2*dx*dy*dxy + dyy*dx*dx ) / cub( hh ); 
          }
          else curvature = 0.0;

          diffusion = - F1 * curvature * sqrt( dx*dx + dy*dy );


          //
          // Morphological term
          //
          Dx_m1 = ( (*f)(x  ,y) - (*f)(x-1,y) ) / DX;
          Dx_m2 = ( (*f)(x-1,y) - (*f)(x-2,y) ) / DX;
          Dx_p1 = ( (*f)(x+1,y) - (*f)(x  ,y) ) / DX;
          Dx_p2 = ( (*f)(x+2,y) - (*f)(x+1,y) ) / DX;
          
          Dy_m1 = ( (*f)(x,y  ) - (*f)(x,y-1) ) / DY;
          Dy_m2 = ( (*f)(x,y-1) - (*f)(x,y-2) ) / DY;
          Dy_p1 = ( (*f)(x,y+1) - (*f)(x,y  ) ) / DY;
          Dy_p2 = ( (*f)(x,y+2) - (*f)(x,y+1) ) / DY;

          Dx_m1_x_m1 = ( Dx_m1 - Dx_m2 ) / DX;
          Dx_p1_x_p1 = ( Dx_p2 - Dx_p1 ) / DX;
          Dx_m1_x_p1 = ( Dx_p1 - Dx_m1 ) / DX;

          Dy_m1_y_m1 = ( Dy_m1 - Dy_m2 ) / DY;
          Dy_p1_y_p1 = ( Dy_p2 - Dy_p1 ) / DY;
          Dy_m1_y_p1 = ( Dy_p1 - Dy_m1 ) / DY;

          A = Dx_m1 + 0.5 * MINABS( Dx_m1_x_m1 , Dx_m1_x_p1 );
          B = Dx_p1 + 0.5 * MINABS( Dx_p1_x_p1 , Dx_m1_x_p1 );
          C = Dy_m1 + 0.5 * MINABS( Dy_m1_y_m1 , Dy_m1_y_p1 );
          D = Dy_p1 + 0.5 * MINABS( Dy_p1_y_p1 , Dy_m1_y_p1 );


          if (F0 > 0)
          {
            aa = (A>0 ? A : 0);
            bb = (B<0 ? B : 0);
            cc = (C>0 ? C : 0);
            dd = (D<0 ? D : 0);

            delta_plus = sqrt( aa*aa + bb*bb + cc*cc + dd*dd );
            morpho = -F0 * delta_plus;
          }
          else
          { 
            aa = (A<0 ? A : 0);
            bb = (B>0 ? B : 0);
            cc = (C<0 ? C : 0);
            dd = (D>0 ? D : 0);

            delta_minus = sqrt( aa*aa + bb*bb + cc*cc + dd*dd );
            morpho = -F0 * delta_minus;
          }
          if ( typeMorpho == 1 )
            morpho *= (*dist)( x, y );
          else if ( typeMorpho == 2 )
            morpho *= ( 1.0 + 0.5 * (*dist)( x, y ) ); 

          //
          // All evolution
          //
          (*newf)(x,y) = (*f)(x,y) + deltat * (diffusion + morpho);
         
          if ( fabs( (*newf)(x,y) ) > tubeRadius )
            (*flag)(x,y) = 0;
          if ( fabs( (*newf)(x,y) ) < tubeRadius )
            for (yy=-1;yy<2;yy++)
              for (xx=-1;xx<2;xx++)
                (*flag)(x+xx,y+yy) = 1;
        }
        // If point is not in the tube
        else
          (*newf)(x,y) = (*f)(x,y);

    tmp  = f;
    f    = newf;
    newf = tmp;

    ForEach2d( (*f0), x, y )
      (*f0)( x, y ) = (*f)( x, y );

    //
    // Reinitializes the distance map on the tube
    //
    t = 0;
    do
    { cout << "\b\b\b\b\b" << "  " << setw(3) << t+1 << flush;
      for (y=2;y<Y-2;y++)
        for (x=2;x<X-2;x++)
          // If point is in the tube
          if ((*flag)(x,y))
          { 
            //
            // Estimates the sethian/Osher gradient of phi
            // 2nd order ENO scheme
            //
            Dx_m1 = ( (*f)(x,y)   - (*f)(x-1,y) ) / DX;
            Dx_m2 = ( (*f)(x-1,y) - (*f)(x-2,y) ) / DX;
            Dx_p1 = ( (*f)(x+1,y) - (*f)(x,y)   ) / DX;
            Dx_p2 = ( (*f)(x+2,y) - (*f)(x+1,y) ) / DX;
          
            Dy_m1 = ( (*f)(x,y)   - (*f)(x,y-1) ) / DY;
            Dy_m2 = ( (*f)(x,y-1) - (*f)(x,y-2) ) / DY;
            Dy_p1 = ( (*f)(x,y+1) - (*f)(x,y)   ) / DY;
            Dy_p2 = ( (*f)(x,y+2) - (*f)(x,y+1) ) / DY;

            Dx_m1_x_m1 = ( Dx_m1 - Dx_m2 ) / DX;
            Dx_p1_x_p1 = ( Dx_p2 - Dx_p1 ) / DX;
            Dx_m1_x_p1 = ( Dx_p1 - Dx_m1 ) / DX;

            Dy_m1_y_m1 = ( Dy_m1 - Dy_m2 ) / DY;
            Dy_p1_y_p1 = ( Dy_p2 - Dy_p1 ) / DY;
            Dy_m1_y_p1 = ( Dy_p1 - Dy_m1 ) / DY;

            A = Dx_m1 + 0.5 * MINABS( Dx_m1_x_m1 , Dx_m1_x_p1 );
            B = Dx_p1 + 0.5 * MINABS( Dx_p1_x_p1 , Dx_m1_x_p1 );
            C = Dy_m1 + 0.5 * MINABS( Dy_m1_y_m1 , Dy_m1_y_p1 );
            D = Dy_p1 + 0.5 * MINABS( Dy_p1_y_p1 , Dy_m1_y_p1 );

            if ( (*f0)( x, y ) > 0)
            {
              aa = (A>0 ? A : 0);
              bb = (B<0 ? B : 0);
              cc = (C>0 ? C : 0);
              dd = (D<0 ? D : 0);

              G = sqrt(aa*aa + bb*bb + cc*cc + dd*dd ) - 1.0;
            }
            else if ( (*f0)( x, y ) < 0)
            { 
              aa = (A<0 ? A : 0);
              bb = (B>0 ? B : 0);
              cc = (C<0 ? C : 0);
              dd = (D>0 ? D : 0);

              G = sqrt(aa*aa + bb*bb + cc*cc + dd*dd) - 1.0;
            }
            else
              G = 0.0;

            S = (*f0)(x,y) / sqrt( sqr( (*f0)(x,y) ) + h2 );

            (*newf)(x,y) = (*f)(x,y) - Deltat * S * G;
          }
      error = 0.0;
      count = 0;
      for (y=2;y<Y-2;y++)
        for (x=2;x<X-2;x++)
          if ((*flag)(x,y))
          { count++;
            error += fabs((*newf)(x,y) - (*f)(x,y));
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
      AimsData<short> vol(X,Y);
      vol.setSizeX( DX );
      vol.setSizeY( DY );
      for (y=0;y<Y;y++)
        for (x=0;x<X;x++)
          vol(x,y) = ((*f)(x,y)>0) ? 0 : 255;
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
