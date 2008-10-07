/*
 *  Copyright (C) 1999-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 *  Mutual Information Rigid registration
 */

#include "miProbe.h"
#include "mutualfunc.h"
#include "corratiofunc.h"
#include <aims/getopt/getopt.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/io/motionW.h>
#include <aims/io/coarseR.h>
#include <aims/math/math_g.h>
#include <aims/io/datatypecode.h>
#include <aims/utility/utility_g.h>
#include <aims/information/pdf.h>
#include <aims/optimization/optimization_g.h>
#include <aims/pyramid/pyramid_g.h>
#include <aims/resampling/resampling_g.h>
#include <iomanip>
#include <time.h>

#include <stdio.h>
#include <cartobase/stream/fileutil.h>

using namespace carto;
using namespace aims;
using namespace std;

const float REF_RATIO = 0.05;
const float TEST_RATIO = 0.1;

#define DEBUG 1

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMIRegister  -r[eference] <ref>                                       ",
  "                -t[est] <testimage>                                      ",
  "                -o[utput] <fileout>                                      ",
  "                -l[ogger] <filelog>                                      ",
  "               [--serialnum  <sn>]                                       ",
  "               [--Tx   <Tx>]                                             ",
  "               [--Ty   <Ty>]                                             ",
  "               [--Tz   <Tz>]                                             ",
  "               [--dTx <dTx>]                                             ",
  "               [--dTy <dTy>]                                             ",
  "               [--dTz <dTz>]                                             ",
  "               [--Rx <Rx>]                                               ",
  "               [--Ry <Ry>]                                               ",
  "               [--Rz <Rz>]                                               ",
  "               [--dRx <dRx>]                                             ",
  "               [--dRy <dRy>]                                             ",
  "               [--dRz <dRz>]                                             ",
  "               [--dir <test_TO_ref: output transfomation filename>]      ",
  "               [--inv <ref_TO_test: output transfomation filename>]      ",
  "               [--interpolation <method>]                                ",
  "               [--optimization <algo>]                                   ",
  "               [--pdfcalc <mode>]                                        ",
  "               [--gcinit <mode>]                                         ",
  "               [--index <index>]                                         ",
  "               [--refstartpyr  <rslevel>]                                ",
  "               [--refendpyr    <relevel>]                                ",
  "               [--teststartpyr <tslevel>]                                ",
  "               [--testendpyr   <televel>]                                ",
  "               [--pays_size <pays_size>]                                 ",
  "               [--graylevel <nl>]                                        ",
  "               [--masksize <ms>]                                         ",
  "               [--error <epsilon>]                                       ",
  "               [--seuilref  <rslevel>]                                   ",
  "               [--seuiltest  <rslevel>]                                  ",
  "               [--seuiltest  <rslevel>]                                  ",
  "               [--verbose  <verboselevel: 0 is low>]                     ",
  "               [-h[elp]]                                                 ",
  "-------------------------------------------------------------------------",
  "Registration according to Mutual Information Method                       ",
  "-------------------------------------------------------------------------",
  "     ref        : source S16 reference data                              ",
  "     test       : source S16 test data (to register)                     ",
  "     fileout    : destination S16 data                                   ",
  "     sn         : number of the frame to consider for ref [default 0]    ",
  "                :      relevant for dynamic series ref images            ",
  "     Tx, Ty, Tz : Init Translaction coord   [default=0.0,0.0]            ",
  "     Rx, Ry, Rz : Init Rotation coord       [default=0.0,0.0]            ",
  "     dWw        : Increment for variable Ww [default=heuristics in code] ",
  "     method     : interpolation  nearest/linear/spline [default=linear]  ",
  "     algo       : optimaizer  powell/random/single [default=powell]      ",
  "                       single --> get a one shot evaluation              ",
  "                       random --> sample cost value in a given area      ",
  "                       powell --> cost minimization with powell algorithm",
  "     index      : index to be optimized mi/cr     [default=mi]           ",
  "                       Mutual Information --> mi                         ",
  "                       Correlation Ratio  --> cr                         ",
  "     mode       : Pdf calculus mode : direct ou pv (def)                 ",
  "     level      : level of the multiresolution optimization              ",
  "                :    rslevel start resolution level for ref              ",
  "                :    relevel end resolution level for ref                ",
  "                :    tslevel start resolution level for tes              ",
  "                :    televel end resolution level for tes                ",
  "     pays_size  : Half size the volume generated if algo=other activated ",
  "     nl         : numb of classes in histogram pdf estimation [def.=64]  ",
  "     ms         : size of the gaussian convolution mask in pdf estimation",
  "                         for Mutual Information         [default=5]      ",
  "     seuilref   : relative thresh applied prior to grav cent estimation  ",
  "     seuiltest  :                                                        ",
  "                                                                         ",
  "     epsilon    : tolerance on results [default=0.01]                 ",
  "     gcinit     : Init with GravCenter : \"yes\" (def.) or \"no\"        ",
  "-------------------------------------------------------------------------",
