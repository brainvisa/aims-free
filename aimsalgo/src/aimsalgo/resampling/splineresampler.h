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


#ifndef AIMS_RESAMPLING_SPLINERESAMPLER_H
#define AIMS_RESAMPLING_SPLINERESAMPLER_H

#include <aims/resampling/resampler.h>
#include <cstdlib>
#include <cmath>
#include <vector>

/// B-Spline-based resampling.
///
/// This is the base class for all resamplers based on B-spline interpolation
/// as described by Unser, Aldroubi & Eden in IEEE PAMI (1991).
/// A class computing the actual spline coefficient is derived for each
/// spline order (1 to 7).
///
/// \see LinearResampler CubicResampler QuadraticResampler QuinticResampler
///      SixthOrderResampler SeventhOrderResampler
///
/// Coefficients are computed through a recursive scheme from an input
/// reference volume. This recursive algorithm is fast for whole
/// volume resampling, but slower for single points. A cache mechanism has
/// thus been implemented so that coefficients are not recomputed if the
/// input volume did not change.
///
/// For an input volume \c inVolume of dimensions [dimX, dimY, dimZ],
/// coefficients are double precision values stored in a
/// volume of same dimensions. Any value inVolume*(x, y, z) can then be
/// interpolated through the formula:\n
/// inVolume*(x, y, z) = Sum[i]( c_i * B^n(x - x_i) * B^n(y - y_i) *
/// B^n(z - z_i) )\n
/// with c_i the coefficients, B^n a nth order B-spline and (x_i, y_i, z_i)
/// the discrete input image points. Given that the support of B^n is
/// ]-(n+1)/2, (n+1)/2[, only the (n+1)^d (with d the volume dimension in
/// [1,2,3]) closest coefficients/discrete points are used to interpolate a
/// given point.
///
/// \tparam T  Voxel data type
template <class T>
class SplineResampler : public Resampler< T >
{
public:

  SplineResampler();
  ~SplineResampler() { }

  /// Spline order (1 to 7)
  virtual int getOrder() const = 0;

  /// Computes spline coefficients corresponding to an input volume.
  ///
  /// Spline coefficients are recomputed only if one of these conditions is
  /// satisfied:
  /// - \c inVolume is different from the last volume used for coefficients
  ///   computation (in the sense that they share the same adress)
  /// - \c t is different from the last t used for coefficients computation
  /// - A call to reset() has previously been made
  ///
  /// \param inVolume  input image
  /// \param t         volume to use in the T dimension in the case where
  ///                  inVolume is a time series.
  /// \return          Volume of double containing the coefficients in the
  ///                  image domain. Border coefficient need to be retrieved
  ///                  by mirror.
  ///
  /// This method actually calls updateParameters() ad returns the coeff
  /// container
  AimsData<double> getSplineCoef( const AimsData< T >& inVolume,
				  int t = 0,
				  bool verbose = false );

  /// Clear the cache.
  ///
  /// After a call to this method, a call to getSplineCoef() recomputes
  /// the spline coefficients in any case.
  void reset();

protected:

  /// Resample a point
  ///
  /// A call to getSplineCoef() or updateParameters() must have been made
  /// using \c inVolume before.
  ///
  /// \param inVolume     volume to resample
  /// \param transform3d
  void doResample( const AimsData< T > &inVolume,
                   const aims::Transformation3d &transform3d,
                   const T &outBackground,
                   const Point3df &outLocation,
                   T &outValue,
                   int t ) const CARTO_OVERRIDE;

  void updateParameters( const AimsData< T >& inVolume, int t,
                         bool verbose ) const CARTO_OVERRIDE;
  void iirConvolveMirror( std::vector< double >& data ) const;

  // This method returns a mirror index when needed
  //
  // Only inVolume's size spline coefficient are computed since "outside"
  // coefficients are equal to their mirror inside the image domain.
  // This method computes this mirror correspondance.
  //
  // - If i is in [0, size-1]: returns i
  // - If i < 0:               returns -1
  // - If i >= size:           returns size - (i - size) - 2
  int getFold( int i, int size ) const;

  /// Returns B^n( x - i )
  ///
  /// This method is defined by each nth order derived class
  virtual double getBSplineWeight( int i, double x ) const = 0;

  std::vector<double>  _poles;
  double               _gain;

  // These three mutable members handle the cache of spline coefficients
  mutable AimsData<double>     _splineCoefficients;
  mutable const AimsData<T> *  _lastvolume;
  mutable int                  _lasttime;
};

namespace aims {

  // This method returns a mirror index when needed
  //
  // Only inVolume's size spline coefficient are computed since "outside"
  // coefficients are equal to their mirror inside the image domain.
  // This method computes this mirror correspondance.
  //
  // - If i is in [0, size-1]: returns i
  // - If i < 0:               returns -1
  // - If i >= size:           returns size - (i - size) - 2
  inline int mirrorCoeff( int i, int size )
  {
    i = std::abs( i );
    if ( i < size )
    {

      return i;

    }
    if ( size == 1 )
    {

      return 0;

    }
    int size2 = ( size << 1 ) - 2;
    ldiv_t modOp = std::ldiv( i, size2 );
    return ( modOp.rem < size ) ? modOp.rem : ( size2 - modOp.rem );
  }

} // namespace aims

#endif
