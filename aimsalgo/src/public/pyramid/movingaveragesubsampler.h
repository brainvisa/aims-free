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

#ifndef AIMS_PYRAMID_MOVINGAVERAGESUBSAMPLER_H
#define AIMS_PYRAMID_MOVINGAVERAGESUBSAMPLER_H

//============================================================================
//
//                       MOVING AVERAGE SUBSAMPLER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/movingaveragefilter.h>
#include <aims/pyramid/convolutionsubsampler.h>     // aims::ConvolutionFilter
#include <aims/vector/vector.h>                                    // Point4df
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  class MovingAverageSubSampler: public ConvolutionSubSampler<MovingAverageKernel>
  {
  public:

    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    MovingAverageSubSampler( unsigned r = 2, unsigned length = 3,
                             FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageSubSampler( const Point4du & r, unsigned length = 3,
                             FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageSubSampler( const Point4du & r, const Point4du & length,
                             FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageSubSampler( const MovingAverageSubSampler & other );
    virtual ~MovingAverageSubSampler();
    MovingAverageSubSampler & operator=( const MovingAverageSubSampler & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    Point4du length() const;
    FilterType::Direction type() const;
    /// Updates underlying basis functions
    void setLength( unsigned length );
    /// Updates underlying basis functions
    void setLength( const Point4du & length );
    /// Updates underlying basis functions
    void setType( FilterType::Direction symtype );
    /// @{

  protected:
    // Make some inherited functions private
    using ConvolutionSubSampler<MovingAverageKernel>::setBasisFunction;
  };

} // namespace aims

//============================================================================
//
//                   MOVING AVERAGE CASCADE SUBSAMPLER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/movingaveragefilter.h>
#include <aims/pyramid/convolutionsubsampler.h>     // aims::ConvolutionFilter
#include <aims/vector/vector.h>                                    // Point4df
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  class MovingAverageCascadeSubSampler: public ConvolutionSubSampler<MovingAverageCascadeKernel>
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    MovingAverageCascadeSubSampler( unsigned r = 2, unsigned length = 3, unsigned n = 2,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const Point4du & r, unsigned length = 3, unsigned n = 2,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const Point4du & r, const Point4du & length, unsigned n = 2,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const Point4du & r, const Point4du & length, const Point4du & n,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const MovingAverageCascadeSubSampler & other );
    virtual ~MovingAverageCascadeSubSampler();
    MovingAverageCascadeSubSampler & operator=( const MovingAverageCascadeSubSampler & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    Point4du length() const;
    Point4du cascade() const;
    FilterType::Direction type() const;
    /// Updates underlying basis functions
    void setLength( unsigned length );
    /// Updates underlying basis functions
    void setLength( const Point4du & length );
    /// Updates underlying basis functions
    void setCascade( unsigned length );
    /// Updates underlying basis functions
    void setCascade( const Point4du & length );
    /// Updates underlying basis functions
    void setType( FilterType::Direction symtype );
    /// @{

  protected:
    // Make some inherited functions private
    using ConvolutionSubSampler<MovingAverageCascadeKernel>::setBasisFunction;
  };

} // namespace aims

#endif // AIMS_PYRAMID_MOVINGAVERAGESUBSAMPLER_H
