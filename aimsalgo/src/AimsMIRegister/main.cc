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
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
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

// CM' = R.CM + T
// OM' = (Id - R).OC + T + R.OM
//        terme NEWTRANS

// ============================================================
// Declaration
// ============================================================
const float REF_RATIO = 0.05;
const float TEST_RATIO = 0.1;

// ------------------------------------------------------------
// RegistrationProcessParams
// ------------------------------------------------------------
struct RegistrationProcessParams {
  string interpolator        = "linear";
  string optimizer           = "powell";
  string pdf                 = "pv";
  string gc                  = "yes";
  string objfunc             = "mi";
  int    numlevel            = 64;
  int    maskSize            = 5;
  float  error               = 0.01;
  int    sn                  = 0;
  int    energysize          = 0;
  float  testratio           = TEST_RATIO;
  float  refratio            = REF_RATIO;
  bool   generatecurve       = true;
  int    axis2D              = -1;
  float  rx                  = 0.0;
  float  ry                  = 0.0;
  float  rz                  = 0.0;
  float  tx                  = 0.0;
  float  ty                  = 0.0;
  float  tz                  = 0.0;
  float  drx                 = 0.0;
  float  dry                 = 0.0;
  float  drz                 = 0.0;
  float  dtx                 = 0.0;
  float  dty                 = 0.0;
  float  dtz                 = 0.0;
  int rsp = 0, rep = 0, tsp = 0, tep = 0;
  // Default is the norm channel
  int refchannel = -1, testchannel = -1;
  string filetest, fileref;
  string fileLog;
  Writer<aims::AffineTransformation3d> direct, inverse;
  Finder testFinder;
};

// ------------------------------------------------------------
// RegistrationProcess<S>
// ------------------------------------------------------------
template <int S>
class RegistrationProcess { 
public:
  // ------------------------------------------------------------
  // Public type definitions
  // ------------------------------------------------------------
  typedef short VoxelType;
  typedef VolumeRef<typename RegistrationProcess::VoxelType> VolumeRefType;
  typedef ChannelReader<typename RegistrationProcess::VolumeRefType> ChannelReaderType;
  typedef Sampler<typename RegistrationProcess::VoxelType> SamplerType;
  typedef Resampler<typename RegistrationProcess::VoxelType> ResamplerType;
  typedef Pyramid<typename RegistrationProcess::VoxelType> PyramidType;
  typedef PyramidFunc<typename RegistrationProcess::VoxelType> PyramidFuncType;

  typedef float ParameterType;
  typedef AimsVector<typename RegistrationProcess::ParameterType, S> ParametersType;
  typedef ObjFunc<typename RegistrationProcess::VoxelType, 
                  typename RegistrationProcess::ParameterType, 
                  S> ObjFuncType;
  typedef Optimizer<typename RegistrationProcess::ParameterType, S> OptimizerType;

  // ------------------------------------------------------------
  // Public methods
  // ------------------------------------------------------------
  RegistrationProcess(RegistrationProcessParams & procparams);
  bool doit();

protected:
  // ------------------------------------------------------------
  // Protected methods
  // ------------------------------------------------------------
  void initializeInterpolation();
  void initializeObjectiveFunc();
  void initializeAxis();
  void initializeOptimizer();
  Point3df getGravityCenter(
            const VolumeRefType & volume, 
            const float threshold_ratio) const;
  ParametersType inferDeltas(const VolumeRefType & data) const;
  void print(const ParametersType & params) const;
  void write(const ParametersType & params);
  
  // ------------------------------------------------------------
  // Protected attributes
  // ------------------------------------------------------------
  RegistrationProcessParams procparams;
  ParametersType paramsInit;
  ParametersType deltasInit;
  ObjFuncType * objfunc = NULL;
  OptimizerType * optimizer = NULL;
  SamplerType * comb = NULL;
  ResamplerType * interpolator = NULL;
  bool firstWrite = true;
};

// ============================================================
// Implementation
// ============================================================