END_USAGE


static void Usage()
{
  AimsUsage(usage);
}


// CM' = R.CM + T
// OM' = (Id - R).OC + T + R.OM
//        terme NEWTRANS


void WriteCurrentDepInFile(
		     AimsVector<float,6>& p,
		     ObjFunc             *objfunc,
		     string                direct,
		     string                inverse,
		     int                  notFirstCall)
{
  // Infer deplacement from p and gravity centers stored in objfunc
  Motion depl    = objfunc->getDepl( p );
  Motion invdepl = depl.inverse();
  
  cout << "Direct FileName " << direct << endl;
  cout << "Inverse FileName " << inverse << endl;

  // Write ref_TO_test transform matrix
  MotionWriter motionWDir(direct,
			  (notFirstCall? ios::app : ios::out) );
  MotionWriter motionWRev(inverse,
			  (notFirstCall? ios::app : ios::out) );
  
  motionWDir << depl;
  motionWRev << invdepl;
  
  
  // Write current parameters for plotting

  // Pour enregistrer les fichiers dans le repertoire de "direct" en commencant leur nom par "inputtest_TO_inputref_":
  string courbe_path = FileUtil::dirname(direct) + FileUtil::separator() + "courbe" ;

  ofstream courbe(courbe_path.c_str(),
		  (notFirstCall ? ios::app : ios::out) );
  if (!notFirstCall)
    {
      courbe << "  Tx  " << "\t" << "  Ty  " << "\t" << "  Tz  " 
	     << "  Rx  " << "\t" << "  Ry  " << "\t" << "  Rz  " << endl;
    }
  courbe << depl.translation().item(0) << "\t" 
	 << depl.translation().item(1) << "\t"
	 << depl.translation().item(2) << "\t"
	 << p[4] << "\t" << p[5] << "\t" << p[2] <<endl;
  courbe.close();
}

void WriteCurrentDep(
		     AimsVector<float,6>& p,
		     ObjFunc             *objfunc,
		     char                *ftest,
		     char                *fref,
		     int                  notFirstCall)
{
  // Build canonical name
  string from = string(ftest), to = string( fref );
  if ( from.rfind('/') != string::npos ) 
    from = from.substr( from.rfind('/')+1, from.size() );
  if ( from.rfind('.') != string::npos ) 
    from = from.substr( 0, from.rfind('.') );
  if ( to.rfind('/') != string::npos ) 
    to = to.substr( to.rfind('/')+1, to.size() );
  if ( to.rfind('.') != string::npos ) 
    to = to.substr( 0, to.rfind('.') );

  // Pour enregistrer les fichiers dans le repertoire de "ftest" en commencant leur nom par "inputtest_TO_inputref_":
  string transfo_path = FileUtil::dirname(string (ftest)) + FileUtil::separator() ;

  string direct = transfo_path + from + "_TO_" + to;
  string inverse= transfo_path + to + "_TO_" + from;

  WriteCurrentDepInFile(p, objfunc, direct, inverse, notFirstCall);
}

int main( int argc, char* argv[] )


