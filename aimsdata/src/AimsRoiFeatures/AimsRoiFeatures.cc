/* Copyright (c) 1995-2005 CEA
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

/*
 *  Apply threshold
 */
#include <cstdlib>
#include <aims/features/roi_features.h>
#include <cartobase/config/verbose.h>
#include <cartobase/stream/fileutil.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace std;
using namespace carto;


int main( int argc, const char **argv )
{
  try {
    string inputFileName;
    vector< string > imageStatistics;
     string outputFileName = "";
    
    AimsApplication	app( argc, argv, 
      "Compute scalar features on one or more images from regions of interest. Features computed are min, max, mean, standard deviation and median. The output is a Python dictionary containing one sub-dictionary per ROI. Each sub-dictionary contains one entry per feature (identified by the label of the feature).\n\nExamples:\n\n"
"1) To compute the statistics of region drawn in Anatomist on a single image:\n\n\tAimsRoiFeature -i regionsDrawnWithAnatomist.arg -s chaos_fa.ima chaos_vr.ima.\n\n"
"2) To compute the statistics of regions defined in an image of labels on several images:\n\n\tAimsRoiFeature -i imageOfLabels.ima -s first.ima second.ima.\n\n"
"3) To compute statistics on several images with the same filename:\n\nAimsRoiFeature -i imageOfLabels.ima -s first:first/image.ima second:second/image.ima.\n\n"
 );
    app.addOption( inputFileName, "-i",
                   "input ROI file (either ROI graph or image)" );
    app.addOptionSeries( imageStatistics, "-s",
      "Compute statistics from one or more images. Each argument of this "
      "parameter can be either an image or a graph. The file name can be prefixed with <label>: to specify the label that will be used to identify the image in the output (by default, the file name without extention is used)." );
    app.addOption( outputFileName, "-o",
                   "output feature file (default = standard output)", true );
    app.alias( "--imageStatistics", "-s" );
    app.alias( "--input", "-i" );
    app.alias( "--output", "-o" );
    app.initialize();
   
    RoiFeatures roiFeatures;
    for( vector< string >::const_iterator it = imageStatistics.begin();
         it != imageStatistics.end(); ++it ) {
      const int index = it->find( ":" );
      string label;
      if ( index < 0 ) {
        label = FileUtil::basename( *it );
        const int pointIndex = label.rfind( "." );
        if ( pointIndex >= 0 ) {
          label = label.substr( 0, pointIndex );
        }
        //throw runtime_error( string( "Invalid image statistics string: " ) +
        //                     *it );
      } else {
        label = it->substr( 0, index );
      }
      const string fileName = it->substr( index+1, it->size() - index - 1 );
      roiFeatures.addImageStatistics( label,
                                      getLinearInterpolator( fileName ) );
    }
    roiFeatures.computeFeatures( getRoiIterator( inputFileName ) );
    if ( outputFileName.empty() ) {
      roiFeatures.write( cout );
    } else {
      roiFeatures.write( outputFileName );
    }
    
    return EXIT_SUCCESS;
  }
  catch( user_interruption & ) {}
  catch( exception & e ) {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
