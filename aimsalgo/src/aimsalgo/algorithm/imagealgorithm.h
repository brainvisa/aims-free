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
#include <cartobase/smart/rcptr.h>
//--- std --------------------------------------------------------------------
#include <iostream>
//----------------------------------------------------------------------------

namespace aims
{
  template <typename T> class ImageAlgorithmInterface;
  template <typename T> class ImageAlgorithm;
  template <typename T, bool M> class ImageAlgorithmSwitch;

  //==========================================================================
  //   IMAGE ALGORITHM INTERFACE
  //==========================================================================
  /// \brief aims::ImageAlgorithmInterface is the interface for an image
  ///  processing algorithm.
  ///
  /// Image processing algorithms take volumes as input and output.
  /// Output dimensions and voxel size may differ from input ones.
  template <typename T>
  class ImageAlgorithmInterface {
    public:
      //----------------------------------------------------------------------
      // types
      //----------------------------------------------------------------------
      typedef T VoxelType;

      //----------------------------------------------------------------------
      // interface (needs implementation in derived classes)
      //----------------------------------------------------------------------
      /// Returns the output dimensions of the processed image. This is used
      /// when output image does not have the same dimensions as input image.
      /// \param Point4dl dims dimensions of the input image.
      /// \return \c Point4dl  dimensions of the output image.
      ///
      /// The default implementation returns same dimensions as input.
      virtual Point4dl getOutputImageDimensions( const Point4dl & dims ) const
      {
        return Point4dl( dims );
      }

      /// Returns the output voxel size of the processed image. This is used
      /// when output image does not have the same voxel size as input image.
      /// \param Point4df voxelsize voxel size of the input image.
      /// \return \c Point4df  voxel size of the output image.
      ///
      /// The default implementation returns same voxel size as input.
      virtual Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const
      {
        return Point4df( voxelsize );
      }

      /// \c ImageAlgorithmInterface<T> Pure virtual method.
      ///
      /// Returns the image processed by an algorithm on the input image into
      /// an already allocated output volume.
      ///
      /// \param in   Input image to process
      /// \param out  Output image processed
      virtual void execute( const carto::VolumeRef<T> & in,
                                  carto::VolumeRef<T> & out ) const = 0;

      virtual void setOptions( const carto::Object & options ) {};
      virtual void updateOptions( const carto::Object & options ) {};

      /// \c ImageAlgorithmInterface<T> Pure virtual method.
      /// Clone the object
      virtual ImageAlgorithmInterface *clone() const = 0;

      //----------------------------------------------------------------------
      // utility method
      //----------------------------------------------------------------------
      /// \sa getOutputImageDimensions( const Point4dl & )
      virtual Point4dl getOutputImageDimensions( const long & dx = 1,
                                                 const long & dy = 1,
                                                 const long & dz = 1,
                                                 const long & dt = 1 ) const
      {
        return getOutputImageDimensions( Point4dl( dx, dy, dz, dt ) );
      }

      /// \sa getOutputImageVoxelSize( const Point4df & )
      virtual Point4df getOutputImageVoxelSize( const float & vx = 1.0,
                                                const float & vy = 1.0,
                                                const float & vz = 1.0,
                                                const float & vt = 1.0 ) const
      {
        return getOutputImageVoxelSize( Point4df( vx, vy, vz, vt ) );
      }

      /// \sa execute( const carto::VolumeRef<T> &, const carto::VolumeRef(<T> )
      virtual carto::VolumeRef<T> execute( const carto::VolumeRef<T> & in ) const
      {
        Point4dl dims( in.getSizeX(), in.getSizeY(), in.getSizeZ(), in.getSizeT() );
        std::vector<float> voxelsize(4,1.);
        in.header().getProperty( "voxel_size", voxelsize );
        Point4df voxel( voxelsize[0], voxelsize[1], voxelsize[2], voxelsize[3] );
        dims = getOutputImageDimensions( dims );
        voxel = getOutputImageVoxelSize( voxel );
        voxelsize[0] = voxel[0];
        voxelsize[1] = voxel[1];
        voxelsize[2] = voxel[2];
        voxelsize[3] = voxel[3];

        carto::VolumeRef<T> out( dims[0], dims[1], dims[2], dims[3] );
        out->copyHeaderFrom( in.header() );
        out.header().setProperty( "voxel_size", voxelsize );
        execute( in, out );

        return out;
      }

