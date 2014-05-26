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

#include <iostream>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/type/datatypeinfo.h>
#include <aims/data/data.h>
#include <aims/utility/channel.h>
#include <aims/vector/vector.h>

namespace aims
{  
  /**
   *  \brief aims::ImageAlgorithmInterface is the interface for an image algorithm.
   *
   *  The \c aims::ImageAlgorithmInterface class is used 
   *  as a base class for image processing algorithm implementation.
   */
   
  template < class VoxelType >
  class ImageAlgorithmInterface {
    public :
      /// \c ImageAlgorithmInterface<VoxelType> Pure virtual method.
      /// Returns the output dimensions of the processed image. This can be useful when output
      /// image does not have the same dimensions as input image.
      /// \param Point4d dims dimensions of the input image.
      /// \return \c Point4d dimensions of the output image.
      virtual const Point4d getOutputImageDimensions( const Point4d & dims ) = 0;
      
      /// \c ImageAlgorithmInterface<VoxelType> Pure virtual method.
      /// Returns the image processed by an algorithm on the input image.
      /// \param in Input image to process
      /// \return AimsData< VoxelType > processed image
      virtual AimsData<VoxelType> execute( const AimsData<VoxelType> & in ) = 0;
  };

  /**
   *  \brief aims::ImageAlgorithmSwitch switches between multichannel and monochannel \c ImageAlgorithmInterface
   *
   *  \c aims::ImageAlgorithmSwitch is the base class to switch between multichannel 
   *  and monochannel \c ImageAlgorithmInterface implementations.
   *  \tparam VoxelType type of voxel that will be processed by \c ImageAlgorithmInterface
   *  \tparam MultiChannel bool that specify wether derived \c ImageAlgorithmInterface 
   *                       is monochannel or multichannel
   */
  template < class VoxelType, bool MultiChannel >
  class ImageAlgorithmSwitch {
      
    public :
      ImageAlgorithmSwitch(){}
  };

  // Partial mono channel specialization
  /**
   *  \brief aims::ImageAlgorithmSwitch switch specialization for mono-channel \c ImageAlgorithmInterface
   *
   *  \c aims::ImageAlgorithmSwitch is the specialized class for mono-channel 
   *  \c ImageAlgorithmInterface implementations.
   *  \tparam VoxelType type of voxel that will be processed by \c ImageAlgorithmInterface
   */
  template < class VoxelType >
  class ImageAlgorithmSwitch<VoxelType, false> {
      
    public :
      ImageAlgorithmSwitch(){}
      
      /// Execute \c ImageAlgorithmInterface<VoxelType> on \c AimsData<VoxelType>.
      /// \param in Input image to filter
      /// \return \c AimsData< VoxelType > Filtered image
      static AimsData<VoxelType> execute( ImageAlgorithmInterface<VoxelType> & algo, 
                                          const AimsData<VoxelType> & in ) {
    //         std::cout << "VoxelType :" << carto::DataTypeCode<VoxelType>::name() << std::endl
    //                   << "VoxelType::ChannelType :" 
    //                   << carto::toString(carto::DataTypeCode<VoxelType>::name()) << std::endl
    //                   << "ImageAlgorithm is NOT multi channel" << std::endl;
    //         std::cout << "Processing channel 0" << std::endl;
        return algo.execute( in );
      }
  };

  // Partial multi channel specialization
  /**
   *  \brief aims::ImageAlgorithmSwitch switch specialization for multi-channel \c ImageAlgorithmInterface
   *
   *  \c aims::ImageAlgorithmSwitch is the specialized class for multi-channel \c ImageAlgorithmInterface 
   *  implementation.
   *  \tparam VoxelType type of voxel that will be processed by \c ImageAlgorithmInterface
   */
  template < class VoxelType >
  class ImageAlgorithmSwitch<VoxelType, true> {
      
    public :
      ImageAlgorithmSwitch(){}
      
      /// Execute \c ImageAlgorithmInterface<VoxelType> on \c AimsData<VoxelType>.
      /// \param in Input image to filter
      /// \return \c AimsData< VoxelType > Filtered image
      static AimsData<VoxelType> execute( ImageAlgorithmInterface<typename VoxelType::ChannelType> & algo, 
                                          const AimsData<VoxelType> & in ) {

    //         std::cout << "VoxelType :" << carto::DataTypeCode<VoxelType>::name() << std::endl
    //                   << "VoxelType::ChannelType :" 
    //                   << carto::toString(carto::DataTypeCode<typename VoxelType::ChannelType>::name()) << std::endl
    //                   << "ImageAlgorithm is multi channel" << std::endl;

        ChannelSelector< AimsData<VoxelType>, AimsData<typename VoxelType::ChannelType> > selector;
        Point4d dims = algo.getOutputImageDimensions( Point4d( in.dimX(),
                                                                in.dimY(),
                                                                in.dimZ(),
                                                                in.dimT() ) );

        AimsData<VoxelType> out( dims[0],
                                 dims[1],
                                 dims[2],
                                 dims[3] );
        if( in.header() ) {
          out.setHeader( in.header()->cloneHeader( true ) );
        }

        for (uint8_t channel = 0; channel < DataTypeInfo< VoxelType >::samples(); channel++) {
          if (carto::verbose)
            std::cout << "Processing channel :" << carto::toString(channel) << std::endl;
          
          /* Split the data and process filter on each component */ \
          const AimsData<typename VoxelType::ChannelType> & inChannel = selector.select( in, channel );
          const AimsData<typename VoxelType::ChannelType> & outChannel = algo.execute( inChannel );
          selector.set( out, channel, outChannel );
        }
        
        return out;
      }
  };

  // Partial simple channel specialization
  /**
   *  \brief aims::ImageAlgorithm class used to implement image algorithms
   *
   *  \c aims::ImageAlgorithm is a base class that allows to not take care about
   *  multi-channel management. When a multi-channel image is processed, the
   *  \c SingleChannelAlgorithmType will be applied to each channel separately.
   *  \tparam VoxelType type of voxel that will be processed by \c ImageAlgorithmInterface
   *  \tparam SingleChannelAlgorithmType algorithm that will be used during processing.
   */
  template <class VoxelType, class SingleChannelAlgorithmType>
  class ImageAlgorithm : public ImageAlgorithmInterface<VoxelType>
  {
    public:
      
      typedef SingleChannelAlgorithmType SingleChannelImageAlgorithmType;
      
      ImageAlgorithm(SingleChannelImageAlgorithmType algo)
        : _algo(algo)
      {}
      
      /// \c ImageAlgorithmInterface<VoxelType>::execute method implementation
      /// Execute the algorithm on the input image.
      /// \param in Input image to process
      /// \return Processed image
      virtual AimsData< VoxelType > execute( const AimsData<VoxelType>& in ) {
        return ImageAlgorithmSwitch<VoxelType, carto::DataTypeTraits< VoxelType >::is_multichannel>::execute(_algo, in);
      }
      
      /// Convenience method that execute the algorithm on the input image.
      /// \param in Input image to process
      /// \return Processed image
      virtual AimsData< VoxelType > doit( const AimsData<VoxelType>& in ) {
        return execute( in );
      }      
      
      /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageDimensions method implementation.
      /// Get the output dimensions for Point4d original dimension. This is can be useful when output
      /// file has not the same dimension as input image (for example : doing subsampling).
      /// \return Point4d output dimension of the filtered image.
      virtual const Point4d getOutputImageDimensions( const Point4d & dims ) {
        return _algo.getOutputImageDimensions(dims);
      }

    protected:
      /// Single channel
      SingleChannelImageAlgorithmType _algo;
  };

}

#endif