// ------------------------------------------------------------
// RegistrationProcess<S>
// ------------------------------------------------------------
template <int S>
void RegistrationProcess<S>::initializeInterpolation() {
  ASSERT(procparams.pdf == "direct" ||
         procparams.pdf == "pv");
  if (procparams.pdf == "direct") {
    if (!procparams.interpolator.empty()) {
      ASSERT(procparams.interpolator == "nearest" ||
             procparams.interpolator == "linear"  ||
             procparams.interpolator == "spline");
      if (procparams.interpolator == "nearest") {
        interpolator = new NearestNeighborResampler<VoxelType>;
        cout << "interpolator : " << procparams.interpolator << endl;
      }
      else if (procparams.interpolator == "linear") {
        interpolator = new LinearResampler<VoxelType>;
        cout << "interpolator    : " << procparams.interpolator << endl;
      }
      else if (procparams.interpolator == "spline") {
        interpolator = new CubicResampler<VoxelType>;
        cout << "interpolator     : " << procparams.interpolator << endl;
      }
    }
  }
  else {
    comb = new SamplerType;
  }
}

template <int S>
void RegistrationProcess<S>::initializeObjectiveFunc() { 
  objfunc = NULL;
  ASSERT(procparams.objfunc == "mi" ||
         procparams.objfunc == "cr");
  if (procparams.objfunc == "mi") {
    objfunc = new MutualInfoFunc<VoxelType, ParameterType, S>(
                  procparams.numlevel, 
                  interpolator, 
                  comb, 
                  procparams.maskSize);
    cout << "Objective function   : mutual information" << endl;
  }
  else if (procparams.objfunc == "cr") {
    objfunc = new CorRatioFunc<VoxelType, ParameterType, S>(
                  procparams.numlevel, 
                  interpolator, 
                  comb,
                  procparams.maskSize);
    cout << "Objective function   : correlation ratio" << endl;
  }
  else ASSERT(false);
}

template <int S>
void RegistrationProcess<S>::initializeAxis() {
  // Default does nothing, should be specialized when necessary
} 

template <int S>
void RegistrationProcess<S>::initializeOptimizer() {
  optimizer = NULL;
  ASSERT(procparams.optimizer == "powell" ||
         procparams.optimizer == "random" ||
         procparams.optimizer == "single");

  if( procparams.optimizer == "powell" ) {
    optimizer = new PowellOptimizer<ParameterType, S>(
                                        *objfunc,
                                        procparams.error);
    cout << "Optimizer            : powell" << endl;
  }
  else if( procparams.optimizer == "random" ) {
    optimizer = new DetermOptimizer<ParameterType, S>(
                                        *objfunc, 
                                        procparams.error, 
                                        10000, 
                                        100, 
                                        true);
    cout << "Optimizer            : deterministic" << endl;
  }
  else if( procparams.optimizer == "single" ) {
    optimizer = new SingleOptimizer<ParameterType, S>(
                                        *objfunc);
    cout << "Optimizer            : singleoptimizer" << endl;
  }
  else ASSERT(false);
}

template <int S>
Point3df RegistrationProcess<S>::getGravityCenter(
                          const VolumeRefType & volume, 
                          const float threshold_ratio) const {
  AimsThreshold<VoxelType, VoxelType> thresh(
                    AIMS_GREATER_THAN,
                    (VoxelType)(threshold_ratio * volume.max()), 
                    0);
  Volume<VoxelType> *tmp = new Volume<VoxelType>;
  *tmp = thresh.bin(volume);

  Point3df gravity_center(0., 0., 0.);
  uint64_t gn = 0;
  for(uint64_t z = 0; z < tmp->getSizeZ(); ++z)
    for(uint64_t y = 0; y < tmp->getSizeY(); ++y)
      for(uint64_t x = 0; x < tmp->getSizeX(); ++x) {
        if ((*tmp)(x, y, z)) {
          gravity_center[0] += x; 
          gravity_center[1] += y;
          gravity_center[2] += z;
          gn++;
        }
      }
  delete tmp;

  gravity_center[0] = (gravity_center[0] / gn) 
                      * volume.getVoxelSize()[0];
  gravity_center[1] = (gravity_center[1] / gn) 
                      * volume.getVoxelSize()[1];
  gravity_center[2] = (gravity_center[2] / gn) 
                      * volume.getVoxelSize()[2];
  
  return gravity_center;
}

