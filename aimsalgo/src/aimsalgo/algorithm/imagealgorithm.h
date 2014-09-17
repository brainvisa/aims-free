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

#ifndef AIMSALGO_ALGORITHM_IMAGEALGORITHM_H
#define AIMSALGO_ALGORITHM_IMAGEALGORITHM_H

//--- aims -------------------------------------------------------------------
#include <aims/data/data.h>
#include <aims/utility/channel.h>
#include <aims/vector/vector.h>
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

namespace aims
{
  //==========================================================================
  //   IMAGE ALGORITHM INTERFACE
  //==========================================================================
  /// \brief aims::ImageAlgorithmInterface is the interface for an image
  ///        algorithm.
  ///
  /// The \c aims::ImageAlgorithmInterface class is used
  /// as a base class for image processing algorithm implementation.
  template <typename VoxelType>
  class ImageAlgorithmInterface {
    public:
      /// \c ImageAlgorithmInterface<VoxelType> Pure virtual method.
      /// Returns the output dimensions of the processed image. This is used
      /// when output image does not have the same dimensions as input image.
      /// \param Point4dl dims dimensions of the input image.
      /// \return \c Point4dl dimensions of the output image.
      virtual const Point4dl getOutputImageDimensions( const Point4dl & dims ) const = 0;

      /// \c ImageAlgorithmInterface<VoxelType> Pure virtual method.
      /// Returns the output voxel size of the processed image. This is used
      /// when output image does not have the same voxel size as input image.
      /// \param Point4df voxelsize voxel size of the input image.
      /// \return \c Point4df voxel size of the output image.
      virtual const Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const = 0;

      /// \c ImageAlgorithmInterface<VoxelType> Pure virtual method.
      /// Returns the image processed by an algorithm on the input image.
      /// \param in Input image to process
      /// \return VolumeRef< VoxelType > processed image
      virtual carto::VolumeRef<VoxelType> execute( const carto::VolumeRef<VoxelType> & in ) const = 0;

      /// \c ImageAlgorithmInterface<VoxelType>
      /// Returns the image processed by an algorithm on the input image.
      /// \param in Input image to process
      /// \return AimsData< VoxelType > processed image
      AimsData<VoxelType> execute( const AimsData<VoxelType> & in ) const
      {
        return AimsData<VoxelType>( execute( carto::VolumeRef<VoxelType>( in.volume() ) ) );
      }

      /// Clone
      virtual ImageAlgorithmInterface<VoxelType> * clone() const = 0;
  };

  //==========================================================================
  //   IMAGE ALGORITHM SWITCH
  //==========================================================================
  /// \brief aims::ImageAlgorithmSwitch switches between multichannel and
  ///        monochannel \c ImageAlgorithmInterface
  ///
  /// \c aims::ImageAlgorithmSwitch is the base class to switch between
  /// multichannel and monochannel \c ImageAlgorithmInterface implementations.
  ///
  /// \tparam VoxelType    type of voxel that will be processed by
  ///                      \c ImageAlgorithmInterface
  /// \tparam MultiChannel bool that specify wether derived
  ///                      \c ImageAlgorithmInterface
  ///                      is monochannel or multichannel
  template <typename VoxelType, bool MultiChannel>
  class ImageAlgorithmSwitch {
    public:
      ImageAlgorithmSwitch(){}
  };

  /// \brief aims::ImageAlgorithmSwitch switch specialization for
  ///        mono-channel \c ImageAlgorithmInterface
  ///
  /// \c aims::ImageAlgorithmSwitch is the specialized class for mono-channel
  /// \c ImageAlgorithmInterface implementations.
  ///
  /// \tparam VoxelType type of voxel that will be processed by
  ///                   \c ImageAlgorithmInterface
  template <typename VoxelType>
  class ImageAlgorithmSwitch<VoxelType,false> {
    public:
      ImageAlgorithmSwitch(){}

