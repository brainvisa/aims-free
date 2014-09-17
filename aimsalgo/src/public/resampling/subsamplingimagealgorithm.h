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

#ifndef AIMSALGO_RESAMPLING_SUBSAMPLINGIMAGEALGORITHM_H
#define AIMSALGO_RESAMPLING_SUBSAMPLINGIMAGEALGORITHM_H

#include <iostream>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/type/datatypeinfo.h>
#include <aims/algorithm/imagealgorithm.h>
#include <aims/data/data.h>
#include <aims/utility/progress.h>
#include <aims/vector/vector.h>

namespace aims {

  namespace singlechannel {

  /// \brief aims::singlechannel::SubSamplingImageAlgorithm is the algorithm
  ///        to apply subsampling on single channel image.
  ///
  /// The \c aims::SubSamplingImageAlgorithm class is used
  /// to subsample a single channel image.
  ///
  /// \tparam VoxelType type of voxel that will be subsampled.
  /// \tparam FilteringFunctionType type of subsampling to use
  /// (\c MeanFilterFunc, \c MedianFilterFunc, \c MinFilterFunc,
  /// \c MaxFilterFunc, ... ).
  ///
  /// \sa MeanFilterFunc, MedianFilterFunc, MinFilterFunc, MaxFilterFunc,
  /// MajorityFilterFunc
    template <class VoxelType, class FilteringFunctionType>
    class SubSamplingImageAlgorithm: public ImageAlgorithmInterface<VoxelType>
    {
      public:
        SubSamplingImageAlgorithm( int sx = 3, int sy = 3, int sz = 1,
                                   carto::Object options = carto::none() );
        virtual ~SubSamplingImageAlgorithm() {}

        /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageDimensions
        /// method implementation.
        /// Returns the output dimensions based on an input dimensions.
        /// \return \c Point4d output dimensions of the subsampled image.
        virtual const Point4dl getOutputImageDimensions( const Point4dl & dims ) const {
          return Point4dl( dims[0] / _win_size_x,
                           dims[1] / _win_size_y,
                           dims[2] / _win_size_z,
                           dims[3] );

        }

        /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageVoxelSize
        /// method implementation.
        /// Returns the output voxel size based on an input voxel size.
        /// \param Point4d voxelsize voxel size of the input image.
        /// \return \c Point4d voxel size of the output image.
        virtual const Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const {
          return Point4df( voxelsize[0] * _win_size_x,
                           voxelsize[1] * _win_size_y,
                           voxelsize[2] * _win_size_z,
                           voxelsize[3] );
        }

        /// \c ImageAlgorithmInterface<VoxelType>::execute method implementation.
        /// Execute the subsampling algorithm on the input image using the \c FilteringFunctionType
        /// \param in AimsData<VoxelType> input image to subsample
        /// \return AimsData< VoxelType > subsampled image
        virtual carto::VolumeRef<VoxelType> execute( const carto::VolumeRef<VoxelType> & in ) const;

        /// NOT IMPLEMENTED
        /// For now, returns default constructor
        virtual SubSamplingImageAlgorithm<VoxelType, FilteringFunctionType> * clone() const {
          return new SubSamplingImageAlgorithm<VoxelType, FilteringFunctionType>();
        }
      protected:
        int                         _win_size_x;
        int                         _win_size_y;
        int                         _win_size_z;
        const FilteringFunctionType _func;
        carto::Object               _options;
    };


    template <class VoxelType, class FilteringFunctionType>
    inline
    SubSamplingImageAlgorithm<VoxelType, FilteringFunctionType>::SubSamplingImageAlgorithm(
      int sx, int sy, int sz, carto::Object options
    ):
      _win_size_x(sx),
      _win_size_y(sy),
      _win_size_z(sz),
      _options(options)
    {
    }