template<int S>
inline
void RegistrationProcess<S>::print(const ParametersType & params) const {
  Point3df translation = objfunc->getTranslation(params);
  Point3df rotation = objfunc->getRotation(params);
  std::cout << "  - Tx: " << carto::toString(translation[0]) << std::endl
            << "  - Ty: " << carto::toString(translation[1]) << std::endl
            << "  - Tz: " << carto::toString(translation[2]) << std::endl
            << "  - Rx: " << carto::toString(rotation[0]) << std::endl
            << "  - Ry: " << carto::toString(rotation[1]) << std::endl
            << "  - Rz: " << carto::toString(rotation[2]) << std::endl;
}

template <int S>
inline
void RegistrationProcess<S>::write(const ParametersType & params) {
  // Infer deplacement from params and gravity centers stored in objfunc
  AffineTransformation3d transformation = objfunc->getTransformation(params);

  string curvedir = "";
  if (!procparams.direct.fileName().empty()) {
    cout << "Write direct transformation file: " 
         << procparams.direct.fileName() 
         << endl;
    procparams.direct.write(transformation);

    curvedir = FileUtil::dirname(procparams.direct.fileName());
  }

  if (!procparams.inverse.fileName().empty()) {
    unique_ptr<AffineTransformation3d> inverse = transformation.inverse();
    cout << "Write inverse transformation file: "
         << procparams.inverse.fileName() 
         << endl;
    procparams.inverse.write(*inverse);

    if (curvedir.empty()) {
      curvedir = FileUtil::dirname(procparams.inverse.fileName());
    } 
  }

  if (procparams.generatecurve && !curvedir.empty()) {
    // Write current parameters for plotting
    string curve_path = curvedir + FileUtil::separator() + "curve" ;
    ofstream curve(curve_path.c_str(), (firstWrite ? ios::app : ios::out));

    if (firstWrite) {
      firstWrite = false;
      curve << "  Tx  " << "\t" << "  Ty  " << "\t" << "  Tz  " 
            << "  Rx  " << "\t" << "  Ry  " << "\t" << "  Rz  " << endl;
    }
    
    Point3df rotation = objfunc->getRotation(params);
    curve << transformation.translation().item(0) << "\t" 
          << transformation.translation().item(1) << "\t"
          << transformation.translation().item(2) << "\t"
          << rotation[0] << "\t" << rotation[1] << "\t" << rotation[2] <<endl;
    curve.close();
  }
}