      /// Execute \c ImageAlgorithmInterface<VoxelType> on
      /// \c AimsData<VoxelType>.
      /// \param in Input image to filter
      /// \return \c AimsData<VoxelType> Filtered image
      static AimsData<VoxelType> execute(
        const ImageAlgorithmInterface<VoxelType> & algo,
        const AimsData<VoxelType> & in )
      {
        // std::cout << "VoxelType :" << carto::DataTypeCode<VoxelType>::name() << std::endl
        //           << "VoxelType::ChannelType :"
        //           << carto::toString(carto::DataTypeCode<VoxelType>::name()) << std::endl
        //           << "ImageAlgorithm is NOT multi channel" << std::endl;
        // std::cout << "Processing channel 0" << std::endl;
        return algo.execute( in );
      }

      /// Execute \c ImageAlgorithmInterface<VoxelType> on
      /// \c VolumeRef<VoxelType>.
      /// \param in Input image to filter
      /// \return \c VolumeRef<VoxelType> Filtered image
      static carto::VolumeRef<VoxelType> execute(
        const ImageAlgorithmInterface<VoxelType> & algo,
        const carto::VolumeRef<VoxelType> & in )
      {
        return algo.execute( in );
      }
  };

  /// \brief aims::ImageAlgorithmSwitch switch specialization for
  ///        multi-channel \c ImageAlgorithmInterface
  ///
  /// \c aims::ImageAlgorithmSwitch is the specialized class for
  /// multi-channel \c ImageAlgorithmInterface implementation.
  ///
  /// \tparam VoxelType type of voxel that will be processed by
  ///                   \c ImageAlgorithmInterface
  template <typename VoxelType>
  class ImageAlgorithmSwitch<VoxelType,true> {
    public:
      ImageAlgorithmSwitch(){}

      /// Execute \c ImageAlgorithmInterface<VoxelType> on
      /// \c VolumeRef<VoxelType>.
      /// \param in Input image to filter
      /// \return \c VolumeRef< VoxelType > Filtered image
      static carto::VolumeRef<VoxelType> execute(
        const ImageAlgorithmInterface<typename VoxelType::ChannelType> & algo,
        const carto::VolumeRef<VoxelType> & in )
      {

        // std::cout << "VoxelType :"
        //           << carto::DataTypeCode<VoxelType>::name() << std::endl
        //           << "VoxelType::ChannelType :"
        //           << carto::toString(carto::DataTypeCode<typename VoxelType::ChannelType>::name()) << std::endl
        //           << "ImageAlgorithm is multi channel" << std::endl;

        ChannelSelector< carto::VolumeRef<VoxelType>,
                         carto::VolumeRef<typename VoxelType::ChannelType>
                       > selector;
        Point4dl dims = algo.getOutputImageDimensions(
          Point4dl( in->getSizeX(), in->getSizeY(), in->getSizeZ(), in->getSizeT() ) );
        std::vector<double> vsv( 4, 1. );
        carto::Object vso = in->header().getProperty("voxel_size");
        if( !vso.isNull() ) {
          vsv[0] = (double) vso->getArrayItem(0)->getScalar();
          vsv[1] = (double) vso->getArrayItem(1)->getScalar();
          vsv[2] = (double) vso->getArrayItem(2)->getScalar();
          vsv[3] = (double) vso->getArrayItem(3)->getScalar();
        }
        Point4df vs = algo.getOutputImageVoxelSize( Point4df( vsv[0], vsv[1], vsv[2], vsv[3] ) );
        vsv[0] = vs[0];
        vsv[1] = vs[1];
        vsv[2] = vs[2];
        vsv[3] = vs[3];

        carto::VolumeRef<VoxelType> out( dims[0], dims[1], dims[2], dims[3] );
        out->copyHeaderFrom( in->header() );
        out->header().setProperty( "voxel_size", vsv );

        for( uint8_t channel = 0;
            channel < DataTypeInfo<VoxelType>::samples();
            channel++)
        {
          if( carto::verbose )
            std::cout << "Processing channel :"
                      << carto::toString(channel)
                      << std::endl;

          // Split the data and process filter on each component
          const carto::VolumeRef<typename VoxelType::ChannelType> & inChannel = selector.select( in, channel );
          const carto::VolumeRef<typename VoxelType::ChannelType> & outChannel = algo.execute( inChannel );
          selector.set( out, channel, outChannel );
        }

        return out;
      }

