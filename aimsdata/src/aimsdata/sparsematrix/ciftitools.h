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

#ifndef AIMS_SPARSEMATRIX_CIFTITOOLS_H
#define AIMS_SPARSEMATRIX_CIFTITOOLS_H

#include <cartobase/smart/rcptr.h>
#include <map>
#include <vector>
#include <string>
#include <list>

template <typename T> class TimeTexture;
class Void;
template <int D, typename T> class AimsTimeSurface;

namespace carto
{
  class Object;
}


namespace aims
{
  class SparseOrDenseMatrix;

  /** CIFTI-2 shaped matrices manipulation tools.

      This class eases manipulation of matrices (SparseOrDenseMatrix) with
      CIFTI information in their header. CIFTI information specify the nature
      of data contained in the matrix, and can bring ROIs or parcellations
      information.

      Mainly one can obtain textures to map matrix information onto meshes.
      Textures may be ROI information (label textures, TimeTexture<int>),
      or matrix values textures, possibly expanded onto brain regions
      (TimeTexture<float> in this case).
  */
  class CiftiTools
  {
  public:
    typedef std::map<std::string, int> BrainStuctureToMeshMap;
    typedef std::vector<carto::rc_ptr<TimeTexture<int> > > RoiTextureList;
    typedef std::vector<carto::rc_ptr<TimeTexture<float> > > TextureList;

    CiftiTools(
      carto::rc_ptr<SparseOrDenseMatrix> matrix,
      const BrainStuctureToMeshMap & smap = BrainStuctureToMeshMap() );
    ~CiftiTools();

    const carto::rc_ptr<SparseOrDenseMatrix> matrix() const
    { return _matrix; }
    carto::rc_ptr<SparseOrDenseMatrix> matrix()
    { return _matrix; }
    void setMatrix( carto::rc_ptr<SparseOrDenseMatrix> matrix );
    const BrainStuctureToMeshMap & brainStructureMap() const
    { return _smap; }
    BrainStuctureToMeshMap & brainStructureMap()
    { return _smap; }
    void setBrainStructureMap( const BrainStuctureToMeshMap & smap )
    { _smap = smap; }
    /** Get ROI information for a given matrix dimension.

        If the matrix dimension defines ROIs (parcels, brain regions), textures
        defining these regions are built.

        If the matrix dimension defines labels, an empty texture will be
        created, with appropriate colormap information

        If the matrix dimensions defines scalars or series, this function will
        probably not be useful.
    */
    RoiTextureList *roiTextureFromDimension( int dim ) const;
    /** Get matrix data in textures, possibly expanded to regions if needed.

        Parcels data will be expanded to all vertices of each parcel, etc.
        The resulting textures can be mapped on appropriate brain meshes.

        Scalar or time series data will be stored in texture timepoints.

        Labels data will be stored in separate textures because they provide
        possibly different colormaps. In this case the returned list will have
        the size meshes_number * labels_number, and will be stored in this
        order:

        [ mesh0_label0, mesh1_label0, .. meshN_label0, mesh0_label1, ..
          meshN_label1, .. mesh0_label0, .. meshN_labelM ]
    */
    TextureList *expandedValueTextureFromDimension(
      int dim,
      const std::vector<int> & dim_indices_pos,
      TextureList* existing_tex_list = 0 ) const;

    // intermediate
    /** Define the brain structure to mesh mapping: several meshes may be
        involved in CIFTI data mapping (typically a left hemisphere mesh and
        a right)
    */
    static BrainStuctureToMeshMap defaultBrainStructureToMeshMap();
    int valuesDimNum() const;
    static int valuesDimNum( carto::Object cifti_info );
    static size_t valuesDimSize( const SparseOrDenseMatrix & mat,
                                 int value_dim );
    static std::string dimensionType( carto::Object cifti_info, int dim );
    std::string dimensionType( int dim ) const;

    // lower-level (could probably be private)
    carto::Object getDimensionObject( int dim ) const;

    /** buid a dimension object in a simple case where the matrix (probably
        non-cifti) is a reduced matrix over regions of a single mesh */
    template <typename T>
    void buildDimensionObjectFromLabelsTexture( int dim,
                                                const TimeTexture<T> & tex );

    static void getLabelsInfo( carto::Object cifti_info, RoiTextureList & );
    static void getSeriesInfo( carto::Object cifti_info, RoiTextureList & );
    void getParcelsInfo( carto::Object cifti_info, RoiTextureList & ) const;
    void getBrainModelsInfo( carto::Object cifti_info,
                             RoiTextureList & ) const;
    void getBrainModelsTexture(
      carto::Object cifti_info, TextureList & texlist, int dim,
      const std::vector<int> & dim_indices_pos ) const;
    void getParcelsTexture(
      carto::Object cifti_info, TextureList & texlist, int dim,
      const std::vector<int> & dim_indices_pos ) const;
    std::list<std::string> getBrainStructures( int dim,
                                               bool keepSurfaces = true,
                                               bool keepVoxels = true ) const;
    /** Get the list of matrix indices corresponding to a given brain
        structure, on a given dimension. */
    std::vector<int> getIndicesForBrainStructure(
      int dim, const std::string & struct_name ) const;
    /** Get the list of matrix indices corresponding to a given region on a
        mesh, for a given dimension.
    */
    std::vector<int> getIndicesForSurfaceIndices(
      int dim, int surface_num, const std::vector<int> & roi_indices ) const;
    size_t getBrainStructureMeshNumberOfNodes(
      int dim, const std::string & struct_name ) const;
    /** checks if the given mesh matches a surface description in the given
        dimension of the Cifti matrix.
        Returns true if it matches, and in this case, brainmodels and
        surf_index are filled in accordingly.
        surf_hint specifies which surface (index) is tested first.
    */
    bool isMatchingSurface( int dim, const AimsTimeSurface<3, Void> & mesh,
                            std::list<std::string> & brainmodels,
                            int & surf_index, int surf_hint = 0 ) const
    {
      return isMatchingSurfaceOrTexture( dim, mesh, brainmodels, surf_index,
                                         surf_hint );
    }
    /** checks if the given texture matches a surface description in the given
        dimension of the Cifti matrix.
        Returns true if it matches, and in this case, brainmodels and
        surf_index are filled in accordingly.
        surf_hint specifies which surface (index) is tested first.
    */
    template <typename T>
    bool isMatchingTexture( int dim, const TimeTexture<T> & tex,
                            std::list<std::string> & brainmodels,
                            int & surf_index, int surf_hint = 0 ) const
    {
      return isMatchingSurfaceOrTexture( dim, tex, brainmodels, surf_index,
                                         surf_hint );
    }
    bool isMatchingSurfaceOrTexture( int dim, size_t nvertices,
                                     const carto::Object header,
                                     std::list<std::string> & brainmodels,
                                     int & surf_index,
                                     int surf_hint = 0 ) const;

  private:
    template <typename T> bool
      isMatchingSurfaceOrTexture( int dim, const T & mesh,
                                  std::list<std::string> & brainmodels,
                                  int & surf_index, int surf_hint = 0 ) const;
    void getSurfaceModelTexture(
      carto::Object surf, TextureList & texlist,
      const BrainStuctureToMeshMap & smap,
      SparseOrDenseMatrix & mat, int dim,
      const std::vector<int> & dim_indices_pos,
      unsigned  nsurfaces ) const;

    struct Private;

    Private *d;
    mutable carto::rc_ptr<SparseOrDenseMatrix> _matrix;
    BrainStuctureToMeshMap _smap;
  };

}

#endif