      /// \sa execute( const carto::VolumeRef<T> &, const carto::VolumeRef<T> & )
      virtual AimsData<T> execute( const AimsData<T> & in ) const
      {
        return AimsData<T>( execute( carto::VolumeRef<T>( in.volume() ) ) );
      }

      //----------------------------------------------------------------------
      // verbose
      //----------------------------------------------------------------------
    public:
      virtual void setVerbose( int level ) { _verbose = level; }
      virtual void setQuiet() { setVerbose(0); }
    protected:
      int _verbose;

      //----------------------------------------------------------------------
      // polymorphism
      //----------------------------------------------------------------------
    public:
      virtual ~ImageAlgorithmInterface() {}
    protected:
      ImageAlgorithmInterface(): _verbose(carto::verbose) {}
      ImageAlgorithmInterface( const ImageAlgorithmInterface & other ):
        _verbose(other._verbose)
      {}
      ImageAlgorithmInterface & operator=( const ImageAlgorithmInterface & other )
      {
        _verbose = other._verbose;
      }
  };


  //==========================================================================
  //   IMAGE ALGORITHM
  //==========================================================================
  /// \brief aims::ImageAlgorithm class used to implement image algorithms
  ///
  /// \c aims::ImageAlgorithm is a base class that allows to not take care
  /// about multi-channel management. When a multi-channel image is
  /// processed, the c ImageAlgorithmInterface will be applied to each
  /// channel separately.
  ///
  /// \tparam T  type of voxel that will be processed by
  ///            \c ImageAlgorithmInterface
  template <typename T>
  class ImageAlgorithm: public ImageAlgorithmInterface<T>
  {
    public:
      //----------------------------------------------------------------------
      // types
      //----------------------------------------------------------------------
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;

      //----------------------------------------------------------------------
      // interface implementation
      //----------------------------------------------------------------------
      virtual void execute( const carto::VolumeRef<T> & in,
                                  carto::VolumeRef<T> & out ) const
      {
        typedef ImageAlgorithmSwitch<T,carto::DataTypeTraits<T>::is_multichannel> S;
        S::execute( *_algo, in, out, this->_verbose );
      }

      virtual Point4dl getOutputImageDimensions( const Point4dl & dims ) const {
        return _algo->getOutputImageDimensions( dims );
      }

      virtual Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const {
        return _algo->getOutputImageVoxelSize( voxelsize );
      }

      // I need this line to unhide all the "execute" method since the
      // overloading of one hid all the others.
      using ImageAlgorithmInterface<T>::execute;
      using ImageAlgorithmInterface<T>::getOutputImageDimensions;
      using ImageAlgorithmInterface<T>::getOutputImageVoxelSize;

      virtual ImageAlgorithm<T> *clone() const {
        return new ImageAlgorithm<T>( *this );
      }

      virtual void setVerbose( int level )
      {
        ImageAlgorithmInterface<T>::setVerbose( level );
        _algo->setVerbose( level );
      }

      //----------------------------------------------------------------------
      // member method (backward compability)
      //----------------------------------------------------------------------
      /// Convenience method that execute the algorithm on the input image.
      /// Please prefer the execute() method
      /// \param in Input image to process
      /// \return Processed image
      virtual AimsData< T > doit( const AimsData<T>& in )
      {
        return ImageAlgorithmInterface<T>::execute( in );
      }

      //----------------------------------------------------------------------
      // polymorphism
      //----------------------------------------------------------------------
    public:
      virtual ~ImageAlgorithm() {}
    protected:
      ImageAlgorithm() {}
      ImageAlgorithm( const ImageAlgorithmInterface<ChannelType> & algo ):
        ImageAlgorithmInterface<T>(),
        _algo(algo.clone())
      {}
      ImageAlgorithm( ImageAlgorithmInterface<ChannelType> * algo, bool deepcopy = true ):
        ImageAlgorithmInterface<T>(),
        _algo( (deepcopy ? algo->clone() : algo ) )
      {}
      ImageAlgorithm( const ImageAlgorithm & other ):
        ImageAlgorithmInterface<T>(other),
        _algo(other._algo->clone())
      {}
      ImageAlgorithm & operator= ( const ImageAlgorithm & other )
      {
        ASSERT( typeid(other) == typeid(*this) );
        if( &other != this ) {
          ImageAlgorithmInterface<T>::operator=( other );
          _algo.reset( other._algo->clone() );
        }
        return *this;
      }

