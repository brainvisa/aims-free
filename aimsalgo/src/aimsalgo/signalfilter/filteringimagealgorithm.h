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

#include <iostream>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/type/datatypeinfo.h>
#include <aims/algorithm/imagealgorithm.h>
#include <aims/data/data.h>
#include <aims/signalfilter/nonlin_filt-func.h>
#include <aims/utility/progress.h>

namespace aims {

  namespace singlechannel {

  /**
   *  \brief aims::singlechannel::FilteringImageAlgorithm is the algorithm to apply filter on single channel image.
   *
   *  The \c aims::FilteringImageAlgorithm class is used 
   *  to filter single channel image.
   *  \tparam VoxelType type of voxel that will be filtered.
   *  \tparam FilteringFunctionType type of filtering to use (\c MeanFilterFunc, \c MedianFilterFunc, \c MinFilterFunc, \c MaxFilterFunc, ... ).
   * 
   *  \sa MeanFilterFunc, MedianFilterFunc, MinFilterFunc, MaxFilterFunc, MajorityFilterFunc
   */
    template< class VoxelType, class FilteringFunctionType >
    class FilteringImageAlgorithm :
      public ImageAlgorithmInterface< VoxelType >
    {
      public:

        FilteringImageAlgorithm( int sx = 3, int sy = 3, int sz = 1 );

        virtual ~FilteringImageAlgorithm() { }

        /// \c ImageAlgorithmInterface<VoxelType>::getOutputImageDimensions method implementation.
        /// Get the output dimensions for Point4d original dimension.
        /// \return Point4d output dimension of the filtered image.
        virtual const Point4d getOutputImageDimensions( const Point4d & dims ) {
          return dims;
        }
        
        /// \c ImageAlgorithmInterface<VoxelType>::execute method implementation.
        /// Execute the filtering algorithm on the input image using the \c FilteringFunctionType
        /// \param in Input image to filter
        /// \return AimsData< VoxelType > Filtered image
        virtual AimsData< VoxelType > execute( const AimsData<VoxelType>& in );

      protected:

        int                         _win_size_x;
        int                         _win_size_y;
        int                         _win_size_z;
        const FilteringFunctionType _func;
    };

    template< class VoxelType, class FilteringFunctionType >
    inline
    FilteringImageAlgorithm<VoxelType, FilteringFunctionType>::FilteringImageAlgorithm( int sx, int sy, int sz ) 
          : _win_size_x(sx), 
            _win_size_y(sy),
            _win_size_z(sz)
    {
    }
        
    template< class VoxelType, class FilteringFunctionType >
    inline
    AimsData< VoxelType > FilteringImageAlgorithm<VoxelType, FilteringFunctionType>::execute( const AimsData<VoxelType>& in ) {

      int x, y, z, t, nx, ny, nz, n;
      short dx, dy, dz, bz, by, bx;

      dx = _win_size_x / 2 ;
      dy = _win_size_y / 2 ;
      dz = _win_size_z / 2 ;
    
      AimsData<VoxelType> out(in.dimX(), in.dimY(), in.dimZ(), in.dimT());
      out = in.clone();
      if( in.header() ) {
        out.setHeader( in.header()->cloneHeader( true ) );
      }
    
      // Update mask dims since it is always an odd number
      _win_size_x = dx * 2 + 1;
      _win_size_y = dy * 2 + 1;
      _win_size_z = dz * 2 + 1;

      if (carto::verbose)
        std::cout << "Window size : " << carto::toString( Point3d( _win_size_x,
                                                                  _win_size_y, 
                                                                  _win_size_z ) ) << std::endl;
                                                                
      AimsData<VoxelType> win( _win_size_x * _win_size_y * _win_size_z );
      bz = (out.dimZ() - dz);
      by = (out.dimY() - dy);
      bx = (out.dimX() - dx);
      
      aims::Progression progress(out.dimT() * out.dimZ() * out.dimY() - 1);
      if (carto::verbose)
        std::cout << "Filtering progress: ";
        
      for ( t = 0; t < in.dimT(); ++t )
        for ( z = dz; z < bz; ++z )
          for ( y = dy; y < by; ++y, ++progress ) {
            if (carto::verbose) {
              // Display progression
              std::cout << progress << std::flush;
            }
            for ( x = dx; x < bx; ++x )
            {
              // Processes window to use for filtering
              n = 0;
              for ( nz = -dz; nz <= dz; ++nz )
                for ( ny = -dy; ny <= dy; ++ny )
                  for ( nx = -dx; nx <= dx; ++nx, ++n )
                    win(n) = in(nx + x, ny + y, nz + z, t);
                  
              out( x, y, z, t ) = _func.doit( win );
            }
          }
        
      if (carto::verbose)
        std::cout << std::endl;

      return( out );

    }
        
  }

  /**
   *  \brief aims::FilteringImageAlgorithm is the algorithm to apply filter on image.
   *
   *  The \c aims::FilteringImageAlgorithm class is used 
   *  to filter both single and multi channel image.
   */
  template <class VoxelType, class FilterType>
  class FilteringImageAlgorithm :
    public ImageAlgorithm<VoxelType,  
              aims::singlechannel::FilteringImageAlgorithm<
                typename carto::DataTypeTraits<VoxelType>::ChannelType, 
                FilterType > 
              >
  {
    public:
      typedef ImageAlgorithm<VoxelType,  
              aims::singlechannel::FilteringImageAlgorithm<
                typename carto::DataTypeTraits<VoxelType>::ChannelType, 
                FilterType > 
              >
              ImageAlgorithmType;
              
      typedef typename ImageAlgorithmType::SingleChannelImageAlgorithmType 
              SingleChannelImageAlgorithmType;
              
      typedef FilterType FilterFuncType;
      
      FilteringImageAlgorithm( int sx = 3, int sy = 3, int sz = 1 )
        : ImageAlgorithmType(
            SingleChannelImageAlgorithmType(sx, sy, sz)
          )
      {
      }
      
      virtual ~FilteringImageAlgorithm() { }
  };

}

#endif
