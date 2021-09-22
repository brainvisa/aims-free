/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

//============================================================================
// This command implements FFD deformation estimation as described in:
//   Mattes, Haynor, Vesselle, Lewellen & Eubank
//   IEEE Transactions on Medical Imaging (2003)
//============================================================================

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

//--- local ------------------------------------------------------------------
#include "lbfgsb.h"
#include "dataModel.h"
#include "scaleControl.h"
#include "ppdf.h"
//--- aims -------------------------------------------------------------------
#include <aims/getopt/getopt2.h>
#include <aims/io/channelR.h>
#include <aims/io/reader.h>
#include <aims/registration/ffd.h>
#include <aims/utility/clock.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/stream/fileutil.h>                      // carto::FileUtil
//----------------------------------------------------------------------------

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;
  try {

    //------------------------------------------------------------------------
    // Command line variables
    //------------------------------------------------------------------------
    string      deprecatedinputmotion("");
    string      inputtest(""), inputref(""), inputlist(""),
                inputmotion(""), output(""), initffd(""), initdep("");
    int         refchannel = -1, testchannel = -1;
    int         bref = 64, btest = 64;
    int         init_ctrl_numx = 4, init_ctrl_numy = 4, init_ctrl_numz = 4;
    int         final_ctrl_numx = 8, final_ctrl_numy = 8, final_ctrl_numz = 8;
    Point3d     incr(2, 2, 2);
    float       mi_ref = 0., gain_final_th = 0.01, gain_pyr_coef = 1.0;
    int         gain_first = 3;
    Point3df    defmax(-1., -1., -1.);
    bool        prepro = true;

    // debug
    string  dbg_dir("/tmp/");
    bool    dbg_write_all         = false;
    bool    dbg_write_jpdf        = false;
    bool    dbg_write_contrib     = false;
    bool    dbg_write_ctrlknots   = false;
    bool    dbg_write_derparam    = false;
    bool    dbg_write_deformation = false;
    bool    dbg_write_mi          = false;

    //------------------------------------------------------------------------
    // Command line arguments
    //------------------------------------------------------------------------
    AimsApplication application( argc, argv, "Estimation of FFD deformation" );

    application.addOption( inputref, "-r", "Input reference image" );
    application.addOption( inputtest, "-t", "Input test image" );
    application.addOption( refchannel, "-cr", "Channel of multi-channel reference image to use during registration estimation\n"
                                      "Possible types and values:\n"
                                      "type : RGB or RGBA\n"
                                      "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                      "type : HSV\n"
                                      "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                      "[default is set to norm]\n\n", true);
    application.addOption( testchannel, "-ct", "Channel of multi-channel test image to use during registration estimation\n"
                                      "Possible types and values:\n"
                                      "type : RGB or RGBA\n"
                                      "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                      "type : HSV\n"
                                      "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                      "[default is set to same value as reference channel]\n\n", true);
    application.addOption( output, "-o", "Output FFD field" );
    application.addOption( inputlist, "-l", "List of VOID bucket", true );
    application.addOption( deprecatedinputmotion, "-d", "Deprecated parameter. Please use -m option using inversed motion <test_TO_ref>.", true );
    application.addOption( inputmotion, "-m", "Rigid initial displacement <test_TO_ref>", true );
    application.addOption( initffd, "-initFFD", "Initial FFD field <ctrlshift.ima>", true );
    application.addOption( initdep, "-initDEP", "Initial Displacement field <Ex: derived from BlockMatching>", true );
    application.addOption( init_ctrl_numx, "-icx", "Initial number of control knots / X", true );
    application.addOption( init_ctrl_numy, "-icy", "Initial number of control knots / Y", true );
    application.addOption( init_ctrl_numz, "-icz", "Initial number of control knots / Z", true );
    application.addOption( final_ctrl_numx, "-fcx", "Final number of control knots / X", true );
    application.addOption( final_ctrl_numy, "-fcy", "Final number of control knots / Y", true );
    application.addOption( final_ctrl_numz, "-fcz", "Final number of control knots / Z", true );
    application.addOption( incr[0], "-ix", "Increment number of control knots / X", true );
    application.addOption( incr[1], "-iy", "Increment number of control knots / Y", true );
    application.addOption( incr[2], "-iz", "Increment number of control knots / Z", true );
    application.addOption( defmax[0], "-dx", "Maximum local deformation (mm) / X", true );
    application.addOption( defmax[1], "-dy", "Maximum local deformation (mm) / Y", true );
    application.addOption( defmax[2], "-dz", "Maximum local deformation (mm) / Z", true );
    application.addOption( gain_first, "-sfirst", "Nb of iterations before first gain computation (min 1) [default=3]", true );
    application.addOption( gain_final_th, "-sfin", "Gain threshold (for the final pyramid level) [default=0.01]>", true );
    application.addOption( gain_pyr_coef, "-scoef", "Reduction of the gain threshold / pyramid level [default=1=const]", true );
    application.addOption( bref, "-bref", "Number of bins to use for ref histogram [default=64]>", true );
    application.addOption( btest, "-btest", "Number of bins to use for test histogram [default=64]>", true );
    application.addOption( prepro, "-pre", "Preprocess volumes (flatten masks, 99% cutoff [default=true]", true );
    application.addOption( dbg_dir, "-dbgPath", "[debug] Directory to write intermediate files [default=/tmp/]", true );
    application.addOption( dbg_write_all, "-dbgWriteAll", "[debug] Write all intermediate files [default=false]", true );
    application.addOption( dbg_write_jpdf, "-dbgWriteJPDF", "[debug] Write joint histograms [default=false]", true );
    application.addOption( dbg_write_ctrlknots, "-dbgWriteParam", "[debug] Write parameters (i.e. FFD control knots) [default=false]", true );
    application.addOption( dbg_write_derparam, "-dbgWriteDerParam", "[debug] Write parameter derivatives [default=false]", true );
    application.addOption( dbg_write_deformation, "-dbgWriteDef", "[debug] Write deformations [default=false]", true );
    application.addOption( dbg_write_contrib, "-dbgWriteContrib", "[debug] Write joint histogram's contributing points [default=false]", true );
    application.addOption( dbg_write_mi, "-dbgWriteMI", "[debug] Write the successive mutual information scores [default=false]", true );
    application.initialize();

    if (deprecatedinputmotion.size() > 0) {
      cerr << "-d option is deprecated. Please use -m option with inversed motion <test_TO_ref>." << endl << flush;
      return EXIT_FAILURE;
    }

    //========================================================================
    //
    //      Read input Debug options
    //
    //========================================================================

    // directory for "degub writing
    dbg_dir = FileUtil::dirname(dbg_dir) + FileUtil::separator();
    cout << dbg_dir << endl;

    // filename for debug writing
    char dbg_name[150];

    if( dbg_write_all )
    {
      dbg_write_jpdf        = true;
      dbg_write_contrib     = true;
      dbg_write_ctrlknots   = true;
      dbg_write_derparam    = true;
      dbg_write_deformation = true;
      dbg_write_mi          = true;
    }

    //------------------------------------------------------------------------
    // Init comp timing
    //------------------------------------------------------------------------
    Clock time;

    //========================================================================
    //
    //      Read input data
    //
    //========================================================================

    if( testchannel == -1 )
      testchannel = refchannel;

    //------------------------------------------------------------------------
    // Reference image
    //------------------------------------------------------------------------
    cout << "Reading reference image" << endl;
    ChannelReader<VolumeRef<int16_t> > rdref(inputref);
    VolumeRef<int16_t> reference;
    rdref.read(reference, refchannel);

    //------------------------------------------------------------------------
    // Test image
    //------------------------------------------------------------------------
    cout << "Reading test image" << endl;
    VolumeRef<int16_t>  testimg;
    ChannelReader<VolumeRef<int16_t> > rdtest(inputtest);
    rdtest.read(testimg, testchannel);

    //------------------------------------------------------------------------
    // Evaluation points
    //------------------------------------------------------------------------
    BucketMap<Void> bucketMap;
    if( !inputlist.empty() ) {
      cout << "Reading bucket of evaluation points" << endl;
      Reader<BucketMap<Void> > rdlist(inputlist);
      rdlist.read(bucketMap);
    }

    //------------------------------------------------------------------------
    // Affine motion
    //------------------------------------------------------------------------
    AffineTransformation3d motion; // default is Identity
    if( !inputmotion.empty() ) {
      cout << "Reading affine motion" << endl;
      Reader<AffineTransformation3d> mrd(inputmotion);
      mrd.read( motion );
      motion = motion.inverse();
    }
    // motion.scale( Point3df( reference.sizeX(), reference.sizeY(), reference.sizeZ() ),
    //               Point3df( testimg.sizeX(),   testimg.sizeY(),   testimg.sizeZ()   ) );

    //------------------------------------------------------------------------
    // FFD initialization
    //------------------------------------------------------------------------
    VolumeRef<Point3df> iffd;
    if( !initffd.empty() || !initdep.empty() )
    {
      cout << "Reading initial FFD deformations: " << initffd << endl;
      Reader<VolumeRef<Point3df> > rdiffd( ( !initffd.empty() ? initffd : initdep ) );
      rdiffd >> iffd;

      init_ctrl_numx = iffd->getSizeX();
      init_ctrl_numy = iffd->getSizeY();
      init_ctrl_numz = iffd->getSizeZ();
    }

    //========================================================================
    //
    //      Manage 2D cases
    //
    //========================================================================
    bool is2d[3];
    is2d[0] = ( testimg->getSizeX() == 1 );
    is2d[1] = ( testimg->getSizeY() == 1 );
    is2d[2] = ( testimg->getSizeZ() == 1 );
    if( is2d[0] ) {
      init_ctrl_numx = 1;
      final_ctrl_numx = 1;
      incr[0] = 0;
    }
    if( is2d[1] ) {
      init_ctrl_numy = 1;
      final_ctrl_numy = 1;
      incr[1] = 0;
    }
    if( is2d[2] ) {
      init_ctrl_numz = 1;
      final_ctrl_numz = 1;
      incr[2] = 0;
    }

    //========================================================================
    //
    //      Check control knots parameters for the pyramid
    //
    //========================================================================
    if (!is2d[0] && ((init_ctrl_numx < 4) || (init_ctrl_numx > 10))) {
      cerr << "Wrong -icx option value: " << carto::toString(init_ctrl_numx)
           << ". Available values for this option are in the range 4-10."
           << endl << flush;
      return EXIT_FAILURE;
    }

    if (!is2d[1] && ((init_ctrl_numy < 4) || (init_ctrl_numy > 10))) {
      cerr << "Wrong -icy option value: " << carto::toString(init_ctrl_numy)
           << ". Available values for this option are in the range 4-10."
           << endl << flush;
      return EXIT_FAILURE;
    }

    if (!is2d[2] && ((init_ctrl_numz < 4) || (init_ctrl_numz > 10))) {
      cerr << "Wrong -icz option value: " << carto::toString(init_ctrl_numz)
           << ". Available values for this option are in the range 4-10."
           << endl << flush;
      return EXIT_FAILURE;
    }

    if (!is2d[0] && ((final_ctrl_numx < init_ctrl_numx) || (final_ctrl_numx > 10))) {
      cerr << "Wrong -fcx option value: " << carto::toString(final_ctrl_numx)
           << ". Available values for this option are in the range "
           << carto::toString(init_ctrl_numx) << "-10." << endl << flush;
      return EXIT_FAILURE;
    }

    if (!is2d[1] && ((final_ctrl_numy < init_ctrl_numy) || (final_ctrl_numy > 10))) {
      cerr << "Wrong -fcy option value: " << carto::toString(final_ctrl_numy)
           << ". Available values for this option are in the range "
           << carto::toString(init_ctrl_numy) << "-10." << endl << flush;
      return EXIT_FAILURE;
    }

    if (!is2d[2] && ((final_ctrl_numz < init_ctrl_numz) || (final_ctrl_numz > 10))) {
      cerr << "Wrong -fcz option value: " << carto::toString(final_ctrl_numz)
           << ". Available values for this option are in the range "
           << carto::toString(init_ctrl_numz) << "-10." << endl << flush;
      return EXIT_FAILURE;
    }

    //========================================================================
    //
    //      FFD initialization
    //
    //========================================================================
    cout << "=====================================================" << endl;
    int dimParam = !is2d[0] + !is2d[1] + !is2d[2];
    cout << "FFD dimension: " << dimParam << "D" << endl;

    //------------------------------------------------------------------------
    // Init variables
    //------------------------------------------------------------------------

    // in defmax, convert mm to voxels
    // defmax[0] /= testimg.sizeX();
    // defmax[1] /= testimg.sizeY();
    // defmax[2] /= testimg.sizeZ();

    cout << "FFD grid dimension: "
         << init_ctrl_numx << " x     "
         << init_ctrl_numy << " y     "
         << init_ctrl_numz << " z" << endl;

    //------------------------------------------------------------------------
    // Create and init FFD object
    //------------------------------------------------------------------------
    bool test_init = true;       // do ffd init in loop ?
    cout << "Initializing FFD deformations... " << endl;
    SplineFfd deformation( init_ctrl_numx, init_ctrl_numy, init_ctrl_numz,
                           *testimg );

    if( !initffd.empty() )
    {
      deformation.updateAllCtrlKnot( iffd );
      test_init = false;
      cout << "Initialization using initFFD done" << endl;
    }

    if( !initdep.empty() )
    {
      deformation.updateAllCtrlKnotFromDeformation( iffd );
      test_init = false;
      cout << "Initialization using initDEP done" << endl;
    }

    //------------------------------------------------------------------------
    // Create and init PPDF object
    //------------------------------------------------------------------------
    cout << "Initializing PDF..." << endl;
    ParzenProbDensFunction pPdf( deformation, is2d[0], is2d[1], is2d[2],
                                 bref, btest );

    //------------------------------------------------------------------------
    // Create and init evaluation object
    //------------------------------------------------------------------------
    cout << "Initializing data model..." << endl;
    DataModel dataModel( bucketMap,
                         reference,
                         testimg,
                         motion,
                         deformation,
                         pPdf,
                         prepro );
    dataModel.initBound( defmax );

    //========================================================================
    //
    //      Loop on pyramid levels
    //
    //========================================================================

    //------------------------------------------------------------------------
    // Optimization variables
    //------------------------------------------------------------------------
    LbfgsbOptimizer lbfgsbOptimizer;   // L-BFGS-B optimize
    vector<float> param;               // optimized FFD parameters
    list<float>   mi_list;             // list of MI at each step

    // Compute initial gain_pyr_th
    float gain_pyr_th = gain_final_th;
    int pyr_count = 1;
    if( gain_pyr_coef > 0 )
    {
      int curx = init_ctrl_numx, cury = init_ctrl_numy, curz = init_ctrl_numz;
      while( curx*cury*curz < final_ctrl_numx*final_ctrl_numy*final_ctrl_numz )
      {
        gain_pyr_th *= gain_pyr_coef;
        curx += incr[0];
        cury += incr[1];
        curz += incr[2];
        ++pyr_count;
      }
    }

    //------------------------------------------------------------------------
    // loop variables
    //------------------------------------------------------------------------
    VolumeRef<Point3df> ct; // Intermediate and final control points
    int incr_limit = 0;
    int  incr_max = ( dimParam * final_ctrl_numx
                               * final_ctrl_numy
                               * final_ctrl_numz );
    bool do_optimize = true;
    bool update_mi_ref = true;   // test to allow new criterion for new pyramid level
    bool first_optimize = true;
    bool do_update_bounds = true;

    //------------------------------------------------------------------------
    // Loop
    //------------------------------------------------------------------------
    int c_pyr = 1, c_opt;
    do
    {
      c_opt = 0;
      first_optimize = true;
      incr_limit = dimParam * deformation.dimX()
                            * deformation.dimY()
                            * deformation.dimZ();

      cout << endl;
      cout << "=====================================================" << endl;
      cout << "               Pyramid level " << c_pyr << endl;
      cout << "-----------------------------------------------------" << endl;
      cout << "\tResolution of the control point grid :" << endl;
      cout << endl;
      cout << "\tx = " << deformation.dimX();
      cout << "\ty = " << deformation.dimY();
      cout << "\tz = " << deformation.dimZ();
      cout << endl;
      cout << "\tincr_limit = " << incr_limit
           << "\tincr_max = " << incr_max
           <<  endl;
      cout << "\tGain threshold = " << gain_pyr_th   << endl;
      cout << endl;
      cout << "=====================================================" << endl;
      cout << endl;

      // Initialisation de l optimiseur
      lbfgsbOptimizer.init();

      //----------------------------------------------------------------------
      // Parameters initialization
      //----------------------------------------------------------------------
      if(test_init)
      {
        cout << "Initiate parameters" << endl;
        param = vector<float>( dimParam
                               * deformation.dimX()
                               * deformation.dimY()
                               * deformation.dimZ(),
                               0.0 );
        test_init = false;
      }
      //---> End init
      else
      //----------------------------------------------------------------------
      // Update parameters
      //----------------------------------------------------------------------
      {
        cout << "Update parameters" << endl;
        param.resize( dimParam * deformation.dimX()
                               * deformation.dimY()
                               * deformation.dimZ() );

        int ind = 0;
        for( int k = 0; k < deformation.dimZ(); ++k )
          for( int j = 0; j < deformation.dimY(); ++j )
            for( int i = 0; i < deformation.dimX(); ++i )
              for( int c = 0; c < 3; ++c )
              {
                if( !is2d[c] )
                  param[ind++] = deformation.getCtrlKnot(i, j, k)[c];
              }
      }
      //---> end update parameters

      //----------------------------------------------------------------------
      // Optimization loop
      //----------------------------------------------------------------------
      while( first_optimize || ( lbfgsbOptimizer.cont() && do_optimize ) )
      {
        first_optimize = false;

        //---< Update MI
        if ( !lbfgsbOptimizer.updateAvailable() )
        {

          //--- update -------------------------------------------------------
          dataModel.update(param);
          if( do_update_bounds ) {
            dataModel.updateBound();
            do_update_bounds = false;
          }

          if(update_mi_ref)
          {
            cout << "\nUpdate reference MI value = " << dataModel.mutInfo() << endl << endl;
            mi_ref = dataModel.mutInfo();
            update_mi_ref = false;
          }

          mi_list.push_back(dataModel.mutInfo());
          cout << "\t\t\t\tvalue = " << mi_list.back() << endl;

          //--- debug --------------------------------------------------------
          if( dbg_write_ctrlknots )
          {
            sprintf( dbg_name, "%sctrlknots_p%d_o%d.nii", dbg_dir.c_str(), c_pyr, c_opt );
            cout << string("[dbg] ") + dbg_name << endl;
            dataModel.writeDebugCtrlKnots( dbg_name );
          }
          if( dbg_write_derparam )
          {
            sprintf( dbg_name, "%sderparam_p%d_o%d.nii", dbg_dir.c_str(), c_pyr, c_opt );
            cout << string("[dbg] ") + dbg_name << endl;
            dataModel.writeDebugDerMI( dbg_name );
          }
          if( dbg_write_jpdf )
          {
            sprintf(dbg_name, "%sjoint_hist_p%d_o%d.nii", dbg_dir.c_str(), c_pyr, c_opt );
            cout << string("[dbg] ") + dbg_name << endl;
            dataModel.writeDebugJPdf( dbg_name );
          }
          if( dbg_write_contrib )
          {
            sprintf(dbg_name, "%scontrib_p%d_o%d.nii", dbg_dir.c_str(), c_pyr, c_opt );
            cout << string("[dbg] ") + dbg_name << endl;
            dataModel.writeDebugContrib( dbg_name );
          }
          if( dbg_write_deformation )
          {
            sprintf(dbg_name, "%sdeformation_p%d_o%d.nii", dbg_dir.c_str(), c_pyr, c_opt );
            cout << string("[dbg] ") + dbg_name << endl;
            vector<float> vs = testimg->getVoxelSize();
            dataModel.writeDebugDeformations( dbg_name,
                                              testimg->getSizeX(),
                                              testimg->getSizeY(),
                                              testimg->getSizeZ(),
                                              vs[0], vs[1], vs[2] );
          }

          //--- test condition -----------------------------------------------
          if( c_opt >= gain_first )
          {
            float mi_last1 = mi_list.back();
            mi_list.pop_back();
            float mi_last2 = mi_list.back();
            mi_list.push_back(mi_last1);
            // /!\ mi are negatives here
            float gain = ( mi_last2 - mi_last1 ) / ( -mi_last2 );

            cout << "\t\t\t\tgain : " << gain << endl ;

            if( 0 <= gain && gain < gain_pyr_th && mi_last1 <= mi_ref )
            {
                cout << "\t\t\t\tTest condition satisfied" << endl;
                do_optimize = false;
                break;
            }
          }

          ++c_opt;
        }
        //--> end update MI

        // Optimize parameters
        if( do_optimize )
        {
          param = lbfgsbOptimizer.getMinDir( param,
                                             dataModel.mutInfo(),
                                             dataModel.derMutInfo(),
                                             dataModel.getLowerBound(),
                                             dataModel.getUpperBound() );

          lbfgsbOptimizer.status();
        }
      }
      //---> End optimization loop

      //----------------------------------------------------------------------
      // Increase pyramid level
      //----------------------------------------------------------------------
      if( incr_limit < incr_max )
      {
        // Increment du nb de points de controle : Spline / dataModel / ppdf
        deformation.increaseResolution(incr);
        dataModel.resizeParam( dimParam, deformation.dimX(),
                                         deformation.dimY(),
                                         deformation.dimZ() );



        // Modif de la condition d arret
        gain_pyr_th /= gain_pyr_coef;

        // Reinit  MI / TestFunc
        update_mi_ref = true;
        do_optimize = true;
        do_update_bounds = true;
      }
      ++c_pyr;
    }
    while( incr_limit < incr_max ); // Nbre max de pts de ctrl + cond sur critere OK


    //========================================================================
    //
    //      Write results
    //
    //========================================================================

    //------------------------------------------------------------------------
    // Write output deformation field
    //------------------------------------------------------------------------
    if( !output.empty() )
      deformation.write( output );

    //--- debug --------------------------------------------------------------
    if( dbg_write_mi )
    {
      string file_mi = dbg_dir + "mi.txt";
      ofstream write_mi(file_mi.c_str());
      for( list<float>::const_iterator it = mi_list.begin();
           it != mi_list.end(); ++it)
      {
        write_mi << *it << endl;
        cout << *it << endl;
      }
      write_mi.close();
    }


    //--- computing time -----------------------------------------------------
    cout << "Computing time = " << time.laps() << " sec" << endl;

    return( 0 );

  }
  catch( user_interruption &e )
  {}
  catch( std::exception &e )
  {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume< VolumeRef< Point3dd > >;
template class carto::Volume< Point3dd >;