    template <class VoxelType, class FilteringFunctionType>
    inline
    carto::VolumeRef<VoxelType> SubSamplingImageAlgorithm<VoxelType, FilteringFunctionType>::execute(
      const carto::VolumeRef< VoxelType >& in
    ) const
    {
      // std::cout << "Input dim: [" << carto::toString(in.dimX()) << ", "
      //                             << carto::toString(in.dimY()) << ", "
      //                             << carto::toString(in.dimZ()) << ", "
      //                             << carto::toString(in.dimT()) << "]"
      //                             << std::endl;
      // std::cout << "Window dim: [" << carto::toString(_win_size_x) << ", "
      //                              << carto::toString(_win_size_y) << ", "
      //                              << carto::toString(_win_size_z) << "]"
      //                              << std::endl;
      // std::cout << "Output dim: [" << carto::toString(in.dimX() / _win_size_x) << ", "
      //                              << carto::toString(in.dimY() / _win_size_y) << ", "
      //                              << carto::toString(in.dimZ() / _win_size_z) << "]"
      //                              << std::endl;
      Point4dl dim = getOutputImageDimensions( Point4dl( in->getSizeX(),
                                                         in->getSizeY(),
                                                         in->getSizeZ(),
                                                         in->getSizeT() ) );

      // std::cout << "Output dim: ["
      //           << carto::toString(dim[0]) << ", "
      //           << carto::toString(dim[1]) << ", "
      //           << carto::toString(dim[2]) << ", "
      //           << carto::toString(dim[3]) << "]"
      //           << std::endl;
      std::vector<double> vsv( 4, 1. );
      in->header().getProperty( "voxel_size", vsv );
      vsv[0] *= _win_size_x;
      vsv[1] *= _win_size_y;
      vsv[2] *= _win_size_z;

      // Data OUT
      carto::VolumeRef<VoxelType> out( dim[0], dim[1], dim[2], dim[3] );
      out->copyHeaderFrom( in->header() );
      out->header().setProperty( "voxel_size", vsv );

      // if (carto::verbose) {
      //   std::cout << "Input volume dimensions: ["
      //             << in.dimX() << ", "
      //             << in.dimY() << ", "
      //             << in.dimZ() << "]"
      //             << std::endl
      //             << "Subsampled volume dimensions: ["
      //             << out.dimX() << ", "
      //             << out.dimY() << ", "
      //             << out.dimZ() << "]"
      //             << std::endl
      //             << "Input voxel size: ["
      //             << in.sizeX() << ", "
      //             << in.sizeY() << ", "
      //             << in.sizeZ() << "]"
      //             << std::endl
      //             << "Subsampled voxel size: ["
      //             << out.sizeX() << ", "
      //             << out.sizeY() << ", "
      //             << out.sizeZ() << "]"
      //             << std::endl;
      // }

      if( out->getSizeT() * out->getSizeZ() * out->getSizeY() > 0 ) {
        // Window used to process
        // AimsData< VoxelType > win( _win_size_x * _win_size_y * _win_size_z );

        // Creates a view in the AimsData that has windows dimensions
        // and that
        carto::VolumeRef<VoxelType> win(
          new carto::Volume<VoxelType>(
            in,
            typename carto::Volume<VoxelType>::Position4Di( 0, 0, 0, 0 ),
            typename carto::Volume<VoxelType>::Position4Di( _win_size_x,
                                                            _win_size_y,
                                                            _win_size_z,
                                                            1 )
          )
        );

        int32_t i, j, k, t;
        unsigned m, n, l;
        aims::Progression progress(out->getSizeT() * out->getSizeZ() * out->getSizeY() - 1);

        if (carto::verbose)
          std::cout << "Subsampling progress: ";

        for( t = 0; t < (int32_t)out->getSizeT(); ++t )
          for( k = 0; k < (int32_t)out->getSizeZ(); ++k )
            for( j = 0; j < (int32_t)out->getSizeY(); ++j, ++progress )
            {
              // Display progression
              // The "normal" operator << should be as:
              // std::cout << progress << std::flush;
              // but it doesn't work in gcc 4.0, there is a namespace
              // confusion, so we have to specify ::operator <<
              if (carto::verbose)
                ::operator << ( std::cout, progress ) << std::flush;

              for( i = 0; i < (int32_t)out->getSizeX(); ++i ) {
                // Set view position in the volume
                win->setPosInRefVolume( typename carto::Volume<VoxelType>::Position4Di(
                  i * _win_size_x, j * _win_size_y, k * _win_size_z, t) );
                (*out)( i, j, k, t ) = _func.execute(win);
              }
            }

        if (carto::verbose)
          std::cout << std::endl;
      }

      return out;
    }

  }

  /// \brief aims::SubSamplingImageAlgorithm is the algorithm to subsample
  ///        image.
  ///
  /// The \c aims::SubSamplingImageAlgorithm class is used
  /// to subsample both single and multi channel image.
  template <class VoxelType, class FilterType>
  class SubSamplingImageAlgorithm :
    public ImageAlgorithm<VoxelType,
              aims::singlechannel::SubSamplingImageAlgorithm<
                typename carto::DataTypeTraits<VoxelType>::ChannelType,
                FilterType >
              >
  {
    public:
      typedef ImageAlgorithm<VoxelType,
              aims::singlechannel::SubSamplingImageAlgorithm<
                typename carto::DataTypeTraits<VoxelType>::ChannelType,
                FilterType >
              >
              ImageAlgorithmType;

      typedef typename ImageAlgorithmType::SingleChannelImageAlgorithmType
              SingleChannelImageAlgorithmType;

      typedef FilterType FilterFuncType;

      SubSamplingImageAlgorithm( int sx = 3, int sy = 3, int sz = 1,
                                 carto::Object options = carto::none() )
        : ImageAlgorithmType(
            SingleChannelImageAlgorithmType(sx, sy, sz, options)
          )
      {
      }
      virtual ~SubSamplingImageAlgorithm() { }

      /// NOT IMPLEMENTED
      /// For now, returns default constructor
      virtual SubSamplingImageAlgorithm<VoxelType, FilterType> * clone() const {
        return new SubSamplingImageAlgorithm<VoxelType, FilterType>();
      }
  };

}

#endif
