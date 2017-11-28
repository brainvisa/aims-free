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
#ifndef AIMS_INTERPOLATION_INTERPOLATEDVOLUME_H
#define AIMS_INTERPOLATION_INTERPOLATEDVOLUME_H

//--- aims -------------------------------------------------------------------
#include <aims/data/data_g.h>               // AimsData
#include <aims/math/bspline.h>              // aims::TabulBSpline
#include <aims/vector/vector.h>             // Point*
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>        // carto::VolumeRef
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>          // carto::rc_ptr
//--- forward declaration ----------------------------------------------------
template <typename T> class SplineResampler;
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  //
  //                          CONTINUOUS VOLUME
  //
  //==========================================================================
  /// Spline interpolation of volumes with simple accessors to interpolated
  /// values.
  class InterpolatedVolume
  {
  public:
    /// typedefs
    typedef Point3dd                  Vector;
    typedef carto::VolumeRef<double>  Matrix;

    /// Constructors / Destructors / Copy
    /// @{
    /// Default constructor: allocates no data.
    InterpolatedVolume();
    /// Constructor from Volume
    /// \warning This is the most expensive constructor because the volume
    /// has to be fully copied to create an AimsData.
    template <typename T>
    InterpolatedVolume( const carto::Volume<T> & vol, unsigned order = 3 );
    /// Constructor from VolumeRef
    template <typename T>
    InterpolatedVolume( const carto::VolumeRef<T> & vol, unsigned order = 3 );
    /// Constructor from AimsData
    template <typename T>
    InterpolatedVolume( const AimsData<T> & vol, unsigned order = 3 );
    /// Copy constructor
    /// (underlying data is fully copied and not shared between the volumes)
    InterpolatedVolume( const InterpolatedVolume & other );
    /// Destructor
    virtual ~InterpolatedVolume();
    /// Assignment by copy
    /// (underlying data is fully copied and not shared between the volumes)
    InterpolatedVolume & operator=( const InterpolatedVolume & other );
    /// @}

    /// Access interpolated values
    /// @{
    double at( double x, double y = 0., double z = 0., long t = 0 ) const;
    Vector derivative( double x, double y = 0., double z = 0., long t = 0 ) const;
    Matrix nderivative( unsigned n, double x, double y = 0., double z = 0., long t = 0 ) const;
    // aliases
    double operator() ( double x, double y = 0., double z = 0., long t = 0 ) const;
    Vector der( double x, double y = 0., double z = 0., long t = 0 ) const;
    Matrix der2( double x, double y = 0., double z = 0., long t = 0 ) const;
    Matrix der3( double x, double y = 0., double z = 0., long t = 0 ) const;
    /// @}

    /// Access interpolated values in mm
    /// @{
    double atMm( double x, double y = 0., double z = 0., long t = 0 ) const;
    Vector derivativeMm( double x, double y = 0., double z = 0., long t = 0 ) const;
    Matrix nderivativeMm( unsigned n, double x, double y = 0., double z = 0., long t = 0 ) const;
    // aliases
    Vector derMm( double x, double y = 0., double z = 0., long t = 0 ) const;
    Matrix der2Mm( double x, double y = 0., double z = 0., long t = 0 ) const;
    Matrix der3Mm( double x, double y = 0., double z = 0., long t = 0 ) const;
    // dimension
    double getSizeXMm() const;
    double getSizeYMm() const;
    double getSizeZMm() const;
    /// @}

    /// Header / Dimension
    /// @{
          carto::PropertySet & header();
    const carto::PropertySet & header() const;
    double getSizeX() const;
    double getSizeY() const;
    double getSizeZ() const;
    long   getSizeT() const;
    /// @}

    /// Change interpolated volume
    /// @{
    template <typename T>
    void setVolume( const carto::Volume<T> & vol, int order = -1 );
    template <typename T>
    void setVolume( const carto::VolumeRef<T> & vol, int order = -1 );
    template <typename T>
    void setVolume( const AimsData<T> & vol, int order = -1 );
    /// @}

    /// Change coefficient volume
    /// @{
    template <typename T>
    void setCoeff( const carto::Volume<T> & vol, int order = -1  );
    template <typename T>
    void setCoeff( const carto::VolumeRef<T> & vol, int order = -1  );
    template <typename T>
    void setCoeff( const AimsData<T> & vol, int order = -1  );
    /// @}

    /// Parameters
    /// @{
    /// Interpolation order
    unsigned order() const;
    /// Is the volume empty (true if only default constructed)
    bool empty() const;
    /// Recursive/Fast/Tabular mode
    /// (default is Fast)
    void setRecursiveMode();
    /// Recursive/Fast/Tabular mode
    /// (default is Fast)
    void setFastMode();
    /// Recursive/Fast/Tabular mode
    /// (default is Fast)
    void setTabularMode();
    /// @}

    /// Access to coefficients
    /// @{
          carto::VolumeRef<double> & coefficients();
    const carto::VolumeRef<double> & coefficients() const;
    /// @}

  protected:
    carto::VolumeRef<double>  _coeff;
    carto::rc_ptr<BSpline>    _spline;
    unsigned                  _order;
  };

} // namespace aims

#endif // AIMS_INTERPOLATION_INTERPOLATEDVOLUME_H
