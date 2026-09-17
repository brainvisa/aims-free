
#ifndef AIMS_ROI_ROI_DIFF_H
#define AIMS_ROI_ROI_DIFF_H

#include <graph/graph/graph.h>
#include <aims/bucket/bucket.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  /** Differences between two labelled graphs (sulci...).

      Both graphs are supposed to have the same voxels support, but graph nodes
      may be split differently in the two graphs. For each label differences
      are counted voxel-wise.
  */
  class RoiDiff
  {
  public:
    struct DiffStat
    {
      DiffStat()
        : matching_voxels( 0 ), unmatching_voxels( 0 ),
          //to_unknown( 0 ), from_unknown( 0 ),
          dice( 0. )
        {}

      /// nb of voxels from graph1 matching in graph2
      unsigned matching_voxels;
      /// nb of voxels from graph1 not matching in graph2
      unsigned unmatching_voxels;
  //     unsigned to_unknown;
  //     unsigned from_unknown;
      double dice;
      /// voxels from graph2
      BucketMap<Void> g2_bucket;
    };

    RoiDiff();
    ~RoiDiff();

    void diff( const Graph & g1, const Graph & g2 );
    void writeCSV( const std::string & name );
    void writeCSV( std::ostream & s );

    const std::map<std::string, DiffStat> & statsByLabel() const;
    const DiffStat & globalStats() const;
    carto::rc_ptr<BucketMap<Void> > mismatch_bucket() const;
    carto::rc_ptr<carto::Volume<int16_t> > graph2LabelVolume() const;
    const std::vector<std::string> & roiNames() const;
    const std::map<std::string, int> & roiNamesInv() const;

  private:
    struct Private;
    Private *d;
  };

}

#endif

