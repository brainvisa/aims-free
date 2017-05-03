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
				   const aims::BundleInfo &bundle ) /AutoGen/;
  virtual void newBundleSlice( aims::BundlesSlicer &slicer,
                               const aims::BundleInfo &bundle, float time,
                               const vector_POINT3DF &points ) /AutoGen/;
  virtual void terminateBundleSlicing( aims::BundlesSlicer &slicer,
				       const aims::BundleInfo &bundle ) /AutoGen/;
  virtual void noMoreBundleSlice( aims::BundlesSlicer &slicer ) /AutoGen/;
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