template <int S>
bool RegistrationProcess<S>::doit() {
    ofstream                     mycout;
    int  rspLevel                       = 0;
    int  repLevel                       = 0;
    int  tspLevel                       = 0;
    int  tepLevel                       = 0;
    PyramidType             *pyramidref = NULL,
                           *pyramidtest = NULL;
    PyramidFuncType        *pyramidfunc = NULL;
    ResamplerType         *interpolator = NULL;
    SamplerType                   *comb = NULL;
    ChannelReaderType             rdref(procparams.fileref),
                                 rdtest(procparams.filetest);
    VolumeRefType                  test,
                                    ref;
    Point3df                         gc,
                                keep_gc;

    if (procparams.testchannel == -1) {
      procparams.testchannel = procparams.refchannel;
    }
    
    // Reads reference image
    cout << "Reading channel " << carto::toString(procparams.refchannel) 
         << " of reference image" << endl << flush;
    map<std::string, set<string> > allowedtypes;
    set<string> & voltypes = allowedtypes["Volume"];
    voltypes.insert( "S8" );
    voltypes.insert( "U8" );
    voltypes.insert( "S16" );
    voltypes.insert( "U16" );
    voltypes.insert( "S32" );
    voltypes.insert( "U32" );
    voltypes.insert( "FLOAT" );
    voltypes.insert( "DOUBLE" );
    voltypes.insert( "RGB" );
    voltypes.insert( "RGBA" );
    voltypes.insert( "HSV" );
    rdref.setAllowedInputDataTypes( allowedtypes );
    rdref.read(ref, procparams.refchannel, 0, 0, 0 );

    // 
    // Log file management
    // ----------------------------------------------
    if(!procparams.fileLog.empty()) {
      mycout.open(procparams.fileLog.c_str());
    }

    //
    // Choice of interpolation
    // ----------------------------------------------
    initializeInterpolation();

    //
    // Choice of index to be minimized
    // ----------------------------------------------
    initializeObjectiveFunc();

    //
    // Initialize axis for 2D cases
    // ----------------------------------------------
    initializeAxis();

    //
    // Choice of optimizer
    // ----------------------------------------------
    initializeOptimizer();

    //
    // Get gravity centers of the ref image
    // ----------------------------------------------
    gc = getGravityCenter(ref, procparams.refratio);
    objfunc->setGcRef(gc[0] , gc[1], gc[2]);
    cout << "Gravity center (mm) of reference data: [" 
         << carto::toString(gc[0]) << ", "
         << carto::toString(gc[1]) << ", "
         << carto::toString(gc[2]) << "]" << endl;
    
    if (string(procparams.gc) != "yes") {
        keep_gc = gc;
    }

    //
    // Infer pyramid range from the following heuristics
    // ----------------------------------------------
    rspLevel = (procparams.rsp ? procparams.rsp : rspLevel);
    repLevel = (procparams.rep ? procparams.rep : rspLevel);
    // Only one scale level for reference
    ASSERT(repLevel == rspLevel);
    tspLevel = (procparams.tsp ? procparams.tsp : tspLevel);
    tepLevel = (procparams.tep ? procparams.tep : tspLevel);
    ASSERT(tepLevel <= tspLevel);

    //
    // State of input data.
    // ----------------------------------------------
    cout << "Reference image          : " << procparams.fileref << endl;
    cout << "Reference pyramid range  : " << rspLevel << " - " << repLevel << endl;
    if (procparams.sn > 0) {
      cout << "Frame selected from serie: " << procparams.sn << endl;
    }
    cout << "Test image               : " << procparams.filetest << endl;
    cout << "Test pyramid range       : " << tspLevel << " - " << tepLevel << endl;
    cout << "Number of Level          : " << procparams.numlevel << endl;
    cout << "Parzen Mask Size         : " << procparams.maskSize << endl;
    cout << "Error (Stop Criterion)   : " << procparams.error << endl;
    cout << "Reference threshold ratio: " << procparams.refratio << endl;
    cout << "Test threshold ratio     : " << procparams.testratio << endl;
    cout << "Initial parameter        : " << paramsInit << endl;
    cout << "Parameter variations     : " << deltasInit << endl;

    AimsVector<bool, 3> axes(true, true, true);
    if (procparams.axis2D != -1)
      axes[procparams.axis2D] = false;

    //  
    // Start the job.
    //     1. Init ObjectiveFunction ref images.
    //     2. Loop on time dimension (with init test image).
    // ----------------------------------------------
    pyramidfunc = new MedianPyramidFunc<VoxelType>();
    //   pyramidfunc = new MeanPyramidFunc<VoxelType>();
    pyramidref = new PyramidType(*pyramidfunc);
    pyramidref->setAxes(axes);
    pyramidref->setRef(ref);
    pyramidref->setLevel(rspLevel);

    // objfunc->setRef takes the address of an VolumeRef - it has to exist
    // while objfunc exists. pyramidref->item() gives a rc_ptr<Volume>.
    VolumeRefType levelref = pyramidref->item(rspLevel);
    objfunc->setRef(levelref);

    //
    // Get time frames
    // ----------------------------------------------
    std::vector<int> v;
    int sizeT = 1;
    const PythonHeader *ph = dynamic_cast<const PythonHeader *>( procparams.testFinder.header() );
    ph->getProperty("volume_dimension", v);
    if( v.size() >= 4 )
      sizeT = v[3];

    // Loop on time frames
    // ----------------------------------------------
    ParametersType params, optimized, deltas;

    for (int i=0; i<sizeT; i++)
    {
      //
      // Direct Frame reading
      // ----------------------------------------------
      string f = procparams.testFinder.format();
      rdtest.read(test, procparams.testchannel, 0, &f , i);

      //
      // Find and Set Gravity Center of test Volume
      // ----------------------------------------------
      if (string(procparams.gc) != "yes") {
        gc = keep_gc;
      }
      else {
        gc = getGravityCenter(test, procparams.testratio);
      }
      objfunc->setGcTest(gc[0], gc[1], gc[2]);
      cout << "Gravity center (mm) of test data frame " 
           << carto::toString(i) << ": [" 
           << carto::toString(gc[0]) << ", "
           << carto::toString(gc[1]) << ", "
           << carto::toString(gc[2]) << "]" << endl;
      
      //
      // Reset parameters and deltas
      // ----------------------------------------------
      params = paramsInit;
      deltas = deltasInit;

      //
      // Infer deltas from the following heuristics
      // ----------------------------------------------
      if (deltas == float(0.0) ) {
        deltas = inferDeltas(test);
      }

      //
      // Coarse to fine
      // ----------------------------------------------
      if (pyramidtest) 
        delete (pyramidtest);
      pyramidtest = new Pyramid<VoxelType>(*pyramidfunc);
      pyramidtest->setRef(test);
      pyramidtest->setLevel(tspLevel);
      deltas *= float(pow((double)2, 
                          (double)tspLevel));

      for (int level=tspLevel; level>=tepLevel; level--) {
        // objfunc->setTest takes the address of an VolumeRef - it has to exist
        // while objfunc exists. pyramidtest->item() gives a rc_ptr<Volume>.
        VolumeRefType leveltest = pyramidtest->item(level);
        objfunc->setTest(leveltest);
        cout << "Processing pyramid level "
             << carto::toString(level) << endl;
        cout << "Level " << carto::toString(level) 
                         << " parameters: " 
                         << carto::toString(params) 
                         << endl;
        cout << "Level " << carto::toString(level) 
                         << " deltas: " 
                         << carto::toString(deltas) 
                         << endl << flush;

        clock_t start = clock();  
        optimized = optimizer->doit(params, deltas);
        clock_t finish = clock();

        cout << "Level " << carto::toString(level) 
                         << " optimized parameters: " 
                         << carto::toString(optimized) 
                         << endl;
        cout << "Level " << carto::toString(level) 
             << " optimization duration: "
             << carto::toString(finish - start) 
             << " ticks" << endl << flush;

        deltas /= 2.0;
        params = optimized;
      }

      //
      // dump results
      // ----------------------------------------------
      cout << "Frame " << carto::toString(i) << endl;
      print(params);
      cout << endl;
      
      string transfo_path = FileUtil::dirname(procparams.filetest);
      string from = FileUtil::removeExtension(FileUtil::basename(procparams.filetest));
      string to = FileUtil::removeExtension(FileUtil::basename(procparams.fileref));
      
      // if( procparams.direct.fileName().empty() ) {
      //   // Get default file name
      //   procparams.direct.setFileName(transfo_path + FileUtil::separator() + from + "_TO_" + to + ".trm");
      // }
        
      // if ( procparams.inverse.fileName().empty() ) {
      //   // Get default file name
      //   procparams.inverse.setFileName(transfo_path + FileUtil::separator() + to + "_TO_" + from + ".trm");
      // }

      this->write(params);
    } // End loop on frame

    if (!procparams.fileLog.empty()) {
      mycout.close( );
    }

    return true;
}