      /// Execute \c ImageAlgorithmInterface<VoxelType> on
      /// \c AimsData<VoxelType>.
      /// \param in Input image to filter
      /// \return \c AimsData< VoxelType > Filtered image
      static AimsData<VoxelType> execute(
        const ImageAlgorithmInterface<typename VoxelType::ChannelType> & algo,
        const AimsData<VoxelType> & in )
      {
        return AimsData<VoxelType>( execute( algo, carto::VolumeRef<VoxelType>( in.volume() ) ) );
      }
  };

  //==========================================================================
  //   IMAGE ALGORITHM
  //==========================================================================
  /// \brief aims::ImageAlgorithm class used to implement image algorithms
  ///
  /// \c aims::ImageAlgorithm is a base class that allows to not take care
  /// about multi-channel management. When a multi-channel image is
  /// processed, the c SingleChannelAlgorithmType will be applied to each
  /// channel separately.
  ///
  /// \tparam VoxelType type of voxel that will be processed by
  ///                   \c ImageAlgorithmInterface
  /// \tparam SingleChannelAlgorithmType algorithm that will be used during
  ///                                    processing.
  template <typename T, class A>
  class ImageAlgorithm: virtual public ImageAlgorithmInterface<T>
  {
    public:
      typedef T VoxelType;
      typedef A SingleChannelImageAlgorithmType;

      ImageAlgorithm( SingleChannelImageAlgorithmType algo ): _algo(algo) {}

      /// \c ImageAlgorithmInterface<VoxelType>::execute method implementation
      /// Execute the algorithm on the input image.
      /// \param in Input image to process
      /// \return Processed image
      virtual AimsData<VoxelType> execute( const AimsData<VoxelType> & in ) const
      {
        return ImageAlgorithmSwitch<VoxelType,carto::DataTypeTraits<VoxelType>::is_multichannel>::execute(_algo, in);
      }

      virtual carto::VolumeRef<VoxelType> execute( const carto::VolumeRef<VoxelType> & in ) const
      {
        return ImageAlgorithmSwitch<VoxelType,carto::DataTypeTraits<VoxelType>::is_multichannel>::execute(_algo, in);
      }

      /// Convenience method that execute the algorithm on the input image.
      /// Please prefer the execute() method
      /// \param in Input image to process
      /// \return Processed image
      virtual AimsData< VoxelType > doit( const AimsData<VoxelType>& in ) const
      {
        return execute( in );
      }

      /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageDimensions
      /// method implementation.
      /// Returns the output dimensions based on input dimensions. This is
      /// used when output image has not the same dimensions as input image.
      /// \return \c Point4dl dimension of the output image.
      virtual const Point4dl getOutputImageDimensions( const Point4dl & dims ) const {
        return _algo.getOutputImageDimensions(dims);
      }

      /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageVoxelSize
      /// method implementation.
      /// Returns the output voxel size based on an input voxel size.
      /// \param Point4d voxelsize voxel size of the input image.
      /// \return \c Point4d voxel size of the output image.
      virtual const Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const {
        return _algo.getOutputImageVoxelSize(voxelsize);
      }

      /// Clone
      virtual ImageAlgorithm<T,A> * clone() const {
        return new ImageAlgorithm<T,A>(*this);
      }

    protected:
      /// Single channel
      SingleChannelImageAlgorithmType _algo;
  };

}

#endif
