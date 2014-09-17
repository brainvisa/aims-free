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

#ifndef AIMSALGO_SIGNALFILTER_FILTERINGIMAGEALGORITHM_H
#define AIMSALGO_SIGNALFILTER_FILTERINGIMAGEALGORITHM_H

//--- aims -------------------------------------------------------------------
#include <aims/algorithm/imagealgorithm.h>
#include <aims/data/data.h>
#include <aims/utility/progress.h>
#include <aims/connectivity/structuring_element.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/type/datatypeinfo.h>
//--- std --------------------------------------------------------------------
#include <iostream>
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  //   FILTERING IMAGE ALGORITHM: INTERFACE
  //==========================================================================
  /// Pure abstract class: interface for filtering image algorithms
  template <typename T>
  class FilteringImageAlgorithmInterface:
    virtual public ImageAlgorithmInterface<T>
  {
    public:
      virtual ~FilteringImageAlgorithmInterface() {};
      virtual FilteringImageAlgorithmInterface<T> * clone() const = 0;
      virtual void setParameters( const StructuringElement & strel,
                                  carto::Object options ) = 0;
  };

  namespace singlechannel {

    //========================================================================
    //   FILTERING IMAGE ALGORITHM: SINGLE CHANNEL
    //========================================================================
    /**
     *  \brief aims::singlechannel::FilteringImageAlgorithm is the algorithm
     *  to apply filter on single channel image.
     *
     *  The \c aims::FilteringImageAlgorithm class is used
     *  to filter single channel image.
     *
     *  \tparam VoxelType type of voxel that will be filtered.
     *  \tparam FilteringFunctionType type of filtering to use
     *          (\c MeanFilterFunc, \c MedianFilterFunc, \c MinFilterFunc,
     *          \c MaxFilterFunc, ... ).
     *
     *  \sa MeanFilterFunc, MedianFilterFunc, MinFilterFunc, MaxFilterFunc,
     *      MajorityFilterFunc
     */
    template <typename VoxelType, class FilteringFunctionType>
    class FilteringImageAlgorithm:
      public FilteringImageAlgorithmInterface<VoxelType>
    {
      public:

        /// \param strel Structuring element that gives the offset to each
        ///              point to be included in the filtering of a iven
        ///              voxel.
        /// \param options Options dictionary. Allows calling filtering
        ///                functions with specific parameters.
        FilteringImageAlgorithm( const StructuringElement & strel = strel::Cube( 1. ),
                                 carto::Object options = carto::none() ):
          _strel(strel.clone()),
          _options(options),
          _func(options)
        {}

        /// Copy constructor
        FilteringImageAlgorithm( const FilteringImageAlgorithm<VoxelType,FilteringFunctionType> & f ):
          _strel(f._strel->clone()),
          _options(f._options),
          _func(f._func)
        {}

        virtual ~FilteringImageAlgorithm() {}

        /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageDimensions
        /// method implementation.
        /// Get the output dimensions for Point4d original dimension.
        /// \return Point4d output dimension of the filtered image.
        virtual const Point4dl getOutputImageDimensions( const Point4dl & dims ) const {
          return dims;
        }

        /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageVoxelSize
        /// method implementation.
        /// Returns the output voxel size based on an input voxel size.
        /// \param Point4d voxelsize voxel size of the input image.
        /// \return \c Point4d voxel size of the output image.
        virtual const Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const {
          return voxelsize;
        }

        /// \c ImageAlgorithmInterface<VoxelType>::execute method implementation.
        /// Execute the filtering algorithm on the input image using the
        /// \c FilteringFunctionType
        /// \param in Input image to filter
        /// \return AimsData< VoxelType > Filtered image
        virtual carto::VolumeRef<VoxelType> execute( const carto::VolumeRef<VoxelType> & in ) const;

        /// Implement interface: change member values
        virtual void setParameters( const StructuringElement & strel,
                                    carto::Object options )
        {
          _strel = StructuringElementRef( strel.clone() );
          _options = options;
          _func = FilteringFunctionType( options );
        }

        /// Implement interface: clone
        virtual FilteringImageAlgorithm<VoxelType,FilteringFunctionType> * clone() const {
          return new FilteringImageAlgorithm<VoxelType,FilteringFunctionType>(*this);
        }

      protected:
        StructuringElementRef        _strel;
        carto::Object                _options;
        FilteringFunctionType        _func;
    };

    template <typename VoxelType, class FilteringFunctionType>
    inline
    carto::VolumeRef<VoxelType> FilteringImageAlgorithm<VoxelType, FilteringFunctionType>::execute(
      const carto::VolumeRef<VoxelType> & in ) const
    {
      int x, y, z, t, nx, ny, nz, n;
      int sz, sy, sx, ez, ey, ex;

      carto::VolumeRef<VoxelType> out( new carto::Volume<VoxelType>( *in ) );
      out->copyHeaderFrom( in->header() );
      out->fill( 0 );

      carto::VolumeRef<VoxelType> win(
        new carto::Volume<VoxelType>(
          in,
          typename carto::Volume<VoxelType>::Position4Di( 0, 0, 0, 0 ),
          typename carto::Volume<VoxelType>::Position4Di( 1, 1, 1, 1 )
        )
      );

      // In linear case, compute structuring element
      StructuringElementRef strel = _strel;
      if( _func.isLinear() )
      {
        std::vector<double> vs(4,1.0);
        in->header().getProperty( "voxel_size", vs );
        StructuringElementRef se = _func.getStructuringElement( vs );
        if( se != strel::none() )
          strel = se;
      }

      // Manage borders / structuring element size
      const std::vector<int> & borders = in->getBorders();
      const std::vector<int> & amplitude = strel->getAmplitude();

      // When volume has borders, it is possible to use it to process filtering
      sz = amplitude[4] - borders[4];
      sy = amplitude[2] - borders[2];
      sx = amplitude[0] - borders[0];
      ez = in->getSizeZ() - amplitude[5] + borders[5];
      ey = in->getSizeY() - amplitude[3] + borders[3];
      ex = in->getSizeX() - amplitude[1] + borders[1];

      if( carto::verbose ) {
        std::cout << "Structuring element amplitude: [ ";
        for(int i = 0; i < 6; ++i)
          std::cout << carto::toString(amplitude[i]) << ( i==8 ? " " : ", ");
        std::cout << "]" << std::endl;

        std::cout << "Processing with borders: [ ";
        for(int i = 0; i < 8; ++i)
          std::cout << carto::toString(borders[i]) << ( i==8 ? " " : ", ");
        std::cout << "]" << std::endl;

        std::cout << "Start: [" << carto::toString(sx) << ", "
                                << carto::toString(sy) << ", "
                                << carto::toString(sz) << "]" << std::endl;

        std::cout << "End: [" << carto::toString(ex) << ", "
                              << carto::toString(ey) << ", "
                              << carto::toString(ez) << "]" << std::endl;
      }

      aims::Progression progress( out->getSizeT() * (ez - sz) * (ey - sy) - 1 );
      if (carto::verbose)
        std::cout << "Filtering progress: ";

      for( t = 0; t < in->getSizeT(); ++t )
        for( z = sz; z < ez; ++z )
          for( y = sy; y < ey; ++y, ++progress ) {
            if( carto::verbose ) {
              // Display progression
              // The "normal" operator << should be as:
              // std::cout << progress << std::flush;
              // but it doesn't work in gcc 4.0, there is a namespace
              // confusion, so we have to specify ::operator <<
              ::operator << ( std::cout, progress ) << std::flush;
            }
            for( x = sx; x < ex; ++x )
            {
              // Processes window to use for filtering
              n = 0;

              // Set view position in the volume
              win->setPosInRefVolume( typename carto::Volume<VoxelType>::Position4Di(x,y,z,t) );
              (*out)( x, y, z, t ) = _func.execute( win, strel );
            }
          }

      if (carto::verbose)
        std::cout << std::endl;

      return( out );
    }

  } // namespace singlechannel

  //==========================================================================
  //   FILTERING IMAGE ALGORITHM: MULTI CHANNELS
  //==========================================================================
  /// \brief aims::FilteringImageAlgorithm is the algorithm to apply filter on image.
  ///
  /// The \c aims::FilteringImageAlgorithm class is used
  /// to filter both single and multi channel image.
  ///
  /// \tparam T Voxel type
  /// \tparam F Filtering function class
  template <typename T, class F>
  class FilteringImageAlgorithm :
    virtual public ImageAlgorithmInterface<T>,
    public FilteringImageAlgorithmInterface<T>,
    public ImageAlgorithm<T,
              singlechannel::FilteringImageAlgorithm<
                typename carto::DataTypeTraits<T>::ChannelType,
                F >
              >
  {
    public:
      typedef T VoxelType;
      typedef F FilterFuncType;
      typedef ImageAlgorithm<VoxelType,
                singlechannel::FilteringImageAlgorithm<
                  typename carto::DataTypeTraits<VoxelType>::ChannelType,
                  FilterFuncType >
                >
              ImageAlgorithmType;
      typedef typename ImageAlgorithmType::SingleChannelImageAlgorithmType
              SingleChannelImageAlgorithmType;

      FilteringImageAlgorithm( const StructuringElement & strel = strel::Cube( 1. ),
                               carto::Object options = carto::none() ):
        ImageAlgorithmType( SingleChannelImageAlgorithmType( strel, options ) )
      {}

      virtual ~FilteringImageAlgorithm() {}

      /// Implement interface: change member values
      virtual void setParameters( const StructuringElement & strel,
                                  carto::Object options )
      {
        ImageAlgorithmType::_algo.setParameters( strel, options );
      }

      /// Implement interface: clone
      virtual FilteringImageAlgorithm<T,F> * clone() const {
        return new FilteringImageAlgorithm<T,F>(*this);
      }
  };

} // namespace aims

#endif
