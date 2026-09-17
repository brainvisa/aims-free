
#ifndef AIMS_DISTANCEMAP_ADVECTION_H
#define AIMS_DISTANCEMAP_ADVECTION_H

#include <cartodata/volume/volume.h>
#include <set>

namespace aims
{

  /** Propagate labels along a vector field.

      Labels are propagated in a front that grows from the seeds. Only the
      immediate neighbours of a voxel are considered when deciding whether to
      include it in a region. This makes the algorithm fast, but the direction
      of propagation is approximate.

      \par Interpretation of labels in the seed volume
        - zero: propagation zone;
        - positive: seed, should be propagated;
        - negative: forbidden region, will not be propagated, and nothing will
          be propagated here.
   */
  class GradientAdvection
  {
  public:
    GradientAdvection();
    virtual ~GradientAdvection();

    void setVerbose(bool = true);
    void setPropagationThreshold(float);

    /** Propagate a region using an overlap criterion.

       \warning This *experimental* method currently works in 2D only.
       Read the code!

       \return The returned volume contains 1 where the region has propagated,
       -1 in forbidden regions and 0 elsewhere.
     */
    carto::VolumeRef<int16_t>
    doit(const carto::VolumeRef<int16_t> &label_vol,
         const carto::VolumeRef<float> &gradX,
         const carto::VolumeRef<float> &gradY,
         const carto::VolumeRef<float> &gradZ,
         int16_t seedlabel);

    /** Propagate labels using the local direcion of a vector field.

        Each voxel is assigned the seed label of the 26-neighbour that the
        local field points to. More precisely, the field is normalized using
        the maximum norm, then rounded to the nearest integral voxel
        coordinates.

        \warning This method was only tested for isotropic voxels.
     */
    carto::VolumeRef<int16_t>
    propagate_all_labels(const carto::VolumeRef<int16_t> &seeds,
                         const carto::VolumeRef<float> &fieldx,
                         const carto::VolumeRef<float> &fieldy,
                         const carto::VolumeRef<float> &fieldz);

    /** Propagate labels along the gradient of a scalar field.

        Each voxel is assigned the seed label of its maximum 26-neighbour.

        \warning This probably makes little sense for non-isotropic voxels.
     */
    carto::VolumeRef<int16_t>
    descend_scalar_field(const carto::VolumeRef<int16_t> &seeds,
                         const carto::VolumeRef<float> &field);

    static const float default_propagation_threshold;

  private:
    bool _verbose;
    float _propagation_threshold;
  };
};

#endif
