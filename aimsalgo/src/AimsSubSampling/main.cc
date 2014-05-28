/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <iomanip>
#include <cartobase/config/verbose.h>
#include <cartobase/smart/rcptr.h>
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/data/data.h>
#include <aims/io/writer.h>
#include <aims/io/reader.h>
#include <aims/utility/utility_g.h>
#include <aims/utility/progress.h>
#include <aims/math/math_g.h>
#include <aims/resampling/subsamplingimagealgorithm.h>
#include <aims/resampling/extremadifferencesubsampling.h>
#include <aims/resampling/mediansubsampling.h>
#include <aims/resampling/meansubsampling.h>
#include <aims/resampling/minsubsampling.h>
#include <aims/resampling/maxsubsampling.h>
#include <aims/resampling/majoritysubsampling.h>

using namespace std;
using namespace carto;
using namespace aims;

// Processing class, with template operation function
template<class T>
static bool doit( Process &, const string &, Finder & );

class SubSampling : public Process {
    public:
        SubSampling( const string & fout, unsigned rx, unsigned ry, unsigned rz, unsigned type );
        
        template<class T>
        friend bool doit( Process &, const string &, Finder & );
        
        unsigned getRX() const { return _rx; }
        unsigned getRY() const { return _ry; }
        unsigned getRZ() const { return _rz; }
        unsigned getType() const { return _type; }

    private:
        string _fout;
        unsigned _rx;
        unsigned _ry;
        unsigned _rz;
        unsigned _type;
};

SubSampling::SubSampling( const string & fout,
                          unsigned rx,
                          unsigned ry,
                          unsigned rz,
                          unsigned type )
  : Process(),
    _fout( fout ),
    _rx(rx),
    _ry(ry),
    _rz(rz),
    _type(type)
{
    registerProcessType( "Volume", "S8", &doit<int8_t> );
    registerProcessType( "Volume", "U8", &doit<uint8_t> );
    registerProcessType( "Volume", "S16", &doit<int16_t> );
    registerProcessType( "Volume", "U16", &doit<uint16_t> );
    registerProcessType( "Volume", "S32", &doit<int32_t> );
    registerProcessType( "Volume", "U32", &doit<uint32_t> );
    registerProcessType( "Volume", "FLOAT", &doit<float> );
    registerProcessType( "Volume", "DOUBLE", &doit<double> );
    registerProcessType( "Volume", "RGB", &doit<AimsRGB> );
    registerProcessType( "Volume", "RGBA", &doit<AimsRGBA> );
}

template <class VoxelType>
const rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > getSubsamplingImageAlgorithm( const unsigned type,
                                                                                        const unsigned win_size_x,
                                                                                        const unsigned win_size_y,
                                                                                        const unsigned win_size_z ) {
  switch(type) {
    
    case 0:
      return rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > (
                new MedianSubSampling<VoxelType>( win_size_x, 
                                                  win_size_y,
                                                  win_size_z )
             );
      break;
      
    //Mean computation
    case 1:
      return rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > (
                new MeanSubSampling<VoxelType>( win_size_x, 
                                                win_size_y,
                                                win_size_z )
             );
    break;
    
    // Min computation
    case 2:
      return rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > (
                new MinSubSampling<VoxelType>( win_size_x, 
                                               win_size_y,
                                               win_size_z )
             );
    break;
    
    // Max computation
    case 3:
      return rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > (
                new MaxSubSampling<VoxelType>( win_size_x, 
                                               win_size_y,
                                               win_size_z )
             );
    break;
    
    // Majority computation
    case 4:
      return rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > (
                new MajoritySubSampling<VoxelType>( win_size_x, 
                                                    win_size_y,
                                                    win_size_z )
             );
    break;
    
    // Difference computation
    case 5:
      return rc_ptr<aims::ImageAlgorithmInterface< VoxelType > > (
                new ExtremaDifferenceSubSampling<VoxelType>( win_size_x, 
                                                             win_size_y,
                                                             win_size_z )
             );
    break;
    
    // Median computation
    default:
      throw std::invalid_argument( "Invalid subsampling type: " + carto::toString(type) );
      break;
  }
}

