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
#include "miProbe.h"
#include "mutualfunc.h"
#include "corratiofunc.h"
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/io/motionW.h>
#include <aims/io/channelR.h>
#include <aims/math/math_g.h>
#include <aims/io/datatypecode.h>
#include <aims/utility/utility_g.h>
#include <aims/information/pdf.h>
#include <aims/optimization/optimization_g.h>
#include <aims/pyramid/pyramid_g.h>
#include <aims/resampling/resampling_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <iomanip>
#include <time.h>

#include <stdio.h>
#include <cartobase/stream/fileutil.h>

using namespace carto;
using namespace aims;
using namespace std;

const float REF_RATIO = 0.05;
const float TEST_RATIO = 0.1;

// #define DEBUG 1

// CM' = R.CM + T
// OM' = (Id - R).OC + T + R.OM
//        terme NEWTRANS


void WriteCurrentDepInFile(
          AimsVector<float,6>& p,
          ObjFunc             *objfunc,
          const string        &direct,
          const string        &inverse,
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

// void WriteCurrentDep(
//           AimsVector<float,6>& p,
//           ObjFunc             *objfunc,
//           const string        &ftest,
//           const string        &fref,
//           int                 notFirstCall)
// {
//   // Build canonical name
//   string from = string(ftest), to = string( fref );
//   if ( from.rfind('/') != string::npos ) 
//     from = from.substr( from.rfind('/')+1, from.size() );
//   if ( from.rfind('.') != string::npos ) 
//     from = from.substr( 0, from.rfind('.') );
//   if ( to.rfind('/') != string::npos ) 
//     to = to.substr( to.rfind('/')+1, to.size() );
//   if ( to.rfind('.') != string::npos ) 
//     to = to.substr( 0, to.rfind('.') );
// 
//   // Pour enregistrer les fichiers dans le repertoire de "ftest" en commencant leur nom par "inputtest_TO_inputref_":
//   string transfo_path = FileUtil::dirname(string (ftest)) + FileUtil::separator() ;
// 
//   string direct = transfo_path + from + "_TO_" + to;
//   string inverse = transfo_path + to + "_TO_" + from;
// 
//   WriteCurrentDepInFile(p, objfunc, direct, inverse, notFirstCall);
// }

int main( int argc, const char** argv )
{
  //
  // Default values
  //
  int refchannel = -1, testchannel = -1; // Default is the norm channel
  string c_interpolator      = "linear";
  string c_optimizer         = "powell";
  string c_pdf               = "pv";
  string c_gc                = "yes";
  string c_objfunc           = "mi";
  int   numLevel            = 64;
  int   maskSize            = 5;
  float error               = 0.01;
  AimsVector<float,6>  param( 0.0 );
  AimsVector<float,6> deltaP( 0.0 );

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

  int rsp = 0, rep = 0, tsp = 0, tep = 0;

  string fileTest, fileRef;
  string fileLog;
  Writer<Motion> directTrans, inverseTrans;

  AimsApplication app( argc, argv,
                       "Registration according to Mutual Information Method" );
  app.addOption( fileRef, "-r", "source reference volume" );
  app.alias( "--reference", "-r" );
  app.addOption( fileTest, "-t", "source test volume to register)" );
  app.alias( "--test", "-t" );
  app.addOption( refchannel, "-cr",  "Channel of multi-channel reference image to use during registration estimation\n"
                                      "Possible types and values:\n"
                                      "type : RGB or RGBA\n"
                                      "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                      "type : HSV\n"
                                      "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                      "[default is set to norm]\n\n", true);
  app.addOption( testchannel, "-ct",  "Channel of multi-channel test image to use during registration estimation\n"
                                      "Possible types and values:\n"
                                      "type : RGB or RGBA\n"
                                      "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                      "type : HSV\n"
                                      "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                      "[default is set to same value as reference channel]\n\n", true);
  app.addOption( directTrans, "--dir",
                 "test_TO_ref: output transfomation filename "
                 "[default=<test>_TO_<ref>.trm]", true );
  app.addOption( inverseTrans, "--inv",
                 "ref_TO_test: output transfomation filename "
                 "[default=<ref>_TO_<test>.trm]", true );
  app.addOption( fileLog, "-l", "output log file", true );
  app.alias( "--logger", "-l" );
  app.addOption( sn, "--serialnum", "number of the frame to consider for ref "
      "[default 0], relevant for dynamic series ref images", true );
  app.addOption( param[0], "--Tx",
                 "Init Translaction coord [default=0.]", true );
  app.addOption( param[1], "--Ty",
                 "Init Translaction coord [default=0.]", true );
  app.addOption( param[3], "--Tz",
                 "Init Translaction coord [default=0.]", true );
  app.addOption( deltaP[0], "--dTx",
                 "Increment for variable Tx [default=heuristics in code]",
                 true );
  app.addOption( deltaP[1], "--dTy",
                 "Increment for variable Ty [default=heuristics in code]",
                 true );
  app.addOption( deltaP[3], "--dTz",
                 "Increment for variable Tz [default=heuristics in code]",
                 true );
  app.addOption( param[4], "--Rx", "Init Rotation coord [default=0.]", true );
  app.addOption( param[5], "--Ry", "Init Rotation coord [default=0.]", true );
  app.addOption( param[2], "--Rz", "Init Rotation coord [default=0.]", true );
  app.addOption( deltaP[4], "--dRx",
                 "Increment for variable Rx [default=heuristics in code]",
                 true );
  app.addOption( deltaP[5], "--dRy",
                 "Increment for variable Ry [default=heuristics in code]",
                 true );
  app.addOption( deltaP[2], "--dRz",
                 "Increment for variable Rz [default=heuristics in code]",
                 true );
  app.addOption( c_interpolator, "--interpolation",
                 "interpolation  nearest/linear/spline [default=linear]",
                 true );
  app.addOption( c_optimizer, "--optimization",
                 "optimaizer  powell/random/single [default=powell]\n"
                 "single --> get a one shot evaluation\n"
                 "random --> sample cost value in a given area\n"
                 "powell --> cost minimization with powell algorithm", true );
  app.addOption( c_pdf, "--pdfcalc",
                 "Pdf calculus mode : direct ou pv [default=pv]",
                 true );
  app.addOption( c_gc, "--gcinit",
                 "Init with GravCenter: yes/no [default=yes]", true );
  app.addOption( c_objfunc, "--index",
                 "index to be optimized mi/cr [default=mi]\n"
                 "Mutual Information --> mi\n"
                 "Correlation Ratio  --> cr", true );
  app.addOption( rsp, "--refstartpyr",
                 "level of the multiresolution optimization:\n"
                 "start resolution level for ref\n", true );
  app.addOption( rep, "--refendpyr",
                 "level of the multiresolution optimization:\n"
                 "end resolution level for ref", true );
  app.addOption( tsp, "--teststartpyr",
                 "level of the multiresolution optimization:\n"
                 "start resolution level for tes", true );
  app.addOption( tep, "--testendpyr",
                 "level of the multiresolution optimization:\n"
                 "end resolution level for tes", true );
  app.addOption( energySize, "--pays_size", "Half size the volume generated "
                 "if optimization=other activated", true );
  app.addOption( numLevel, "--graylevel",
                 "numb of classes in histogram pdf estimation [default=64]",
                 true );
  app.addOption( maskSize, "--masksize", "size of the gaussian convolution "
                 "mask in pdf estimation for Mutual Information [default=5]",
                 true );
  app.addOption( error, "--error", "tolerance on results [default=0.01]",
                 true );
  app.addOption( refratio, "--threshref",
                 "relative thresh applied prior to grav cent estimation",
                 true );
  app.alias( "--seuilref", "--threshref" );
  app.addOption( testratio, "--threshtest",
                 "relative thresh applied prior to grav cent estimation",
                 true );
  app.alias( "--seuiltest", "--threshtest" );

  try
  {
    app.initialize();
    
    if (testchannel == -1) {
      testchannel = refchannel;
    }
    
    // 
    // Log file management
    //
    if( !fileLog.empty() )
    {
      mycout.open( fileLog.c_str() );
    }

    //
    // choice of interpolation
    //
    Resampler<short>* interpolator = NULL;
    Sampler<short>*           comb = NULL;

    ASSERT( c_pdf == "direct" || c_pdf == "pv" );
    if( c_pdf == "direct" )
    {
      if( !c_interpolator.empty() )
        {
          ASSERT( c_interpolator == "nearest" ||
                  c_interpolator == "linear"  ||
                  c_interpolator == "spline"    );
          if( c_interpolator == "nearest" )
          {
            //       interpolator = new NNInterpolator;
            cout << "interpolator : " << c_interpolator << endl;
            cout << "Not implemented yet" << endl;
          }
          else if( c_interpolator == "linear" )
          {
            interpolator = new MaskLinearResampler<short>;
            cout << "interpolator    : " << c_interpolator << endl;
          }
          else if( c_interpolator == "spline" )
          {
            //       interpolator = new SplineInterpolator;
            cout << "interpolator     : " << c_interpolator << endl;
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
    if( !c_objfunc.empty() )
    {
      ASSERT( c_objfunc == "mi" ||
              c_objfunc == "cr"   );
      if( c_objfunc == "mi" )
      {
        objfunc = new MutualInfoFunc( numLevel, interpolator, comb, maskSize );
        cout << "objective function   : mutual information" << endl;
      }
      else if( c_objfunc == "cr" )
      {
        objfunc = new CorRatioFunc( numLevel, interpolator, comb, maskSize );
        cout << "objective function   : correlation ratio" << endl;
      }
    }


    //
    // choice of optimizer
    //
    Optimizer<float,6>* optimizer = NULL;
    if( !c_optimizer.empty() )
    {
      ASSERT( c_optimizer == "powell" ||
              c_optimizer == "random" ||
              c_optimizer == "single"    );
      if( c_optimizer == "powell" )
      {
        optimizer = new PowellOptimizer<float,6>( *objfunc, error );
        cout << "optimizer            : powell" << endl;
      }
      else if( c_optimizer == "random" )
      {
        optimizer = new DetermOptimizer<float,6>( *objfunc, error, 10000, 100, 
              true );
        cout << "optimizer            : deterministic" << endl;
      }
      else if( c_optimizer == "single" )
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
    AimsData<short>    ref;
    
    ChannelReader< AimsData<short> > rdref(fileRef);
    ChannelReader< AimsData<short> > rdtest(fileTest);
    cout << "Reading channel " << carto::toString(refchannel) << " of reference image" << endl << flush;
    rdref.read(ref, refchannel, 0, 0, 0 );

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
    int sizeT = 1;
    if ( ph->getProperty("volume_dimension", v ) )
    {
      if( v.size() >= 4 )
        sizeT = v[3];
    }

    //
    //Get gravity centers if the ref image
    //
    float KeepGC_x=0.0, KeepGC_y=0.0, KeepGC_z=0.0;

    {
      float gx, gy, gz;
      int gn;
      int x, y, z;
      AimsThreshold<short,short> thresh( AIMS_GREATER_THAN,
                                        (short)( refratio * ref.maximum() ),
                                          0 );
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
      cout << "DEBUG:MAIN>>"  << "refgx " << gx << "refgy " << gy << "refgz "
          << gz << endl;
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
      rdtest.read( test, testchannel, 0, &f , i );

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
        
        AimsThreshold<short,short>
                  thresh( AIMS_GREATER_THAN,
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
        cout << "DEBUG:MAIN>>" << "testgx " << gx << "testgy " << gy
                  << "testgz " << gz << endl;
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
      
      
      if( directTrans.fileName().empty() ) {
        // Get default file name
        string transfo_path = FileUtil::dirname(fileTest);
        string from = FileUtil::removeExtension(FileUtil::basename(fileTest));
        string to = FileUtil::removeExtension(FileUtil::basename(fileRef));
        directTrans.setFileName(transfo_path + FileUtil::separator() + from + "_TO_" + to + ".trm");
      }
        
      if ( inverseTrans.fileName().empty() ) {
        // Get default file name
        string transfo_path = FileUtil::dirname(fileTest);
        string from = FileUtil::removeExtension(FileUtil::basename(fileTest));
        string to = FileUtil::removeExtension(FileUtil::basename(fileRef));
        inverseTrans.setFileName(transfo_path + FileUtil::separator() + to + "_TO_" + from + ".trm");
      }

      WriteCurrentDepInFile(p, objfunc, directTrans.fileName(),
                            inverseTrans.fileName(), i);
//       WriteCurrentDep(p, objfunc, fileTest, fileRef, i);
    }                            // - - - - - - - - - - - - -End loop on frame

    if( !fileLog.empty() )
    {
      mycout.close( );
    }

    return EXIT_SUCCESS; 
  }
  catch( user_interruption & ) {}
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}

