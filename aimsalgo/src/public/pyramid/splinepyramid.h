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

#ifndef AIMS_PYRAMID_SPLINEPYRAMID_H
#define AIMS_PYRAMID_SPLINEPYRAMID_H

//--- aims -------------------------------------------------------------------
#include <aims/interpolation/interpolatedvolume.h>
#include <aims/vector/vector.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>
//--- std --------------------------------------------------------------------
#include <vector>
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  //
  //                            SPLINE PYRAMID
  //
  //==========================================================================

  /// Pyramid of Interpolated Volumes
  class SplinePyramid
  {
  public:
    //------------------------------------------------------------------------
    /// InterpolatedVolume types
    //------------------------------------------------------------------------
    /// @{
    typedef typename InterpolatedVolume::Vector Vector;
    typedef typename InterpolatedVolume::Matrix Matrix;
    /// @}

    //------------------------------------------------------------------------
    /// Constructors / Destructors / Copy
    //------------------------------------------------------------------------
    /// @{
    SplinePyramid( const std::vector<InterpolatedVolume> & pyramid
                    = std::vector<InterpolatedVolume>() );
    template <typename T>
    SplinePyramid( const std::vector<carto::VolumeRef<T> > & pyramid,
                   unsigned order = 3,
                   bool is_coeff = false );
    SplinePyramid( const SplinePyramid & other );
    ~SplinePyramid();
    SplinePyramid & operator= ( const SplinePyramid & other );
    /// @}

    //------------------------------------------------------------------------
    /// Change pyramid
    //------------------------------------------------------------------------
    /// @{
    const std::vector<InterpolatedVolume> & pyramid() const;
    void setPyramid( const std::vector<InterpolatedVolume> & pyramid );
    /// @}

    //------------------------------------------------------------------------
    /// Level accessor
    //------------------------------------------------------------------------
    /// @{
    InterpolatedVolume & operator[] ( unsigned level );
    const InterpolatedVolume & operator[] ( unsigned level ) const;
    int nlevel() const;
    /// @}

    //------------------------------------------------------------------------
    /// Dimensions
    //------------------------------------------------------------------------
    /// @{
    double getSizeXMm() const;
    double getSizeYMm() const;
    double getSizeZMm() const;
    carto::PropertySet & header( unsigned level = 0 );
    const carto::PropertySet & header( unsigned level = 0 ) const;
    /// @}

  protected:
    std::vector<InterpolatedVolume> _pyramid;
  };


  //==========================================================================
  //
  //                         SPLINE PYRAMID BUILDER
  //
  //==========================================================================

  /// The method follows that of
  /// Under, Aldroubi & Eden, "The L2 Polynomial Spline Pyramid" in IEEE
  /// Transactions on Pattern Analysis and Machine Intelligence (1993)
  class SplinePyramidBuilder
  {
  public:
    //------------------------------------------------------------------------
    /// Constructors / Destructors / Copy
    //------------------------------------------------------------------------
    /// @{
    SplinePyramidBuilder( unsigned factor = 2, unsigned spline_order = 3 );
    SplinePyramidBuilder( const Point4du & factor, unsigned spline_order = 3 );
    SplinePyramidBuilder( const std::vector<Point4du> & factor, unsigned spline_order = 3 );
    SplinePyramidBuilder( const SplinePyramidBuilder & other );
    virtual ~SplinePyramidBuilder();
    SplinePyramidBuilder & operator=( const SplinePyramidBuilder & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    unsigned order() const;
    const std::vector<bool> & directions() const;
    const std::vector<Point4du> & factor() const;
    void setOrder( unsigned n );
    void setFactor( unsigned r );
    void setFactor( const Point4du & r );
    void setFactor( const std::vector<Point4du> & r );
    void setDirections( const std::vector<bool> & dir );
    void setDirections( bool dirx, bool diry, bool dirz, bool dirt );
    void setVerbose( int verbose = 1 );
    void setQuiet();
    /// @}

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// @{
    template <typename T>
    SplinePyramid execute( const carto::VolumeRef<T> & vol, bool is_coeff = false ) const;
    /// @}

  protected:
    unsigned              _order;
    std::vector<Point4du> _factor;
    std::vector<bool>     _dir;
    int                   _verbose;

    //------------------------------------------------------------------------
    // Helpers
    //------------------------------------------------------------------------
    std::vector<Point4du> computeFactors( const std::vector<int> & size ) const;
  };

  //--------------------------------------------------------------------------
  /// FUNCTORS
  //--------------------------------------------------------------------------
  template <typename T>
  SplinePyramid buildSplinePyramid( const carto::VolumeRef<T> & vol,
                                    unsigned order, unsigned factor = 2 )
  {
    SplinePyramidBuilder builder( factor, order );
    return builder.execute( vol );
  }

  template <typename T>
  SplinePyramid buildSplinePyramid( const carto::VolumeRef<T> & vol,
                                    unsigned order, const Point4du & factor )
  {
    SplinePyramidBuilder builder( factor, order );
    return builder.execute( vol );
  }
  template <typename T>
  SplinePyramid buildSplinePyramid( const carto::VolumeRef<T> & vol,
                                    unsigned order,
                                    const std::vector<Point4du> & factor )
  {
    SplinePyramidBuilder builder( factor, order );
    return builder.execute( vol );
  }

  template <typename T>
  SplinePyramid cubicSplinePyramid( const carto::VolumeRef<T> & vol,
                                    unsigned factor = 2 )
  {
    SplinePyramidBuilder builder( factor, 3 );
    return builder.execute( vol );
  }

  template <typename T>
  SplinePyramid cubicSplinePyramid( const carto::VolumeRef<T> & vol,
                                    const Point4du & factor )
  {
    SplinePyramidBuilder builder( factor, 3 );
    return builder.execute( vol );
  }

  template <typename T>
  SplinePyramid cubicSplinePyramid( const carto::VolumeRef<T> & vol,
                                    const std::vector<Point4du> & factor )
  {
    SplinePyramidBuilder builder( factor, 3 );
    return builder.execute( vol );
  }

} // namespace aims

#endif // AIMS_PYRAMID_SPLINEPYRAMID_H
