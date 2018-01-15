/*
 *  Copyright (C) 2000-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 */


#include <aims/io/reader.h>
#include <aims/fibers/bundles.h>
#include <aims/getopt/getopt2.h>
#include <aims/resampling/linearInterpolator.h>
#include <aims/fibers/bundles_features.h>
#include <cartobase/config/verbose.h>

using namespace std;
using namespace carto;
using namespace aims;


int main( int argc, const char *argv[] )
{
  int result = EXIT_SUCCESS;

  try
  {
    string bundlesFile;
    string motionFile;
    string splitRoiFile;
    string splitRoiMotion;
    bool splitKeepOriginalBundle = false;
    string selectRoiFile;
    string selectRoiMotion;
    string selectRulesFile;
    float selectROIMinimumOverlap = 0;
    double minimumLength = 0;
    vector< string > imageStatistics;
    string statFileName;
    string graphFile;
    string bundlesOutputFile;
    size_t numberOfSlices = 0;
    Writer< BundleSlicesToMesh::TimeMesh_t > slicesMesh;

    // option(s) management
    AimsApplication 
    application( argc, argv,
     "This command can be used for several purposes :\n\n"
     "* Get quantitative information about bundles. This information is "
     "written in a text file containing a table (one column per information, "
     "one row per bundle). See -o and --imageStatistics options.\n\n"
     "* Transform a bundle in another bundle according to several filters. See"
     " -t, --split and -o options.\n\n"
     "* Convert a bundles file to an Anatomist graph. See -g option." );
    application.addOption( bundlesFile, "-i",
                           "Bundles input file" );
    application.addOption( motionFile, "-t",
                           "Transformation matrix to apply to input file "
                           "(default: no transformation). The transformation "
                           "is applyed before any other processing.", true );
    application.addOption( selectRoiFile, "--selectionROI",
      "ROI graph that will be used to select fibers and put them in bundles "
      "according to the rule file from --selectionRules parameter."
      , true );
    application.addOption( selectRoiMotion, "--selectionROITransformation", 
     "Transformation matrix to apply to selection ROI graph (option "
     "--selectionROI). Default: no transformation.", true );
    application.addOption( selectRulesFile, "--selectionRules", 
     "Name of file containing selection rules. This option must be used if "
     "and only if --selectionROI is used.", true );
    application.addOption( selectROIMinimumOverlap, "--selectionOverlap", 
     "Percentage of point of a fascicle that have to be in a ROI to consider"
     " that the ROI is touched by the fascicle. 0 (the default) means that "
     "only one point in the ROI is engough, and 100 mean that all the "
     "fascicle points must be inside a ROI. This option is used only with "
     "--selectionROI.", true );
    application.addOption( splitRoiFile, "--split",
      "ROI graph that will be used to split the input bundle curves according "
      "to the ROI boundaries. This option transform the input bundle in "
      "another splitted bundle. The splitted bundle is used for processing "
      "that generate output.", true );
    application.addOption( splitRoiMotion, "--splitTransformation", 
     "Transformation matrix to apply to split ROI graph (option --split). "
     "Default: no transformation.", true );
    application.addOption( splitKeepOriginalBundle, "--splitKeepBundle", 
     "Keeps each fascicle original bundle name in output bundle "
                           "(default = false). ", true );
    application.addOption( minimumLength, "--minimumLength",
      "If greater than 0, all curves whose length is below minimumLength are "
      "ignored. This curve filtering is applied after all bundle "
      "transformation (-t and --split options) and before any other "
      "processing. Default value is 0 (i.e. no filtering).", true );    
    application.addOptionSeries( imageStatistics, "--imageStatistics", 
      "Compute statistics from a 3D image. All bundle points are projected in"
      " the image to get a voxel value (with trilinear interpolation). For "
      "each bundle, the mean, standard deviation, minimum and maximum of voxel"
      " values are stored in the result file. Each argument of this parameter "
      "must have the syntax <label>:<filename> where <label> is a label and "
      "<filename> is a 3D image file name. For each argument, three columns "
      "are added in the result file: <label>_mean, <label>_stddev, <label>_min"
      " and <label>_max." );
    application.addOption( numberOfSlices, "--slices",
     "number of slices in which each bundle is cut", true );
    application.addOption( slicesMesh, "--slicesMesh", 
      "Generate a mesh allowing to check the shape of the slices.", true );
    application.addOption( statFileName, "-o", 
      "Generate a text file containing quantitative information about the "
      "bundles. The file contains the following fields (in addition to the "
      "fields coming from --imageStatistics option):\n"
      "  curves_count: the number of curves in a bundle\n"
      "  points_count: the total number of curves points in a bundle\n"
      "  length_mean: the mean curves length in a bundle.\n"
      "  length_stddev: the standard deviation of curves length in a bundle.\n"
      "  length_min: the minimum of curves length in a bundle.\n"
      "  length_max: the maximum of curves length in a bundle.\n", true );
    application.addOption( graphFile, "-g", 
      "Create and save an ROI graph containing one node per bundle and meshes "
      "to visualize curves in Anatomist.", true );
    application.addOption( bundlesOutputFile, "-b",
     "Save the bundles resulting from all requeted transformation applied to "
     "input bundles (options -t and --split)", true );
    application.initialize();

    if ( verbose ) cout << "Creating bundles reader: " << flush;
    BundleReader bundleReader( bundlesFile );
    BundleProducer *finalProducer = &bundleReader;
    if ( verbose ) cout << "done" << endl;


    rc_ptr< BundleMotion > bundleMotion;
    if ( motionFile.size() ) {
      if ( verbose ) cout << "Creating motion filter: " << flush;
      bundleMotion.reset( new BundleMotion( motionFile ) );
      finalProducer->addBundleListener( *bundleMotion );
      finalProducer = bundleMotion.get();
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< BundleROISelect > bundleROISelect;
    if ( ! selectRoiFile.empty() ) {
      if ( selectRulesFile.empty() ) {
        throw runtime_error( "Selection rules missing" );
      }
      if ( verbose ) cout << "Creating curves selector: " << flush;
      if ( selectRoiMotion.empty() ) {
        bundleROISelect.reset
          ( new BundleROISelect( getRoiIterator( selectRoiFile ),
                                 selectRulesFile,
                                 selectROIMinimumOverlap / 100.0F ) );
      } else {
        Reader<AffineTransformation3d> reader( selectRoiMotion );
        AffineTransformation3d motion;
        reader.read( motion );
        bundleROISelect.reset
          ( new  BundleROISelect( getRoiIterator( selectRoiFile,
                                                  motion ),
                                  selectRulesFile ) );
      }
      finalProducer->addBundleListener( *bundleROISelect );
      finalProducer = bundleROISelect.get();
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< BundleROISplit > bundleROISplit;
    if ( splitRoiFile.size() ) {
      if ( verbose ) cout << "Creating curves splitter: " << flush;
      if ( splitRoiMotion.empty() ) {
        bundleROISplit.reset
          ( new BundleROISplit( getRoiIterator( splitRoiFile ),
                                splitKeepOriginalBundle ) );
      } else {
        MotionReader reader( splitRoiMotion );
        Motion motion;
        reader >> motion;
        bundleROISplit.reset
          ( new  BundleROISplit( getRoiIterator( splitRoiFile,
                                                 motion ),
                                 splitKeepOriginalBundle ) );
      }
      finalProducer->addBundleListener( *bundleROISplit );
      finalProducer = bundleROISplit.get();
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< CurveSelection > curveSelection;
    if ( minimumLength > 0 ) {
      if ( verbose ) cout << "Creating curve filter: " << flush;
      curveSelection.reset( new CurveSelection( minimumLength ) );
      finalProducer->addBundleListener( *curveSelection );
      finalProducer = curveSelection.get();
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< CurveOrientation > curveOrientation;
    if ( numberOfSlices ) {
      if ( verbose ) cout << "Creating curve orientation filter: " << flush;
      curveOrientation.reset( new CurveOrientation );
      finalProducer->addBundleListener( *curveOrientation );
      finalProducer = curveOrientation.get();
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< BundleWriter > bundleWriter;
    if ( bundlesOutputFile.size() ) {
      if ( verbose ) cout << "Creating bundle writer: " << flush;
      bundleWriter.reset( new BundleWriter() );
      bundleWriter->setFileString( bundlesOutputFile );
      finalProducer->addBundleListener( *bundleWriter );
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< BundleToGraphWriter > bundleToGraphWriter;
    if ( ! graphFile.empty() ) {
      if ( verbose ) cout << "Creating graph writer: " << flush;
      bundleToGraphWriter.reset( new BundleToGraphWriter );
      bundleToGraphWriter->setFileString( graphFile );
      finalProducer->addBundleListener( *bundleToGraphWriter );
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< BundlesSlicer > bundleSlicer;
    if ( numberOfSlices ) {
      if ( verbose ) cout << "Creating bundle slicer: " << flush;
      bundleSlicer.reset( new SimpleBundlesSlicer( numberOfSlices ) );
      finalProducer->addBundleListener( *bundleSlicer );
      if ( verbose ) cout << "done" << endl;
    }

    rc_ptr< BundlesFeatures > bundlesFeatures;
    rc_ptr< BundleSlicesToMesh > bundleSlicesToMesh;
    if ( ! statFileName.empty() ) {
      if ( verbose ) cout << "Creating bundle features: " << flush;
      bundlesFeatures.reset( new BundlesFeatures );
      for( vector< string >::const_iterator it = imageStatistics.begin();
           it != imageStatistics.end(); ++it ) {
        const int index = it->find( ":" );
        if ( index < 0 ) {
          throw runtime_error( string( "Invalid image statistics string: " ) +
                               *it );
        }
        const string name = it->substr( 0, index );
        const string fileName = it->substr( index+1, it->size() - index - 1 );
        bundlesFeatures->addImageStatistics( name, 
                                           getLinearInterpolator( fileName ) );
      }
      finalProducer->addBundleListener( *bundlesFeatures );
      if ( ! bundleSlicer.isNull() ) {
        bundleSlicer->addBundlesSlicerListener( *bundlesFeatures );
      }
      if ( verbose ) cout << "done" << endl;
    
      if ( ! slicesMesh.fileName().empty() ) {
        if ( verbose ) cout << "Creating slices to mesh filter: " 
                            << flush;
        bundleSlicesToMesh.reset( new BundleSlicesToMesh );
        bundleSlicer->addBundlesSlicerListener( *bundleSlicesToMesh );
        if ( verbose ) cout << "done" << endl;
      }
    }

    if ( verbose ) cout << "Analysing bundles: " << flush;
    bundleReader.read();
    if ( verbose ) cout << "done" << endl;

    if ( ! bundleSlicesToMesh.isNull() ) {
      if ( verbose ) cout << "Writing " <<  slicesMesh.fileName() << ": " 
			  << flush;
      slicesMesh.write( bundleSlicesToMesh->getMesh() );
      if ( verbose ) cout << "done" << endl;
    }

    if ( ! statFileName.empty() ) {
      if ( verbose ) cout << "Writing " << statFileName << ": " << flush;
      bundlesFeatures->write( statFileName );
      if ( verbose ) cout << "done" << endl;
    }
  }
  catch ( user_interruption &e ) {
  }
  catch ( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }

  return result;

}