// ------------------------------------------------------------
// RegistrationProcessParams<3>
// ------------------------------------------------------------
// Specialization for rigid 2D case (i.e. 3 parameters)
// ------------------------------------------------------------
template<>
inline
RegistrationProcess<3>::RegistrationProcess(RegistrationProcessParams & procparams)
  : procparams(procparams), 
    paramsInit(0.0), 
    deltasInit(0.0),
    firstWrite(true) {

    paramsInit[0] = procparams.tx;
    paramsInit[1] = procparams.ty;
    paramsInit[2] = procparams.rz;

    deltasInit[0] = procparams.dtx;
    deltasInit[1] = procparams.dty;
    deltasInit[2] = procparams.drz;
}

template <>
inline void RegistrationProcess<3>::initializeAxis() {
  // Only set axis for 2D case
  if (procparams.axis2D != -1)
    objfunc->setAxis(procparams.axis2D);
}

template<>
inline
RegistrationProcess<3>::ParametersType
RegistrationProcess<3>::inferDeltas(const VolumeRefType & data) const {

  ParametersType deltas;
  deltas[0] = data.getVoxelSize()[0] / 2.0;
  deltas[1] = data.getVoxelSize()[1] / 2.0;
  deltas[2] = 180.0 / M_PI / data.getSizeZ();

  // Delta in degrees around Z axis
  // is initialized using the number of voxels 
  // in diagonal of the (X, Y) plan
  // deltas[2] = 90.0 / (sqrt(pow(data.getSizeX(), 2) 
  //                        + pow(data.getSizeY(), 2)));
  return deltas;
}


