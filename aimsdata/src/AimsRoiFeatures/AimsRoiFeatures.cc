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
#include <cctype>
#include <algorithm>
#include <locale>
#include <aims/features/roi_features.h>
#include <cartobase/config/verbose.h>
#include <cartobase/stream/fileutil.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/motionR.h>


using namespace aims;
using namespace std;
using namespace carto;


int main( int argc, const char **argv )
{
  try {
    string inputFileName;
    Reader< Motion > transformationReader( "" );
    vector< string > imageStatistics;
    string outputFileName = "";
    string outputFormat;
    string outputLocale;

    AimsApplication	app( argc, argv, 
      "Compute simple statistics on one or more images from regions of interest. Features computed are min, max, mean, standard deviation and median. The output can be in minf or csv format (see -f option below). In minf format, its a dictionary containing one sub-dictionary per ROI. Each sub-dictionary contains one entry per feature (identified by the label of the feature). In csv format it is a text file containing values separated by tabulation character and compatible with main spread sheet software such as OpenOffice and Excel.\n\nExamples:\n\n"
"1) To compute the statistics of region drawn in Anatomist on a single image:\n\n\tAimsRoiFeature -i regionsDrawnWithAnatomist.arg -s chaos_fa.ima chaos_vr.ima\n\n"
"2) To compute the statistics of regions defined in an image of labels on several images:\n\n\tAimsRoiFeature -i imageOfLabels.ima -s first.ima second.ima\n\n"
"3) To compute statistics on several images with the same filename:\n\nAimsRoiFeature -i imageOfLabels.ima -s first:first/image.ima second:second/image.ima\n\n"
"4) Same as 3) but save the result in csv format:\n\nAimsRoiFeature -i imageOfLabels.ima -s first:first/image.ima second:second/image.ima -f csv\n\n"
"5) Same as 4) but use french locale (i.e. comma for decimal point in numbers):\n\nAimsRoiFeature -i imageOfLabels.ima -s first:first/image.ima second:second/image.ima -f csv -l fr\n\n"
 );
    app.addOption( inputFileName, "-i",
                   "input ROI file (either ROI graph or image)" );
    app.addOptionSeries( imageStatistics, "-s",
      "Compute statistics from one or more images. Each argument of this "
      "parameter can be either an image or a graph. The file name can be prefixed with <label>: to specify the label that will be used to identify the image in the output (by default, the file name without extention is used)." );
    app.addOption( transformationReader, "-t",
                   "Transformation between ROI and images (default = identity)", true );
    app.addOption( outputFileName, "-o",
                   "output file name (default = standard output)", true );
    app.addOption( outputFormat, "-f",
                   "output file format. Possible formats are minf or csv. Default value: guessed according to file name extension, if the extension is unknown, the minf format is used.", true );
    app.addOption( outputLocale, "-l",
                   "Change the locale used for writing numbers in csv format. For instance, to use a csv file in a french version of Excel, one should use a french locale with \"-l fr\".", true );
    app.alias( "--imageStatistics", "-s" );
    app.alias( "--input", "-i" );
    app.alias( "--transformation", "-t" );
    app.alias( "--output", "-o" );
    app.alias( "--format", "-f" );
    app.alias( "--locale", "-l" );
    app.initialize();
   
    RoiFeatures roiFeatures;
    for( vector< string >::const_iterator it = imageStatistics.begin();
         it != imageStatistics.end(); ++it ) {
      const int index = it->find( ":" );
      string label;
      if ( index < 0 ) {
        if ( imageStatistics.size() > 1 ) {
          label = FileUtil::basename( *it );
          const int pointIndex = label.rfind( "." );
          if ( pointIndex >= 0 ) {
            label = label.substr( 0, pointIndex );
          }
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
    if ( transformationReader.fileName().empty() ) {
      roiFeatures.computeFeatures( getRoiIterator( inputFileName ) );
    } else {
      Motion transformation;
      transformationReader.read( transformation );
      roiFeatures.computeFeatures( getRoiIterator( inputFileName, transformation ) );
    }
    if ( outputFormat.empty() ) {
      if ( outputFileName.empty() ) {
        outputFormat = "minf";
      } else {
        outputFormat = "minf";
        if ( outputFileName.size() > 4 ) { 
          string ext = outputFileName.substr( outputFileName.size()-4, 4 );
          transform( ext.begin(), ext.end(), ext.begin(), ::tolower );
          if ( ext == ".csv" ) outputFormat = "csv";
        }
      }
    } else {
      transform( outputFormat.begin(), outputFormat.end(), outputFormat.begin(), ::tolower );
    }

    if ( outputLocale.empty() || outputFormat != "csv" ) {
      locale::global( locale::classic() );
      cout.imbue( locale::classic() );
    } else {
      locale myLocale = locale( outputLocale.c_str() );
      locale::global( myLocale );
      cout.imbue( myLocale );
    }

    if ( outputFileName.empty() ) {
      roiFeatures.write( cout, outputFormat );
    } else {
      ofstream out( outputFileName.c_str() );
      if ( ! out.good() ) {
        throw runtime_error( string( "Cannot open \"" ) + outputFileName + "\" for writing." );
      }
      roiFeatures.write( out, outputFormat );
    }
    
    return EXIT_SUCCESS;
  }
  catch( user_interruption & ) {}
  catch( exception & e ) {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
