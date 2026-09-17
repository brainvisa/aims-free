#ifndef AIMS_ROI_VOXELSAMPLING_H
#define AIMS_ROI_VOXELSAMPLING_H

#include <aims/vector/vector.h>
#include <cartobase/smart/rcptr.h>
#include <string>

namespace aims
{
    ////////////////////
   //                //
  //  VoxelSampler  //
 //                //
////////////////////

class VoxelSampler : public carto::RCObject
{
public:
  virtual ~VoxelSampler();

  virtual void next() = 0;
  virtual bool isValid() const = 0;
  virtual void restart() = 0;
  virtual const Point3df &value() const = 0;
  virtual std::string info() const = 0;
};


    //////////////////////////
   //                      //
  //  SimpleVoxelSampler  //
 //                      //
//////////////////////////

class SimpleVoxelSampler : public VoxelSampler
{
public:
  
  SimpleVoxelSampler( int );
  virtual ~SimpleVoxelSampler();

  void next();
  bool isValid() const;
  void restart();
  const Point3df &value() const;
  std::string info() const;

private:

  Point3df offset;
  Point3df point;
};

} // namespace aims

#endif
