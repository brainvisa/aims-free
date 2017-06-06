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

    bool diff( const Graph & g1, const Graph & g2 );
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