{
  //
  // Default values
  //
  char* c_interpolator      = "linear";
  char* c_optimizer         = "powell";
  char* c_pdf               = "pv";
  char* c_gc                = "yes";
  char* c_objfunc           = "mi";
  int   numLevel            = 64;
  int   maskSize            = 5;
  float error               = 0.01;
  AimsVector<float,6>  param( 0.0 );
  AimsVector<float,6> deltaP( 0.0 );
  char             *fileRef = NULL, *fileTest = NULL,
                   *directTrans = NULL, *inverseTrans = NULL,
                   *fileOut = NULL, *fileLog = NULL;
  int  verbosity            = 0;
  int  rspLevel             = 0;
  int  repLevel             = 0;
  int  tspLevel             = 0;
  int  tepLevel             = 0;
  int  sn                   = 0;
  int  energySize           = 0;
  float testratio           = TEST_RATIO;
  float refratio            = REF_RATIO;
  Pyramid<short>      *Pref = NULL,  *Ptest=NULL;
  PyramidFunc<short> *pyramidfunc = NULL;
  ofstream           mycout;


  // Collecte des arguments.
  int rsp = 0, rep = 0, tsp = 0, tep = 0;
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { 'r',"reference"    ,AIMS_OPT_STRING,&fileRef   ,0                ,1},
  { 't',"test"         ,AIMS_OPT_STRING,&fileTest  ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileOut   ,0                ,0},
  { 'l',"logger"       ,AIMS_OPT_STRING,&fileLog   ,0                ,0},
  { ' ',"Tx"           ,AIMS_OPT_FLOAT ,&param[0]  ,0                ,0},
  { ' ',"Ty"           ,AIMS_OPT_FLOAT ,&param[1]  ,0                ,0},
  { ' ',"Tz"           ,AIMS_OPT_FLOAT ,&param[3]  ,0                ,0},
  { ' ',"dTx"          ,AIMS_OPT_FLOAT ,&deltaP[0] ,0                ,0},
  { ' ',"dTy"          ,AIMS_OPT_FLOAT ,&deltaP[1] ,0                ,0},
  { ' ',"dTz"          ,AIMS_OPT_FLOAT ,&deltaP[3] ,0                ,0},
  { ' ',"Rx"           ,AIMS_OPT_FLOAT ,&param[4]  ,0                ,0},
  { ' ',"Ry"           ,AIMS_OPT_FLOAT ,&param[5]  ,0                ,0},
  { ' ',"Rz"           ,AIMS_OPT_FLOAT ,&param[2]  ,0                ,0},
  { ' ',"dRx"          ,AIMS_OPT_FLOAT ,&deltaP[4] ,0                ,0},
  { ' ',"dRy"          ,AIMS_OPT_FLOAT ,&deltaP[5] ,0                ,0},
  { ' ',"dRz"          ,AIMS_OPT_FLOAT ,&deltaP[2] ,0                ,0},
  { ' ',"dirTransOut", AIMS_OPT_STRING,&directTrans,0                ,0},
  { ' ',"invTransOut", AIMS_OPT_STRING,&inverseTrans,0               ,0},
  { ' ',"interpolation",AIMS_OPT_STRING,&c_interpolator,0            ,0},
  { ' ',"optimization" ,AIMS_OPT_STRING,&c_optimizer,0               ,0},
  { ' ',"pdfcalc" ,     AIMS_OPT_STRING,&c_pdf,     0                ,0},
  { ' ',"gcinit" ,      AIMS_OPT_STRING,&c_gc,      0                ,0},
  { ' ',"index"        ,AIMS_OPT_STRING,&c_objfunc ,0                ,0},
  { ' ',"refstartpyr"  ,AIMS_OPT_INT   ,&rsp       ,0                ,0},
  { ' ',"refendpyr"    ,AIMS_OPT_INT   ,&rep       ,0                ,0},
  { ' ',"teststartpyr" ,AIMS_OPT_INT   ,&tsp       ,0                ,0},
  { ' ',"testendpyr"   ,AIMS_OPT_INT   ,&tep       ,0                ,0},
  { ' ',"serialnum"    ,AIMS_OPT_INT   ,&sn        ,0                ,0},
  { ' ',"pays_size"    ,AIMS_OPT_INT   ,&energySize,0                ,0},
  { ' ',"graylevel"    ,AIMS_OPT_INT   ,&numLevel  ,0                ,0},
  { ' ',"masksize"     ,AIMS_OPT_INT   ,&maskSize  ,0                ,0},
  { ' ',"error"        ,AIMS_OPT_FLOAT ,&error     ,0                ,0},
  { ' ',"seuiltest"    ,AIMS_OPT_FLOAT ,&testratio ,0                ,0},
  { ' ',"seuilref"     ,AIMS_OPT_FLOAT ,&refratio  ,0                ,0},
  { ' ',"verbosity"    ,AIMS_OPT_INT   ,&verbosity ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);




  // 
  // Log file management
  //
  if ( fileLog )
    {
      mycout.open( fileLog );
    }

  

  //
  // choice of interpolation
  //
  Resampler<short>* interpolator = NULL;
  Sampler<short>*           comb = NULL;

  ASSERT( string( c_pdf ) == "direct" || string( c_pdf ) == "pv" );
  if ( string(c_pdf) == "direct")
    {
      if ( c_interpolator )
	{
	  ASSERT( string( c_interpolator ) == "nearest" ||
		  string( c_interpolator ) == "linear"  ||
		  string( c_interpolator ) == "spline"    );
	  if ( string( c_interpolator ) == "nearest" )
	    {
	      //       interpolator = new NNInterpolator;
	      cout << "interpolator : " << string( c_interpolator ) << endl;
	      cout << "Not implemented yet" << endl;
	    }
	  else if ( string( c_interpolator ) == "linear" )
	    {
	      interpolator = new MaskLinearResampler<short>;
	      cout << "interpolator    : " << string( c_interpolator ) << endl;
	    }
	  else if ( string( c_interpolator ) == "spline" )
	    {
	      //       interpolator = new SplineInterpolator;
	      cout << "interpolator     : " << string( c_interpolator ) << endl;
	      cout << "Not implemented yet" << endl;
	    }
	}
    }
  else
    {
      comb = new Sampler<short>;
    }



  //
  // choice of index to be minimized
  //
  ObjFunc* objfunc = NULL;
  if ( c_objfunc )
  {
    ASSERT( string( c_objfunc ) == "mi" ||
            string( c_objfunc ) == "cr"   );
    if ( string( c_objfunc ) == "mi" )
    {
      objfunc = new MutualInfoFunc( numLevel, interpolator, comb, maskSize );
      cout << "objective function   : mutual information" << endl;
    }
    else if ( string( c_objfunc ) == "cr" )
    {
      objfunc = new CorRatioFunc( numLevel, interpolator, comb, maskSize );
      cout << "objective function   : correlation ratio" << endl;
    }
  }




  //
  // choice of optimizer
  //
  Optimizer<float,6>* optimizer = NULL;
  if ( c_optimizer )
  {
    ASSERT( string( c_optimizer ) == "powell" ||
            string( c_optimizer ) == "random" ||
            string( c_optimizer ) == "single"    );
    if ( string( c_optimizer ) == "powell" )
    {
      optimizer = new PowellOptimizer<float,6>( *objfunc, error );
      cout << "optimizer            : powell" << endl;
    }
    else if ( string( c_optimizer ) == "random" )
    {
      optimizer = new DetermOptimizer<float,6>( *objfunc, error, 10000, 100, 
						true );
      cout << "optimizer            : deterministic" << endl;
    }
    else if ( string( c_optimizer ) == "single" )
    {
      optimizer = new SingleOptimizer<float,6>( *objfunc );
      cout << "optimizer            : singleoptimizer" << endl;
    }
  }



  //
  // Equiped with probe
  //
//   OptimizerProbe<float, 6> * probe = 
//     new MIRegistrationProbe<float, 6>( numLevel, 200, verbosity ) ;
//   optimizer->setProbe( probe ) ;





  //
  //Read data
  //   - read ref data completely: assumed to be one frame
  //   - get info from header to manage test data on a frame by frame basis.
  //-------- Data are read as short int (float data are scaled !!!!)
  Finder testFinder;
  AimsData<short>    test;
  CoarseReader*    testR = new CoarseReader( fileTest ); 
  AimsData<short>    ref;
  CoarseReader*    refR = new CoarseReader( fileRef );

  refR->read( ref, 0, 0, 0 ); //frameRead( ref, 0, 0L );


  if( ! testFinder.check( fileTest ) )
    {
    cerr << "could not load " << fileTest << endl;
    return EXIT_FAILURE;
    }
  const Header	*h = testFinder.header();
  const PythonHeader *ph = dynamic_cast<const PythonHeader *>( h );
  if( !ph )
    {
      cerr << "Can't get info for "<< fileTest << endl;
      return EXIT_FAILURE;
    }
  vector< int > v;
  int sizeT;
  if  ( ph->getProperty("volume_dimension", v ) )
    sizeT = v[3];





  //
  //Get gravity centers if the ref image
  //
  float KeepGC_x=0.0, KeepGC_y=0.0, KeepGC_z=0.0;

  {
    float gx, gy, gz;
    int gn;
    int x, y, z;
    AimsThreshold<short,short> thresh( AIMS_GREATER_THAN,
				       (short)( refratio * ref.maximum() ), 0 );
    AimsData<short> *tmp = new AimsData<short>;

    *tmp = thresh.bin( ref );
    gx = gy = gz = 0.0; gn = 0;
    ForEach3d((*tmp), x, y, z)
      {
	if ( (*tmp)( x, y, z ))
	  {
	    gx += x; gy += y; gz += z; gn++;
	  }
      }
    gx = (gx/gn) * ref.sizeX();
    gy = (gy/gn) * ref.sizeY();
    gz = (gz/gn) * ref.sizeZ();
    objfunc->setGcRef(gx, gy, gz);
#ifdef DEBUG
cout << "DEBUG:MAIN>>"  << "refgx " << gx << "refgy " << gy << "refgz " << gz << endl;
#endif
    delete tmp;
   if (string( c_gc ) != "yes")
   {
       KeepGC_x = gx; KeepGC_y = gy; KeepGC_z = gz;
   }
  }



  //
  // Infer pyramid range from the following heuristics
  //
  rspLevel = (rsp ? rsp : rspLevel);
  repLevel = (rep ? rep : rspLevel);
  ASSERT( (repLevel == rspLevel) ); // Pour la ref il n'y a qu'un niveau d'ech
  tspLevel = (tsp ? tsp : tspLevel);
  tepLevel = (tep ? tep : tspLevel);
  ASSERT( (tepLevel <= tspLevel) );


  //
  // State of input data.
  //
  cout << "Reference Image       : " << fileRef << endl;
  cout << "         Pyramid range: " << rspLevel << " - " << repLevel << endl;
  if ( sn > 0)
    {
      cout << "Frame seled from serie: " << sn << endl;
    }
  cout << "Test      Image       : " << fileTest << endl;
  cout << "         Pyramid range: " << tspLevel << " - " << tepLevel << endl;
  cout << "Number of Level       : " << numLevel << endl;
  cout << "Parzen Mask Size      : " << maskSize << endl;
  cout << "Error (Stop Criterion): " <<  error << endl;
  cout << "Ref Threshold  ratio  : " <<  refratio << endl;
  cout << "Test Threshold ratio  : " <<  testratio << endl;
  cout << "initial parameter     : " << param << endl;
  cout << "parameter variations  : " << deltaP << endl;



  //  
  // Start the job.
  //     1. Init ObectiveFunction Ref Images.
  //     2. Loop on time dimension (with init Test image).
  //

  

   pyramidfunc = new MedianPyramidFunc<short>();
  //   pyramidfunc = new MeanPyramidFunc<short>();
  Pref = new Pyramid<short>(*pyramidfunc);
  Pref->setRef( ref );
  Pref->setLevel( rspLevel );

  objfunc->setRef( Pref->item( rspLevel ) );
  

  AimsVector<float,6> p;
  for (int i=0; i<sizeT; i++) //- - - - - - - - - - - - -  - - - Loop on frames
    {
                                                       // Direct Frame reading
                             // ----------------------------------------------
      string f = testFinder.format();
      testR->read( test, 0, &f , i );

                                   // Find and Set Grav Center of Test Volume
                             // ----------------------------------------------
      if (string(c_gc) != "yes")
	{
	  objfunc->setGcTest(KeepGC_x, KeepGC_y, KeepGC_z);
	}
      else
	{
	  float gx, gy, gz;
	  int gn;
	  int x, y, z;
	  
	  AimsThreshold<short,short> thresh( AIMS_GREATER_THAN, 
					     (short)( testratio *  test.maximum() ), 0 );
	  AimsData<short> *tmp = new AimsData<short>;
	  
	  
	  *tmp = thresh.bin( test );
	  gx = gy = gz = 0.0; gn = 0;
	  ForEach3d((*tmp), x, y, z)
	    {
	      if ( (*tmp)( x, y, z ))
		{
		  gx += x; gy += y; gz += z; gn++;
		}
	    }
	  gx = (gx/gn) * test.sizeX();
	  gy = (gy/gn) * test.sizeY();
	  gz = (gz/gn) * test.sizeZ();
	  
	  objfunc->setGcTest(gx, gy, gz);
#ifdef DEBUG
	  cout << "DEBUG:MAIN>>" << "testgx " << gx << "testgy " << gy << "testgz " << gz << endl;
#endif
	  delete tmp;
	}
      
      
      // Infer deltas from the following heuristics
      // ----------------------------------------------
      if ( deltaP == float(0.0) )
	{
	  deltaP[0] = test.sizeX() / 2.0;
	  deltaP[1] = test.sizeY() / 2.0;
	  deltaP[3] = test.sizeZ() / 2.0;
	  deltaP[2] = 180.0/ 3.14159 / test.dimX();
	  deltaP[4] = 180.0/ 3.14159 / test.dimZ();
	  deltaP[5] = 180.0/ 3.14159 / test.dimZ();   //<-----  ????!!!!!
	}
      
      // Coarse to fine
      // ----------------------------------------------
      if ( Ptest ) delete ( Ptest );
      Ptest = new Pyramid<short>(*pyramidfunc);
      Ptest->setRef( test );
      Ptest->setLevel( tspLevel );
      deltaP *= float(pow( (double)2, (double)(tspLevel)));
      int level;
      for (level=tspLevel; level >=tepLevel; level--)
	{
	  objfunc->setTest( Ptest->item( level ) );
	  cout << "Processing Plevel " << level << param << deltaP << endl;

	  clock_t start = clock();  
	  p = optimizer->doit( param, deltaP );
	  clock_t finish = clock();
	  cout << finish - start << " ticks" << endl;
	  
	  cout << "Niveau : " << level << " Popt " << p << endl;

	  deltaP /=2.0;
	  param = p;
	}
                                                               // dump results
                           // ----------------------------------------------

      cout << "Volume " << i << "Popt " << p << endl;
      cout <<             " -Tx " << p[0];
      cout <<             " -Ty " << p[1];
      cout <<             " -Tz " << p[3];
      cout <<             " -Rx " << p[4];
      cout <<             " -Ry " << p[5];
      cout <<             " -Rz " << p[2] << endl;
      if (directTrans && inverseTrans)
	WriteCurrentDepInFile(p, objfunc, directTrans, inverseTrans, i);
      else
	WriteCurrentDep(p, objfunc, fileTest, fileRef, i);
    }                            // - - - - - - - - - - - - -End loop on frame



  if ( fileLog )
    {
      mycout.close( );
    }
  
  return EXIT_SUCCESS; 
}