      //----------------------------------------------------------------------
      // member
      //----------------------------------------------------------------------
    protected:
      carto::rc_ptr<ImageAlgorithmInterface<ChannelType> > _algo;
  };

  //==========================================================================
  //   IMAGE ALGORITHM SWITCH
  //==========================================================================
  /// \brief aims::ImageAlgorithmSwitch switches between multichannel and
  /// monochannel \c ImageAlgorithmInterface
  ///
  /// \c aims::ImageAlgorithmSwitch is the base class to switch between
  /// multichannel and monochannel \c ImageAlgorithmInterface implementations.
  ///
  /// \tparam T    type of voxel that will be processed by
  ///              \c ImageAlgorithmInterface
  /// \tparam M    bool that specify wether derived \c ImageAlgorithmInterface
  ///              is monochannel or multichannel
  template <typename T, bool M>
  class ImageAlgorithmSwitch {
    public:
      typedef T VoxelType;
      ImageAlgorithmSwitch() {}
  };

  //--------------------------------------------------------------------------
  // specialization: single channel
  //--------------------------------------------------------------------------
  /// \brief aims::ImageAlgorithmSwitch switch specialization for
  /// mono-channel \c ImageAlgorithmInterface
  ///
  /// \c aims::ImageAlgorithmSwitch is the specialized class for mono-channel
  /// \c ImageAlgorithmInterface implementations.
  ///
  /// \tparam T   type of voxel that will be processed by
  ///             \c ImageAlgorithmInterface
  template <typename T>
  class ImageAlgorithmSwitch<T,false> {
    public:
      ImageAlgorithmSwitch() {}

      /// Execute \c ImageAlgorithmInterface<T> on \c VolumeRef<T>.
      /// \param algo Processing algorithm to run
      /// \param in   Input image to filter
      /// \param out  Filtered image (must be already allocated)
      static void execute( ImageAlgorithmInterface<T> & algo,
                           const carto::VolumeRef<T> & in,
                                 carto::VolumeRef<T> & out,
                           int verbose = carto::verbose )
      {
        return algo.execute( in, out );
      }
  };

  //--------------------------------------------------------------------------
  // specialization: multi channel
  //--------------------------------------------------------------------------
  /// \brief aims::ImageAlgorithmSwitch switch specialization for
  /// multi-channel \c ImageAlgorithmInterface
  ///
  /// \c aims::ImageAlgorithmSwitch is the specialized class for
  /// multi-channel \c ImageAlgorithmInterface implementation.
  ///
  /// \tparam T  type of voxel that will be processed by
  ///            \c ImageAlgorithmInterface
  template <typename T>
  class ImageAlgorithmSwitch<T,true> {
    public:
      typedef typename T::ChannelType ChannelType;
      ImageAlgorithmSwitch() {}

      /// Execute \c ImageAlgorithmInterface<T> on \c VolumeRef<T>.
      /// \param algo Processing algorithm to run
      /// \param in   Input image to filter
      /// \param out  Filtered image (must be already allocated)
      static void execute(
        ImageAlgorithmInterface<ChannelType> & algo,
        const carto::VolumeRef<T> & in,
              carto::VolumeRef<T> & out,
        int verbose )
      {
        ChannelSelector< carto::VolumeRef<T>,
                         carto::VolumeRef<ChannelType>
                       > selector;

        for( uint8_t channel = 0;
                     channel < DataTypeInfo<T>::samples();
                     channel++ )
        {
          if( verbose )
            std::cout << "Processing channel :"
                      << carto::toString(channel)
                      << std::endl;

          // Split the data and process filter on each component
          const carto::VolumeRef<ChannelType> & inChannel = selector.select( in, channel );
          const carto::VolumeRef<ChannelType> & outChannel = algo.execute( inChannel );
          selector.set( out, channel, outChannel );
        }
      }
  };

}

#endif
