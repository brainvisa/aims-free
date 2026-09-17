
#ifndef AIMS_MESH_MESHWATERSHED_H
#define AIMS_MESH_MESHWATERSHED_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>


namespace aims
{

  void distancesFromMesh( const AimsSurfaceTriangle & mesh,
                          std::vector<AimsVector<uint,2> > & edges,
                          std::vector<double> & weights );
  /** If I understand:
    \param lidx will contain the vertex index of each local maximum.
    \param ldepth will contain the lifetime (number of iterations before death)
      of each maximum.
    \param label will contain the basin label, for each vertex.
    \param lmajor will contain the label of the parent of each basin:
      label of the highest basin which grew over it.
  */
  int meshWatershed( const AimsSurfaceTriangle & mesh,
                     const std::vector<double> & field,
                     std::vector<int> & idx,
                     std::vector<int> & depth, std::vector<int> & major,
                     std::vector<int> & label, double threshold );
  int meshWatershed( const AimsSurfaceTriangle & mesh,
                     const TimeTexture<double> & field,
                     TimeTexture<int> & idx,
                     TimeTexture<int> & depth, TimeTexture<int> & major,
                     TimeTexture<int> & label, double threshold );
  /** Blobs labeling and bifurcations.

    \param idx: output table of seed node of each blob
    \param father: blob number which merges two (or more) others
    \param height: field value of the peak (seed) node in a blob
    \param label: blob number for each vertex
  */
  int meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                            const std::vector<double> & field,
                            std::vector<int> & idx,
                            std::vector<double> & height,
                            std::vector<int> & father,
                            std::vector<int> & label, double th );
  int meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                            const TimeTexture<double> & field,
                            TimeTexture<int> & idx,
                            TimeTexture<double> & height,
                            TimeTexture<int> & father,
                            TimeTexture<int> & label, double th );

  void blobsHeights( const AimsSurfaceTriangle & mesh,
                     const std::vector<double> & field,
                     const std::vector<int> & watershedlabels,
                     std::vector<int> & junctions );
}

#endif