template<class T> bool 
doit( Process & p, const string & fname, Finder & f ) {

    SubSampling & sp = (SubSampling &) p;
    AimsData<T> dataIn;
    string format = f.format();
    Reader<AimsData<T> > r( fname );
    
    cout << "Reading volume...";
    if( !r.read( dataIn, 0, &format ) )
        return( false );
    cout << " done" << endl;
    
    if (verbose) {
      cout << "Subsampling size: rx = " << sp._rx << "\t"
                                "ry = " << sp._ry << "\t"
                                "rz = " << sp._rz << endl 
           << "Subsampling type: " << sp._type << endl;
    }
    
    const rc_ptr<aims::ImageAlgorithmInterface< T > > & algo = getSubsamplingImageAlgorithm<T>( sp._type, 
                                                                                                sp._rx,
                                                                                                sp._ry,
                                                                                                sp._rz );
    AimsData<T> dataOut = algo->execute(dataIn);
    
    // File Writing
    cout << "Writing volume...";
    Writer<AimsData<T> > writer( sp._fout );
    int result = writer.write( dataOut );
    cout << " done" << endl;
    return result;
}

int main( int argc, const char **argv )
{
    int result = EXIT_SUCCESS;
    try {
        // Default values
        int32_t rx = 1, ry = 1, rz = 1, rxy = 0;
        string filein = "", fileout = "", type = "0";

        // Collect arguments
        AimsApplication application( argc, argv, "Image subsampling" );
        application.addOption( filein, "-i", "Input image to subsample" );
        application.addOption( rx, "-rx", "X number of voxels to aggregate [default = 1]", true );
        application.addOption( ry, "-ry", "Y number of voxels to aggregate [default = 1]", true );
        application.addOption( rz, "-rz", "Z number of voxels to aggregate [default = 1]", true );
        application.addOption( rxy, "-n", "Number of voxels to aggregate in X and Y directions [default = 2]"
                                         "(this option is obsolete and only provided for compatibility purpose)", true);
        application.addOption( type, "-t", "Subsampling type : med[ian], mea[n], min[imum], max[imum], \n"
                                           "maj[ority], dif[ference] [default = median]. Modes may also\n"
                                           "be specified as order number: 0=median, 1=mean, etc.", true);
        application.addOption( fileout, "-o", "Output subsampled image" );
        
        application.alias( "--input", "-i" );
        application.alias( "--output", "-o" );
        application.initialize();
        
        //  Test parameters
        //
        if (rx < 1) rx = 1;
        if (ry < 1) ry = 1;
        if (rz < 1) rz = 1;
        if (rxy > 0) {
          rx = ry = rxy;
        }

        // Subsampling types map
        map<string, unsigned>  types;
        types[ "med" ] = 0;
        types[ "median" ] = 0;
        types[ "0" ] = 0;
        types[ "mea" ] = 1;
        types[ "mean" ] = 1;
        types[ "1" ] = 1;
        types[ "min" ] = 2;
        types[ "minimum" ] = 2;
        types[ "2" ] = 2;
        types[ "max" ] = 3;
        types[ "maximum" ] = 3;
        types[ "3" ] = 3;
        types[ "maj" ] = 4;
        types[ "majority" ] = 4;
        types[ "4" ] = 4;
        types[ "dif" ] = 5;
        types[ "difference" ] = 5;
        types[ "5" ] = 5;

        map<string, unsigned>::iterator it = types.find( type );
          
        if( it == types.end() )
          throw invalid_argument( "Invalid subsampling type: " + type );

        SubSampling proc( fileout,
                          (unsigned)rx,
                          (unsigned)ry,
                          (unsigned)rz,
                          it->second );
        if( !proc.execute( filein ) )
        {
          cerr << "Could not process\n";
          return( 1 );
        }
    }
    catch( user_interruption &e ) {}
    catch( std::exception &e )
    {
        cerr << argv[ 0 ] << ": " << e.what() << endl;
        result = EXIT_FAILURE;
    }
    return result;
}
