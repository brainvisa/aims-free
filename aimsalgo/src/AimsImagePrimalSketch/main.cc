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
#include <aims/texdata/texdata.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/primalsketch/dericheSmoother_d.h>
#include <aims/primalsketch/primalSketch.h>
#include <aims/primalsketch/primalSketch2graph.h>
#include <aims/primalsketch/primalSketchUtil.h>

using namespace aims;
using namespace carto;
using namespace std;


int DoImagePrimalSketch ( std::string fileIn, 
                           std::string fileoutScales, 
                           std::string fileoutBlobs, 
                           std::string fileMask, 
                           std::string subject,
                           int smoothOperator,
                           float dt, 
                           float tmax, 
                           float tmin,
                           std::string statFile, 
                           std::string graphFile ) {
    
    AimsData<float> imageS;
    Reader<AimsData<float> > readerS( fileIn );
    AimsData<float> mask;
    Reader<AimsData<float> > readerM( fileMask );
    if ( subject == "none" )
        subject = fileIn;

    std::cout << "Reading input image " << fileIn << " for subject " << subject << endl;
    if( !readerS.read( imageS ) )
        return EXIT_FAILURE;
    // Checking for NaNs. spmT maps have NaNs outside the brain.
    // (for some reason !)
    // This screws up the smoothing. If NaNs are detected, they
    // are replaced with 0 and a new volume is written.
    int x, y, z, sx, sy, sz, nbN = 0;
    sx = imageS.dimX(); 
    sy = imageS.dimY(); 
    sz = imageS.dimZ();
    for ( z = 0; z < sz; z++ )
        for ( y = 0 ; y < sy ; y++ )
            for ( x = 0 ; x < sx ; x++ ) {
                if ( imageS(x,y,z) != imageS(x,y,z) ) { // test for a NaN
                    imageS(x,y,z)=0.0;
                    nbN++;
                }
            }
    if (nbN > 0) {
        std::cout << "Found some NaN value in the volume. This must be a spmT ;-)" << std::endl;
        std::cout << "Rewriting a clean volume" << std::endl;
        aims::Writer<AimsData<float> > writeNoNaN( fileIn );
        writeNoNaN.write(imageS);
    }

    std::cout << "Reading mask " << fileMask << std::endl;
    if( !readerM.read( mask ) )
        return EXIT_FAILURE;

    TexturedData< AimsData<float>, AimsData<float> > maskData( &mask );

    std::cout << "Creating scale-space with geometric sampling of the scales (t=2^n) up to " << tmax << std::endl;

    std::cout << "Creating smoother" << std::endl;

    DiffusionSmoother<float> *diffSmooth;
    diffSmooth = new DiffusionSmoother<float>( dt );

    DericheSmoother<float> *gaussSmooth;
    gaussSmooth = new DericheSmoother<float>();

    Smoother<carto::VolumeRef<float>, carto::VolumeRef<float> > *smooth;

    if ( smoothOperator == 0 )
        smooth = gaussSmooth;
    else if ( smoothOperator == 1 )
        smooth = diffSmooth;
    else {
        std::cerr << "Wrong value for -s option" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Scale-space computation" << std::endl;

    aims::ScaleSpace<AimsData<float>, AimsData<float> > scale_space( &imageS, smooth );

    scale_space.GenerateDefaultScaleSpace(tmax);

    std::cout << "Ecriture du scale-space dans le fichier " << fileoutScales << std::endl;

    scale_space.Write(fileoutScales);

    std::cout << "Computing primal sketch" << std::endl;

    aims::PrimalSketch<AimsData<float>, AimsData<float> > sketch( subject, &scale_space, &maskData, IMAGE );

    sketch.ComputePrimalSketch(tmin, tmax, statFile);

    std::cout << "Converting primal sketch into graph" << std::endl;

    Primalsketch2graph<AimsData<float>, AimsData<float> > translate ( &sketch );

    translate.DoIt();

    Graph *graphSketch = translate.GetGraph();

    if ( graphFile == "" )
        graphFile = fileIn + "_graphSketch.arg";

    aims::Writer<Graph> dataW ( graphFile );
    std::cerr << "Writing file " << graphFile << std::endl;
    dataW.write( *graphSketch );

    std::cout << "Getting scale-space blob image" << std::endl;
    AimsData<short> *blobImage;
    blobImage = GetSSBlobImage ( &sketch );

    std::cout << "Writing it in file " << fileoutBlobs << std::endl;
    aims::Writer<AimsData<short> > blobW ( fileoutBlobs );
    blobW.write ( *blobImage );

    std::cout << "Getting scale-space blob buckets" << std::endl;
    AimsBucket<Void> *blobBck;
    blobBck = GetSSBlobBucket( &sketch );

    std::cout << "Writing it in file " << fileoutBlobs << std::endl;
    aims::Writer<AimsBucket<Void> > bckW ( fileoutBlobs );
    bckW.write ( *blobBck );

    std::cout << "Finished" << std::endl;

    return EXIT_SUCCESS;
}




int main(int argc, const char **argv) {
    try {
        std::string fileIn, fileoutScales, fileoutBlobs, fileMask, subject;
        std::string scaleSpacePath;
        int smoothOperator = 0;
        float dt = 0.1, tmax, tmin = 1.0;
        std::string statFile, graphFile;
//        bool recover = false;

        subject = "none";

        AimsApplication     app( argc, argv, "ScaleSpace d'une image au format FLOAT");

        app.addOption( fileIn, "-i", "inputImage");
        app.alias( "--input", "-i" );
        app.addOption( fileMask, "-m", "mask");
        app.alias( "--mask", "-m" );
        app.addOption( fileoutScales, "-os", "output scalespace filename");
        app.alias( "--output-scales", "-os" );
        app.addOption( fileoutBlobs, "-ob", "output blobs filename");
        app.alias( "--output-blobs", "-ob" );
        app.addOption( tmin, "-t1", "minimum scale (default=1.0)", true);
        app.alias( "--scalemin", "-t1" );
        app.addOption( tmax, "-t2", "maximum scale");
        app.alias( "--scalemax", "-t2" );
        app.addOption( smoothOperator, "-s", "smoother (0 : Gaussian, 1 : diffusion; default=0)", true);
        app.alias( "--smoother", "-s" );
        app.addOption( dt, "-dt", "time step (for diffusion only; default=0.1)", true);
        app.alias( "--deltat", "-dt" );
        app.addOption( statFile, "-f", "name of file containing statistics for blob measurements normalisation", true );
        app.alias ("--stats", "-f");
        app.addOption( subject, "-sj", "subject name (default : inputImage)", true);
        app.alias ("--subject", "-sj");
//        app.addOption( recover, "--recover", "recover", 1.0 );
//        app.addOption( scaleSpacePath, "--ss", "scale space path", 1.0 );

        app.addOption( graphFile, "-og", "primal sketch graph", true);
        app.alias ("--graph", "-og");
        app.initialize();
        
        
        return DoImagePrimalSketch( fileIn, fileoutScales, fileoutBlobs, fileMask, subject, smoothOperator, dt, tmax, tmin, statFile, graphFile );
        
    }   
    catch( user_interruption & )
    {
    }
    catch( exception & e )
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
