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

#include <aims/getopt/getopt2.h>
#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeutil.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <list>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{

  list< string > inFiles;
  string fileOut;
  list< short > inBackgrounds;
  short outBackground = -1;
  int f, d, x, y, z, t;

  AimsApplication app( argc, argv,
                       "Merge several label data into a new label data" );
  app.addOptionSeries( inFiles, "-i", "input label data filename list", 1 );
  app.addOptionSeries( inBackgrounds, "-b",
                       "background labels for label data (default=-1)" );
  app.addOption( fileOut, "-o", "output label data filename" );
  app.addOption( outBackground, "-bo",
                 "background label for output label data (default=-1)", 1 );

  try
  {

    app.initialize();

    if ( inFiles.size() == 1U )
    {

      cerr << "there should be at least 2 label data" << endl;
      throw;

    }

    if ( inBackgrounds.empty() )
    {

      for ( f = 0; f < ( int )inFiles.size(); f++ )
      {

        inBackgrounds.push_back( ( short )-1 );

      }

    }
    else
    {

      if ( inBackgrounds.size() != inFiles.size() )
      {

        cerr << "there should be as many background short(s) as file count"
             << endl;
        throw;

      }

    }

    Reader< Volume< short > > reader;
    VolumeRef< short > *data = new VolumeRef< short >[ inFiles.size() ];

    cout << "reading input label data : " << flush;
    list< string >::const_iterator f = inFiles.begin(),
                                   fe = inFiles.end();
    int dataCount = 0;
    while ( f != fe )
    {

      cout << *f << " " << flush;
      reader.setFileName( *f );
      data[ dataCount ].reset( reader.read() );
      f ++;
      dataCount ++;

    }
    cout << endl;

    cout << "checking geometry coherence between data" << flush;
    vector<int> dims = data[ 0 ] ->getSize();
    int dimX = data[ 0 ]->getSizeX();
    int dimY = data[ 0 ]->getSizeY();
    int dimZ = data[ 0 ]->getSizeZ();
    int dimT = data[ 0 ]->getSizeT();
    vector<float> vs = data[ 0 ]->getVoxelSize();
    for ( d = 1; d < dataCount; d++ )
    {

      if ( data[ d ]->getSize() != dims ||
           data[ d ]->getVoxelSize() != vs )
      {

        delete [] data;
        cerr << "geometrie(s) are not coherent between data"
             << endl;
        throw;

      }

    }
    cout << endl;

    cout << "initializing output structures" << flush;
    VolumeRef< short > merged( dims );
    merged->header().setProperty( "voxel_size", vs );
    merged->fill( outBackground );

    VolumeRef< ::byte > intersections( dims );
    intersections->header().setProperty( "voxel_size", vs );
    intersections->fill( 0 );
    cout << endl;

    // relabel the whole data set with unique label for each
    // component

    cout << "relabelling component" << flush;
    list< short >::const_iterator b = inBackgrounds.begin();
    short label = outBackground + 1;
    for ( d = 0; d < dataCount; d++ )
    {

      short background = *b;

      map< short, short > labelLut;

      for ( t = 0; t < dimT; t++ )
        for ( z = 0; z < dimZ; z++ )
          for ( y = 0; y < dimY; y++ )
            for ( x = 0; x < dimX; x++ )
            {

              short value = data[ d ]->at( x, y, z, t );
              if ( value != background )
              {

                if ( labelLut.find( value ) == labelLut.end() )
                {

                   labelLut[ value ] = label;
                   label ++;

                }
                data[ d ]->at( x, y, z, t ) = labelLut[ value ];

              }

            }

      b ++;

    }
    cout << endl;

    cout << "merging labels" << flush;
    int labelCount;
    short outputLabel = 0;
    for ( t = 0; t < dimT; t++ )
      for ( z = 0; z < dimZ; z++ )
        for ( y = 0; y < dimY; y++ )
          for ( x = 0; x < dimX; x++ )
          {

            b = inBackgrounds.begin();
            labelCount = 0;
            for ( d = 0; d < dataCount; d++ )
            {

              short background = *b;
              short value = data[ d ]->at( x, y, z, t );
              if ( value != background )
              {

                outputLabel = value;
                labelCount ++;

              }

            }
            if ( labelCount == 1 )
            {

              merged->at( x, y, z, t ) = outputLabel;

            }
            else if ( labelCount > 1 )
            {

              intersections->at( x, y, z, t ) = 1;

            }

          }
    cout << endl;


    cout << "saving merged label data" << flush;
    Writer< VolumeRef< short > > mergedWriter( fileOut );
    mergedWriter.write( merged );
    cout << endl;

    if ( carto::max( intersections ) )
    {

      cout << "saving intersection mask" << flush;
      string res = fileOut;
      string ext="";
      if ( res.length() > 4 )
        ext = res.substr( int(res.length() - 4), 4 );
      if (ext == ".dim" || ext == ".ima")
        res = res.substr( 0, res.length() - 4 );
      fileOut = res + "_intersections";
      Writer< VolumeRef< ::byte > > intersectionsWriter( fileOut );
      intersectionsWriter.write( intersections );
      cout << endl;

    }

  }
  catch( user_interruption &e )
  {

  }
  catch( std::exception &e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    return EXIT_FAILURE;

  }

  return EXIT_SUCCESS;

}


