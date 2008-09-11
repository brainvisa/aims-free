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


#include "mutualfuncbucket.h" 
#include "corratiofuncbucket.h"
#include <aims/bucket/bucketMap.h>
#include <aims/getopt/getopt.h>
#include <aims/io/io_g.h>
#include <aims/io/coarseR.h>
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/information/information_g.h>
#include <aims/optimization/optimization_g.h>
#include <aims/resampling/resampling_g.h>
#include <iomanip>
#include <time.h>

using namespace std;
using namespace aims;

const float REF_RATIO = 0.05;
const float TEST_RATIO = 0.1;

#define DEBUG 1

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMIBucketRegister  -r[eference] <ref (bucket_SHORT file)>                   ",
  "                -t[est] <test >                                          ",
  "                -o[utput] <fileout>                                      ",
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
  "               [--interpolation <method>]                                ",
  "               [--optimization <algo>]                                   ",
  "               [--pdfcalc <mode>]                                        ",
  "               [--gcinit <mode>]                                         ",
  "               [--index <index>]                                         ",
  "               [--pays_size <pays_size>]                                 ",
  "               [--graylevel <nl>]                                        ",
  "               [--masksize <ms>]                                         ",
  "               [--error <epsilon>]                                       ",
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
  "     mode       : Pdf calculus mode : direct(def) ou pv                  ",
  "     pays_size  : Half size the volume generated if algo=other activated ",
  "     nl         : numb of classes in histogram pdf estimation [def.=64]  ",
  "     ms         : size of the gaussian convolution mask in pdf estimation",
  "                         for Mutual Information         [default=5]      ",
  "     seuilref   : relative thresh applied prior to grav cent estimation  ",
  "     seuiltest  :                                                        ",
  "                                                                         ",
  "     epsilon    : tolerance on results [default=0.00001]                 ",
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
void WriteCurrentDep(
		     AimsVector<float,6>& p,
		     ObjFuncBucket             *objfunc,
		     char                *ftest,
		     char                *fref,
		     int                  notFirstCall)
{
   Motion depl    = objfunc->getDepl( p );
   Motion invdepl = depl.inverse();

   // Write ref_TO_test transform matrix
   ofstream ddeplacements((string(ftest)+"_TO_"+string(fref)).c_str(),
			  (notFirstCall ? ios::app : ios::out) );
   ddeplacements << depl.translation().item(0)  << " " 
		 << depl.translation().item(1)  << " " 
		 << depl.translation().item(2)  << endl;
   ddeplacements << depl.rotation()(0,0) << " " 
		 << depl.rotation()(0,1) << " "
		 << depl.rotation()(0,2) << endl;
   ddeplacements << depl.rotation()(1,0) << " " 
		 << depl.rotation()(1,1) << " "
		 << depl.rotation()(1,2) << endl;
   ddeplacements << depl.rotation()(2,0) << " " 
		 << depl.rotation()(2,1) << " "
		 << depl.rotation()(2,2) << endl;
   ddeplacements.close();

   ofstream ideplacements((string(fref)+"_TO_"+string(ftest)).c_str(),
			  (notFirstCall ? ios::app : ios::out) );
   ideplacements << invdepl.translation().item(0)  << " " 
		 << invdepl.translation().item(1)  << " " 
		 << invdepl.translation().item(2)  << endl;
   ideplacements << invdepl.rotation()(0,0) << " " 
		 << invdepl.rotation()(0,1) << " "
		 << invdepl.rotation()(0,2) << endl;
   ideplacements << invdepl.rotation()(1,0) << " " 
		 << invdepl.rotation()(1,1) << " "
		 << invdepl.rotation()(1,2) << endl;
   ideplacements << invdepl.rotation()(2,0) << " " 
		 << invdepl.rotation()(2,1) << " "
		 << invdepl.rotation()(2,2) << endl;
   ideplacements.close();

   ofstream courbe("courbe",
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


template <class T, int D>
class MIRegistrationProbe : public OptimizerProbe<T, D> {
public:
  MIRegistrationProbe( int numLevel, int maxIteration = 20 ) ;
  virtual ~MIRegistrationProbe() ;

  virtual MIRegistrationProbe * clone() ;
  virtual void iteration( const AimsVector<T,D> & parameters, 
			  const AttributedObject& specific,
			  const float * costFunction = 0) ;
  virtual void test( const AimsVector<T,D> & parameters, 
		     const AttributedObject& specific,
		     const float * costFunction = 0) ;
  virtual void end() ;

private:
  AimsData<float>* _pdfOverIterations ;
  int _iteration ;
} ;


template <class T, int D>
MIRegistrationProbe<T,D>::MIRegistrationProbe( int numLevel, int maxIteration = 20 ) :
  _iteration(0)
{
  _pdfOverIterations = new AimsData<float>( numLevel, numLevel, maxIteration ) ;
  _pdfOverIterations->setSizeXYZT(1., 1., 1., 1. ) ;
  
//   cout << "CONSTRUCT\txsize= " << _pdfOverIterations->dimX() 
//        << "\tysize= " << _pdfOverIterations->dimY()  << endl ;

}

template <class T, int D>
MIRegistrationProbe<T,D>::~MIRegistrationProbe( )
{
  delete _pdfOverIterations ;
}

template <class T, int D>
MIRegistrationProbe<T,D> *
MIRegistrationProbe<T,D>::clone()
{
  MIRegistrationProbe<T,D> * probe = 
    new MIRegistrationProbe<T,D>( _pdfOverIterations->dimX(), 
				  _pdfOverIterations->dimZ() ) ;

  probe->_pdfOverIterations = this->_pdfOverIterations ;
  probe->_iteration = this->_iteration ;
  probe->_pdfOverIterations->setSizeXYZT(1., 1., 1., 1. ) ;

//   cout << "CLONE\txsize= " << probe->_pdfOverIterations->dimX() 
//        << "\tysize= " << probe->_pdfOverIterations->dimY()  << endl ;

  return probe ;
}

template <class T, int D>
void 
MIRegistrationProbe<T,D>::test( const AimsVector<T,D> & /*parameters*/, 
			   const AttributedObject& specific,
			   const float * /* costFunction = 0 */)

{
  string info ;
    
//   cout << "TestProbe\t" << ( specific.getProperty("info", info ) ? info : "" )
//        << endl ;
//   cout << "Current parameters = " <<  parameters 
//        << "\t value = " << ( costFunction != 0 ? 
// 			     *costFunction : 0 ) << endl ;

  
//   set<string> attributes( specific.getAttributes() ) ;
  
//   set<string>::const_iterator iter( attributes.begin() ), 
//     last( attributes.end() ) ;

//   cout << endl << endl ;
  
//   while( iter != last ){
//     cout << "\t" << *iter ;
//     ++iter ;
//   }
//   cout << endl << endl ;
  
  AimsVector<float,3> gcRef, gcTest ;
  
  if( ! specific.hasProperty("gc_ref") )
    {
//       cout << "No gravity center for reference image" << endl ;
    }
  else{
    specific.getProperty("gc_ref", gcRef ) ;
//     cout << "Reference image gravity center : " << gcRef << endl ;
  }

  if( ! specific.hasProperty("gc_test") )
    {
//       cout << "No gravity center for test image" << endl ;
    }
  else{
    specific.getProperty("gc_test", gcTest ) ;
//     cout << "Test image gravity center : " << gcTest << endl ;
  }
}

template <class T, int D>
void 
MIRegistrationProbe<T,D>::iteration( const AimsVector<T,D> & /*parameters*/, 
				     const AttributedObject& specific,
				     const float* /*costFunction = 0*/ )

{
//   cout << "IterationProbe" << endl ;
//   cout << "Current parameters = " <<  parameters 
//        << "\t value = " << ( costFunction != 0 ? 
// 			     *costFunction : 0 ) << endl ;

  AimsData<float> joint ;
  if( ! specific.hasProperty("joint_histogram" ) )
    cout << "No joint histogram" << endl ;
  else
    {
      specific.getProperty("joint_histogram", joint ) ;
      
      
      //     cout << "Iteration : " << _iteration << endl;
      //     cout << "FINAL\txsize= " << _pdfOverIterations->dimX() 
      // 	 << "\tysize= " << _pdfOverIterations->dimY()  
      // 	 << "\tzsize= " << _pdfOverIterations->dimZ() << endl ;
      //     cout << "INITIAL\txsize= " << joint.dimX() 
      // 	 << "\tysize= " << joint.dimY()  << endl ;
      
      for( int y = 0 ; y < joint.dimY()  ; ++y )
	for( int x = 0 ; x < joint.dimX()  ; ++x )
	  {
	    (*_pdfOverIterations)(x, y, _iteration ) = joint(x, y, 0) ;
	  }
      
      //     cout << "Copy done" << endl ;
      
      //     AimsData<float>::iterator destIter(&((*_pdfOverIterations)(0, 0, _iteration)));
      //     AimsData<float>::const_iterator fromIter( joint.begin() ),
      //       fromLast( joint.end() ) ;
      //     while( fromIter != fromLast ){
      //       *destIter = *fromIter ;
      //       ++fromIter ; ++destIter ;
      //     }
    }
  
  ++_iteration ;
}

template <class T, int D>
void 
MIRegistrationProbe<T,D>::end( )
{
  Writer<AimsData<float> > w( "jointpdf.ima" ) ;
  w.write( *_pdfOverIterations ) ;
  
  *_pdfOverIterations = 0 ;
  _iteration = 0 ;
}



int main( int argc, char* argv[] )


{
  //
  // Default values
  //
  char* c_interpolator      = "linear";
  char* c_optimizer         = "powell";
  char* c_pdf               = "direct";
  char* c_gc                = "yes";
  char* c_objfunc           = "mi";
  int numLevel              = 64;
  int maskSize              = 5;
  float error               = 0.00001;
  AimsVector<float,6>  param( 0.0 );
  AimsVector<float,6> deltaP( 0.0 );
  char             *fileRef = NULL, *fileTest = NULL, *fileOut = NULL;
  int sn                    = 0;
  int energySize           = 0;
  float testratio           = TEST_RATIO;
  float refratio            = REF_RATIO;


  // Collecte des arguments.
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { 'r',"reference"    ,AIMS_OPT_STRING,&fileRef   ,0                ,1},
  { 't',"test"         ,AIMS_OPT_STRING,&fileTest  ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileOut   ,0                ,0},
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
  { ' ',"interpolation",AIMS_OPT_STRING,&c_interpolator,0            ,0},
  { ' ',"optimization" ,AIMS_OPT_STRING,&c_optimizer,0               ,0},
  { ' ',"pdfcalc" ,     AIMS_OPT_STRING,&c_pdf,     0                ,0},
  { ' ',"gcinit" ,      AIMS_OPT_STRING,&c_gc,      0                ,0},
  { ' ',"index"        ,AIMS_OPT_STRING,&c_objfunc ,0                ,0},
  { ' ',"serialnum"    ,AIMS_OPT_INT   ,&sn        ,0                ,0},
  { ' ',"pays_size"    ,AIMS_OPT_INT   ,&energySize,0                ,0},
  { ' ',"graylevel"    ,AIMS_OPT_INT   ,&numLevel  ,0                ,0},
  { ' ',"masksize"     ,AIMS_OPT_INT   ,&maskSize  ,0                ,0},
  { ' ',"error"        ,AIMS_OPT_FLOAT ,&error     ,0                ,0},
  { ' ',"seuiltest"    ,AIMS_OPT_FLOAT ,&testratio ,0                ,0},
  { ' ',"seuilref"     ,AIMS_OPT_FLOAT ,&refratio  ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);



  

  //
  // choice of interpolation
  //
  Resampler<short>* interpolator = NULL;
  BucketSampler<short>*           comb = NULL;

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
      comb = new BucketSampler<short>;
    }

  //
  // choice of index to be minimized
  //
  ObjFuncBucket* objfunc = NULL;
  if ( c_objfunc )
  {
    ASSERT( string( c_objfunc ) == "mi" ||
            string( c_objfunc ) == "cr"   );
    if ( string( c_objfunc ) == "mi" )
    {
      objfunc = new MutualInfoFuncBucket( numLevel, interpolator, comb, maskSize );
      cout << "objective function   : mutual information" << endl;
    }
    else if ( string( c_objfunc ) == "cr" )
    {
      objfunc = new CorRatioFuncBucket( numLevel, interpolator, comb, maskSize );
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
  
  OptimizerProbe<float, 6> * probe = 
    new MIRegistrationProbe<float, 6>( numLevel, 20 ) ;
  optimizer->setProbe( probe ) ;











  //
  //Read data
  //    Declaration
  //    Read Hdr of test image
  //    Read data of ref image
  BucketMap<short>    ref;
  Reader <BucketMap<short > >* refBucketR = new Reader <BucketMap<short > >( fileRef );
  refBucketR->read( ref ) ;
  cout << "X " << ref.sizeX()<< " Y " << ref.sizeY()<< " Z " << ref.sizeZ() << endl ;
  
  AimsData<short>    test;
  CoarseReader      testR( fileTest ) ;
  testR.read( test );
  cout << "X " << test.sizeX()<< " Y " << test.sizeY()<< " Z " << test.sizeZ() << endl ;

  objfunc->setRef( ref );
  objfunc->setTest( test );







  //
  //Get gravity centers of the ref data....
  //
  BucketMap<short>::Bucket::const_iterator spatialIter, 
    spatialLast ;
  
  short max = ref.begin()->second.begin()->second ;
  
  spatialIter = ref.begin()->second.begin() ; 
  spatialLast = ref.begin()->second.end() ;
  while( spatialIter != spatialLast )
    {
      if( spatialIter->second > max ) max = spatialIter->second ;
      ++spatialIter ;
    }            
  
  spatialIter = ref.begin()->second.begin() ; 
  spatialLast = ref.begin()->second.end() ;
  {
    float gx, gy, gz, x, y, z ;
    int  gn;
    gx = gy = gz = 0.0; gn = 0;
    Point3df pmax(-1., -1., -1.), pmin(-1., -1., -1.) ;
    while( spatialIter != spatialLast )
      {
	bool firstIteration =  (pmax == Point3df(-1., -1., -1.)) ;
	if( spatialIter->second > (short)( testratio * max ) || 
	    firstIteration ){
	  x = (spatialIter->first)[0] ;
	  y = (spatialIter->first)[1] ;
	  z = (spatialIter->first)[2] ;
	  
	  if( x < pmin[0] || firstIteration ) pmin[0] = x ;
	  if( x < pmin[1] || firstIteration ) pmin[1] = y ;
	  if( x < pmin[2] || firstIteration ) pmin[2] = z ;
	  if( x > pmax[0] || firstIteration ) pmax[0] = x ;
	  if( y > pmax[1] || firstIteration ) pmax[1] = y ;
	  if( z > pmax[2] || firstIteration ) pmax[2] = z ;
	  
	  
	  gx += x ;
	  gy += y ;
	  gz += z ;
	  
	  gn++;
	}
	++spatialIter ;
      }
    gx = (gx/gn) * ref.sizeX();
    gy = (gy/gn) * ref.sizeY();
    gz = (gz/gn) * ref.sizeZ();
  
    cout << "Centre de gravite REF : gx = " << gx << "    gy = " << gy << "   gz = " << gz << endl;
  
    objfunc->setGcRef(gx, gy, gz);
  }





  //
  //Get gravity centers of the test data....
  //
  {
    float gx, gy, gz;
    int  gn;
    int x, y, z;
    AimsThreshold<short,short> thresh( AIMS_GREATER_THAN,
				       (short)( refratio * test.maximum() ), 0 );

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

    cout << "Centre de gravite TEST : gx = " << gx << "    gy = " << gy << "   gz = " << gz << endl << endl;

    objfunc->setGcTest(gx, gy, gz);

#ifdef DEBUG
cout << "DEBUG:MAIN>>"  << "refgx " << gx << "refgy " << gy << "refgz " << gz << endl;
#endif
    delete tmp;
//    if (string( c_gc ) != "yes")
//    {
//        KeepGC_x = gx; KeepGC_y = gy; KeepGC_z = gz;
//    }
  }







  //
  // State of input data.
  //
  cout << "Reference Image       : " << fileRef << endl;
  if ( sn > 0)
    {
      cout << "Frame seled from serie: " << sn << endl;
    }
  cout << "Test      Bucket       : " << fileTest << endl;
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


  AimsVector<float,6> p; 
  if ( deltaP == float(0.0) )
    {
      deltaP[0] = test.sizeX() / 2.0;
      deltaP[1] = test.sizeY() / 2.0;
      deltaP[3] = test.sizeZ() / 2.0;
      deltaP[2] = 180.0/ 3.14159 / test.dimX();
      deltaP[4] = 180.0/ 3.14159 / test.dimZ();
      deltaP[5] = 180.0/ 3.14159 / test.dimZ();
    }
  
  clock_t start = clock();  
  
  p = optimizer->doit( param, deltaP );
  // dump results
  // ----------------------------------------------
  
  clock_t finish = clock();
  cout << finish - start << " ticks" << endl;
  



  int i = 0 ;
  cout << "Volume " << i << "Popt " << p << endl;
  cout <<             " -Tx " << p[0];
  cout <<             " -Ty " << p[1];
  cout <<             " -Tz " << p[3];
  cout <<             " -Rx " << p[4];
  cout <<             " -Ry " << p[5];
  cout <<             " -Rz " << p[2] << endl;
  WriteCurrentDep(p, objfunc, fileTest, fileRef, i);
  // - - - - - - - - - - - - -End loop on frame

  // 
  // Desallocation
  //

  delete probe ;
  
  return EXIT_SUCCESS; 
}