// ------------------------------------------------------------
// RegistrationProcessParams<6>
// ------------------------------------------------------------
// Specialization for rigid 3D case (i.e. 6 parameters)
// ------------------------------------------------------------
template<>
inline
RegistrationProcess<6>::RegistrationProcess(RegistrationProcessParams & procparams)
  : procparams(procparams), 
    paramsInit(0.0), 
    deltasInit(0.0),
    firstWrite(true) {
    
    paramsInit[0] = procparams.tx;
    paramsInit[1] = procparams.ty;
    paramsInit[2] = procparams.rz;
    paramsInit[3] = procparams.tz;
    paramsInit[4] = procparams.rx;
    paramsInit[5] = procparams.ry;

    deltasInit[0] = procparams.dtx;
    deltasInit[1] = procparams.dty;
    deltasInit[2] = procparams.drz;
    deltasInit[3] = procparams.dtz;
    deltasInit[4] = procparams.drx;
    deltasInit[5] = procparams.dry;
}

template<>
inline
RegistrationProcess<6>::ParametersType 
RegistrationProcess<6>::inferDeltas(const VolumeRefType & data) const {
  ParametersType deltas;

  deltas[0] = data.getVoxelSize()[0] / 2.0;
  deltas[1] = data.getVoxelSize()[1] / 2.0;
  deltas[3] = data.getVoxelSize()[2] / 2.0;

  // [NS-2025-04-09]: I do not understand this 
  // heuristic: 
  // - deltas[2] is the increment of rotation in degrees around Z axis
  //   why should it be divided by the number of voxels along Z axis.
  // - 
  deltas[2] = 180.0 / M_PI / data.getSizeZ();
  deltas[4] = 180.0 / M_PI / data.getSizeX();
  deltas[5] = 180.0 / M_PI / data.getSizeY();


  // // Delta in degrees around Z axis
  // // is initialized using the number of voxels 
  // // in diagonal of the (X, Y) plan
  // deltas[2] = 90.0 / (sqrt(pow(data.getSizeX(), 2) 
  //                        + pow(data.getSizeY(), 2)));

  // // Delta in degrees around X axis
  // // is initialized using the number of voxels 
  // // in diagonal of the (Y, Z) plan
  // deltas[4] = 90.0 / (sqrt(pow(data.getSizeY(), 2) 
  //                        + pow(data.getSizeZ(), 2)));

  // // Delta in degrees around Y axis
  // // is initialized using the number of voxels 
  // // in diagonal of the (X, Z) plan
  // delta[5] = 90.0 / (sqrt(pow(data.getSizeX(), 2) 
  //                       + pow(data.getSizeZ(), 2)));
  
  return deltas;
}

