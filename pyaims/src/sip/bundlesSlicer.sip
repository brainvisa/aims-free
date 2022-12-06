namespace aims
{

  //-------------------------//
 //  BundlesSlicerListener  //
//-------------------------//

class BundlesSlicerListener
{
%TypeHeaderCode
#include <aims/fibers/bundlesSlicer.h>
%End

public:

  virtual ~BundlesSlicerListener();
  virtual void startBundleSlicing( aims::BundlesSlicer &slicer,
				   const aims::BundleInfo &bundle );
  virtual void newBundleSlice( aims::BundlesSlicer &slicer,
                               const aims::BundleInfo &bundle, float time,
                               const vector_POINT3DF &points );
  virtual void terminateBundleSlicing( aims::BundlesSlicer &slicer,
				       const aims::BundleInfo &bundle );
  virtual void noMoreBundleSlice( aims::BundlesSlicer &slicer );
};


  //-----------------//
 //  BundlesSlicer  //
//-----------------//

class BundlesSlicer : aims::BundleListener
{
%TypeHeaderCode
#include <aims/fibers/bundlesSlicer.h>
%End

public:

  virtual ~BundlesSlicer();
  void addBundlesSlicerListener( aims::BundlesSlicerListener & );
};



  //-----------------------//
 //  SimpleBundlesSlicer  //
//-----------------------//

class SimpleBundlesSlicer : aims::BundlesSlicer
{
%TypeHeaderCode
#include <aims/fibers/bundlesSlicer.h>
%End

public:
  SimpleBundlesSlicer( size_t numberOfSlice );
};


}; // namespace aims
