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

//--- aims -------------------------------------------------------------------
#include <aims/algorithm/imagealgorithm.h>
#include <aims/signalfilter/filteringfunction.h>
#include <aims/data/data.h>
#include <aims/utility/progress.h>
#include <aims/vector/vector.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/type/datatypeinfo.h>
#include <cartobase/smart/rcptr.h>
//--- std --------------------------------------------------------------------
#include <iostream>
//----------------------------------------------------------------------------

namespace aims {

  template <typename T> class SubSamplingImageAlgorithm;
  namespace singlechannel { template <typename T> class SubSamplingImageAlgorithm; }

  //==========================================================================
  // SUBSAMPLING IMAGE ALGORITHM
  //==========================================================================
  /// \brief aims::SubSamplingImageAlgorithm is the algorithm to subsample
  /// image.
  ///
  /// The \c aims::SubSamplingImageAlgorithm class is used
  /// to subsample both single and multi channel image.
  template <typename T>
  class SubSamplingImageAlgorithm: public ImageAlgorithm<T>
  {
    public:
      //----------------------------------------------------------------------
      // types
      //----------------------------------------------------------------------
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef singlechannel::SubSamplingImageAlgorithm<ChannelType> SingleChannelImageAlgorithmType;

      //----------------------------------------------------------------------
      // image algorithm interface: implementation
      //----------------------------------------------------------------------
      virtual SubSamplingImageAlgorithm<T> *clone() const
      {
        return new SubSamplingImageAlgorithm<T>(*this);
      }

      //----------------------------------------------------------------------
      //   constructor (default+copy) & destructor
      //----------------------------------------------------------------------
    protected:
      SubSamplingImageAlgorithm();
      SubSamplingImageAlgorithm( int sx, int sy, int sz,
                                 const FilteringFunctionInterface<ChannelType> & f ):
        ImageAlgorithm<T>( SingleChannelImageAlgorithmType(sx, sy, sz, f) )
      {}

      SubSamplingImageAlgorithm( const SubSamplingImageAlgorithm & other ):
        ImageAlgorithm<T>( other )
      {}

      SubSamplingImageAlgorithm & operator= ( const SubSamplingImageAlgorithm & other )
      {
        ASSERT( typeid(other) == typeid(*this) );
        if( &other != this )
          ImageAlgorithm<T>::operator=( other );
        return *this;
      }

    public:
      virtual ~SubSamplingImageAlgorithm() {}
  };

  namespace singlechannel {

    //========================================================================
    // SUBSAMPLING IMAGE ALGORITHM: SINGLE CHANNEL PROCESSING
    //========================================================================
    /// \brief aims::singlechannel::SubSamplingImageAlgorithm is the algorithm
    ///        to apply subsampling on single channel image.
    ///
    /// The \c aims::SubSamplingImageAlgorithm class is used
    /// to subsample a single channel image.
    ///
    /// \tparam T type of voxel that will be subsampled.
    template <typename T>
    class SubSamplingImageAlgorithm: public ImageAlgorithmInterface<T>
    {
      public:
        //--------------------------------------------------------------------
        // types
        //--------------------------------------------------------------------
        typedef T VoxelType;

        //--------------------------------------------------------------------
        // image algorithm interface: implementation
        //--------------------------------------------------------------------
        virtual void execute( const carto::VolumeRef<T> & in,
                                    carto::VolumeRef<T> & out ) const;

        virtual Point4dl getOutputImageDimensions( const Point4dl & dims ) const {
          return Point4dl( dims[0] / _win_size_x,
                           dims[1] / _win_size_y,
                           dims[2] / _win_size_z,
                           dims[3] );
        }

        virtual Point4df getOutputImageVoxelSize( const Point4df & voxelsize ) const {
          return Point4df( voxelsize[0] * _win_size_x,
                           voxelsize[1] * _win_size_y,
                           voxelsize[2] * _win_size_z,
                           voxelsize[3] );
        }

        virtual SubSamplingImageAlgorithm<T> *clone() const
        {
          return new SubSamplingImageAlgorithm<T>(*this);
        }

        //--------------------------------------------------------------------
        //   constructor (default+copy) & destructor
        //--------------------------------------------------------------------
        SubSamplingImageAlgorithm( int sx, int sy, int sz,
                                   const FilteringFunctionInterface<T> & f ):
          ImageAlgorithmInterface<T>(),
          _win_size_x( sx ),
          _win_size_y( sy ),
          _win_size_z( sz ),
          _func( f.clone() )
        {}

        SubSamplingImageAlgorithm( const SubSamplingImageAlgorithm & other ):
          ImageAlgorithmInterface<T>(),
          _win_size_x( other._win_size_x ),
          _win_size_y( other._win_size_y ),
          _win_size_z( other._win_size_z ),
          _func( other._func->clone() )
        {}

        SubSamplingImageAlgorithm<T> & operator= ( const SubSamplingImageAlgorithm & other )
        {
          ASSERT( typeid(other) == typeid(*this) );
          if( &other != this ) {
            _win_size_x = other._win_size_x;
            _win_size_y = other._win_size_y;
            _win_size_z = other._win_size_z;
            _func.reset( other._func->clone() );
          }
          return *this;
        }

        virtual ~SubSamplingImageAlgorithm() {}

      protected:
        int  _win_size_x;
        int  _win_size_y;
        int  _win_size_z;
        carto::rc_ptr<FilteringFunctionInterface<T> >  _func;
    };


    template <typename T> inline
    void SubSamplingImageAlgorithm<T>::execute(
      const carto::VolumeRef<T> & in,
            carto::VolumeRef<T> & out
    ) const
    {
      int32_t st = (int32_t)out->getSizeT(),
              sz = (int32_t)out->getSizeZ(),
              sy = (int32_t)out->getSizeY(),
              sx = (int32_t)out->getSizeX();
      if( st * sz * sy > 0 )
      {
        // Creates a view in the input volume that has windows dimensions
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
        aims::Progression progress( (long) st *
                                    (long) sz *
                                    (long) sy *
                                    (long) sx );

        if( this->_verbose > 0 )
          std::cout << "Subsampling progress: ";

        for( t = 0; t < st; ++t )
          for( k = 0; k < sz; ++k )
            for( j = 0; j < sy; ++j )
              for( i = 0; i < sx; ++i )
              {
                if( this->_verbose > 0 )
                  (++progress).print();
                // Set view position in the volume
                win->setPosInRefVolume( typename carto::Volume<VoxelType>::Position4Di(
                  i * _win_size_x, j * _win_size_y, k * _win_size_z, t) );
//                 std::cout << "Window position: [" 
//                           << carto::toString(i * _win_size_x) << ", "
//                           << carto::toString(j * _win_size_y) << ", "
//                           << carto::toString(k * _win_size_z) << ", "
//                           << carto::toString(t) << "]" 
//                           << std::endl << std::flush;
                
                (*out)( i, j, k, t ) = _func->execute(win);
              }

        if( this->_verbose > 0 )
          std::cout << std::endl;
      }

    } // internal::SubSamplingImageAlgorithm::execute()

  } // namespace internal

}

#endif