int main( int argc, const char** argv )
{
  RegistrationProcessParams procparams;
  
  //
  // Application options definition
  // ------------------------------------------------------------
  AimsApplication app( argc, argv,
                       "Registration according to Mutual Information Method" );
  app.addOption( procparams.fileref, "-r", "source reference volume" );
  app.alias( "--reference", "-r" );
  app.addOption( procparams.filetest, "-t", "source test volume to register)" );
  app.alias( "--test", "-t" );
  app.addOption( procparams.refchannel, "-cr",  "Channel of multi-channel reference image to use during registration estimation\n"
                                      "Possible types and values:\n"
                                      "type : RGB or RGBA\n"
                                      "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                      "type : HSV\n"
                                      "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                      "[default is set to norm]\n\n", true);
  app.addOption( procparams.testchannel, "-ct",  "Channel of multi-channel test image to use during registration estimation\n"
                                      "Possible types and values:\n"
                                      "type : RGB or RGBA\n"
                                      "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                      "type : HSV\n"
                                      "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                      "[default is set to same value as reference channel]\n\n", true);
  app.addOption( procparams.direct, "--dir",
                 "test_TO_ref: output transfomation filename "
                 "[default=<test>_TO_<ref>.trm]", true );
  app.addOption( procparams.inverse, "--inv",
                 "ref_TO_test: output transfomation filename "
                 "[default=<ref>_TO_<test>.trm]", true );
  app.addOption( procparams.fileLog, "-l", "output log file", true );
  app.alias( "--logger", "-l" );
  app.addOption( procparams.sn, "--serialnum", "number of the frame to consider for ref "
      "[default 0], relevant for dynamic series ref images", true );
  app.addOption( procparams.tx, "--Tx",
                 "Init Translaction coord [default=0.]", true );
  app.addOption( procparams.ty, "--Ty",
                 "Init Translaction coord [default=0.]", true );
  app.addOption( procparams.tz, "--Tz",
                 "Init Translaction coord [default=0.]", true );
  app.addOption( procparams.dtx, "--dTx",
                 "Increment for variable Tx [default=heuristics in code]",
                 true );
  app.addOption( procparams.dty, "--dTy",
                 "Increment for variable Ty [default=heuristics in code]",
                 true );
  app.addOption( procparams.dtz, "--dTz",
                 "Increment for variable Tz [default=heuristics in code]",
                 true );
  app.addOption( procparams.rx, "--Rx", "Init Rotation coord [default=0.]", true );
  app.addOption( procparams.ry, "--Ry", "Init Rotation coord [default=0.]", true );
  app.addOption( procparams.rz, "--Rz", "Init Rotation coord [default=0.]", true );
  app.addOption( procparams.drx, "--dRx",
                 "Increment for variable Rx [default=heuristics in code]",
                 true );
  app.addOption( procparams.dry, "--dRy",
                 "Increment for variable Ry [default=heuristics in code]",
                 true );
  app.addOption( procparams.drz, "--dRz",
                 "Increment for variable Rz [default=heuristics in code]",
                 true );
  app.addOption( procparams.interpolator, "--interpolation",
                 "interpolation  nearest/linear/spline [default=linear]",
                 true );
  app.addOption( procparams.optimizer, "--optimization",
                 "optimaizer  powell/random/single [default=powell]\n"
                 "single --> get a one shot evaluation\n"
                 "random --> sample cost value in a given area\n"
                 "powell --> cost minimization with powell algorithm", true );
  app.addOption( procparams.pdf, "--pdfcalc",
                 "Pdf calculus mode : direct ou pv [default=pv]",
                 true );
  app.addOption( procparams.gc, "--gcinit",
                 "Init with GravCenter: yes/no [default=yes]", true );
  app.addOption( procparams.objfunc, "--index",
                 "index to be optimized mi/cr [default=mi]\n"
                 "Mutual Information --> mi\n"
                 "Correlation Ratio  --> cr", true );
  app.addOption( procparams.rsp, "--refstartpyr",
                 "level of the multiresolution optimization:\n"
                 "start resolution level for ref\n", true );
  app.addOption( procparams.rep, "--refendpyr",
                 "level of the multiresolution optimization:\n"
                 "end resolution level for ref", true );
  app.addOption( procparams.tsp, "--teststartpyr",
                 "level of the multiresolution optimization:\n"
                 "start resolution level for tes", true );
  app.addOption( procparams.tep, "--testendpyr",
                 "level of the multiresolution optimization:\n"
                 "end resolution level for tes", true );
  app.addOption( procparams.energysize, "--pays_size", "Half size the volume generated "
                 "if optimization=other activated", true );
  app.addOption( procparams.numlevel, "--graylevel",
                 "numb of classes in histogram pdf estimation [default=64]",
                 true );
  app.addOption( procparams.maskSize, "--masksize", "size of the gaussian convolution "
                 "mask in pdf estimation for Mutual Information [default=5]",
                 true );
  app.addOption( procparams.error, "--error", "tolerance on results [default=0.01]",
                 true );
  app.addOption( procparams.refratio, "--threshref",
                 "relative thresh applied prior to grav cent estimation",
                 true );
  app.alias( "--seuilref", "--threshref" );
  app.addOption( procparams.testratio, "--threshtest",
                 "relative thresh applied prior to grav cent estimation",
                 true );
  app.alias( "--seuiltest", "--threshtest" );
  app.addOption( procparams.generatecurve, "--generatecurve",
                 "generate curve file with default name 'curve' [default=true]",
                 true );
  app.alias( "-g", "--generatecurve" );

  try {
    app.initialize();
        
    //
    // Equipe with probe
    //
    //   OptimizerProbe<float, 6> * probe = 
    //     new MIRegistrationProbe<float, 6>(numlevel, 200, verbosity);
    //   optimizer->setProbe(probe);
    // ------------------------------------------------------------

    //
    // Read data
    //   - read ref data completely: assumed to be one frame
    //   - get info from header to manage test data on a frame by frame basis.
    //
    // WARNING: Data are read as short int, float data are scaled !!!!
    // ------------------------------------------------------------
    if(!procparams.testFinder.check(procparams.filetest)) {
      cerr << "could not load " << procparams.filetest << endl;
      return EXIT_FAILURE;
    }
    
    const Header *h = procparams.testFinder.header();
    const PythonHeader *ph = dynamic_cast<const PythonHeader *>(h);
    if(!ph) {
      cerr << "Can't get info for "<< procparams.filetest << endl;
      return EXIT_FAILURE;
    }
    
    vector<int> v;
    if (ph->getProperty("volume_dimension", v)) {
      for (int i = 2; i >= 0; --i)
        if (v[i] == 1) {
          procparams.axis2D = i;
          break;
        }
    }
    else { 
      cerr << "Can't get dimensions for "<< procparams.filetest << endl;
    }

    if (procparams.axis2D != -1) {
      std::cout << "2D transformation estimation (axis: " 
                << carto::toString(procparams.axis2D)
                << ")" << std::endl << std::flush;
      RegistrationProcess<3> process(procparams);
      process.doit();
    } 
    else {
      std::cout << "3D transformation estimation" << std::endl << std::flush;
      RegistrationProcess<6> process(procparams);
      process.doit();
    }

    return EXIT_SUCCESS; 
  }
  catch (user_interruption &) {}
  catch (exception & e) {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}

