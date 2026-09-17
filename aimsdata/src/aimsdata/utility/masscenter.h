#ifndef AIMS_UTILITY_MASSCENTER_H
#define AIMS_UTILITY_MASSCENTER_H

#include <map>
#include <cartobase/smart/rcptr.h>
#include <aims/roi/roiIterator.h>
#include <aims/resampling/linearInterpolator.h>

namespace carto
{
  template <typename T> class VolumeRef;
}

/** Mass centers, volumes, mass of ROIs in a volume

    Since Aims 5.2, mass center positions are in mm. They used to be in voxels,
    which was probably a bug, and was inconsistent with volumes which
    were already in mm3.
*/
template <class T>
class MassCenters
{
  public:
      
    typedef std::pair<Point3df, double> MassCenterInfo;
    typedef std::map<int, MassCenterInfo> TimedMassCenterInfo;
    typedef std::map<std::string, TimedMassCenterInfo> RoiMassCenterInfo;
    
    MassCenters(const carto::VolumeRef<T> & data,
                bool bin = false);
    MassCenters(const carto::VolumeRef<T> & data,
                const carto::rc_ptr<aims::RoiIterator> & roiIterator, 
                bool bin = false);
    void doit(bool force = false);
    RoiMassCenterInfo infos();
    TimedMassCenterInfo maskedmasscenter( aims::MaskIterator & maskIterator );
  
  private:
    bool _init;
    carto::VolumeRef<T> _data;
    bool _bin;
    carto::rc_ptr<aims::Interpolator> _interpolator;
    carto::rc_ptr<aims::RoiIterator> _roiIterator;
    RoiMassCenterInfo _masscenterinfos;

};

#endif
