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
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/type/datatypeinfo.h>
#include <cartobase/object/object.h>
//--- std --------------------------------------------------------------------
#include <iostream>
//----------------------------------------------------------------------------

namespace aims {
  template <typename T> class FilteringImageAlgorithmInterface;
  template <typename T> class LinearFilteringImageAlgorithm;
  template <typename T> class LinearFilteringFunction;
  template <typename T> class ElementFilteringImageAlgorithm;
  template <typename T> class ElementFilteringFunction;
  namespace singlechannel {
    template <typename T> class LinearFilteringImageAlgorithm;
    template <typename T> class ElementFilteringImageAlgorithm;
  }

  //==========================================================================
  //   LINEAR FILTERING IMAGE ALGORITHM: MULTI CHANNELS
  //==========================================================================
  template <typename T>
  class LinearFilteringImageAlgorithm: public ImageAlgorithm<T>
  {
    public:
      //----------------------------------------------------------------------
      //   typedef
      //----------------------------------------------------------------------
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef singlechannel::LinearFilteringImageAlgorithm<ChannelType> SingleChannelImageAlgorithmType;

      //----------------------------------------------------------------------
      //   image algorithm interface: implementation
      //----------------------------------------------------------------------
      virtual void setOptions( const carto::Object & options )
      {
        _lalgo->setOptions( options );
      }

      virtual void updateOptions( const carto::Object & options )
      {
        _lalgo->updateOptions( options );
      }

      virtual LinearFilteringImageAlgorithm<T> *clone() const
      {
        return new LinearFilteringImageAlgorithm<T>(*this);
      }

      //----------------------------------------------------------------------
      //   constructor (default+copy) & destructor
      //----------------------------------------------------------------------
    protected:
      LinearFilteringImageAlgorithm();
      LinearFilteringImageAlgorithm( const LinearFilteringFunction<ChannelType> & f ):
        ImageAlgorithm<T>(),
        _lalgo( SingleChannelImageAlgorithmType( f ).clone() )
      {
        ImageAlgorithm<T>::_algo.reset( _lalgo );
      }
      LinearFilteringImageAlgorithm( const LinearFilteringImageAlgorithm<T> & other ):
        ImageAlgorithm<T>(),
        _lalgo( other._lalgo->clone() )
      {
        ImageAlgorithm<T>::_algo.reset( _lalgo );
      }
      LinearFilteringImageAlgorithm & operator= ( LinearFilteringImageAlgorithm & other )
      {
        _lalgo = other._lalgo->clone();
        ImageAlgorithm<T>::_algo.reset( _lalgo );
        return *this;
      }
    public:
      virtual ~LinearFilteringImageAlgorithm() {}

      //----------------------------------------------------------------------
      //   member
      //----------------------------------------------------------------------
    protected:
      SingleChannelImageAlgorithmType * _lalgo;

  };

  //==========================================================================
  //   ELEMENT FILTERING IMAGE ALGORITHM: MULTI CHANNELS
  //==========================================================================
  template <typename T>
  class ElementFilteringImageAlgorithm: public ImageAlgorithm<T>
  {
    public:
      //----------------------------------------------------------------------
      //   typedef
      //----------------------------------------------------------------------
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef singlechannel::ElementFilteringImageAlgorithm<ChannelType> SingleChannelImageAlgorithmType;

      //----------------------------------------------------------------------
      //   image algorithm interface: implementation
      //----------------------------------------------------------------------
      virtual void setOptions( const carto::Object & options )
      {
        _ealgo->setOptions( options );
      }

      virtual void updateOptions( const carto::Object & options )
      {
        _ealgo->updateOptions( options );
      }

      virtual ElementFilteringImageAlgorithm<T> *clone() const {
        return new ElementFilteringImageAlgorithm<T>(*this);
      }

      //----------------------------------------------------------------------
      //   member method
      //----------------------------------------------------------------------
      virtual void setStructuringElement( const StructuringElement & se )
      {
        _ealgo->setStructuringElement( se );
      }

      //----------------------------------------------------------------------
      //   constructor (default+copy) & destructor
      //----------------------------------------------------------------------
    protected:
      ElementFilteringImageAlgorithm(
        const ElementFilteringFunction<ChannelType> & f,
        const StructuringElement & se = strel::Cube(1.)
      ):
        ImageAlgorithm<T>(),
        _ealgo( SingleChannelImageAlgorithmType( f, se ).clone() )
      {
        ImageAlgorithm<T>::_algo.reset( _ealgo );
      }
      ElementFilteringImageAlgorithm( const ElementFilteringImageAlgorithm<T> & other ):
        ImageAlgorithm<T>(),
        _ealgo(other._ealgo->clone())
      {
        ImageAlgorithm<T>::_algo.reset( _ealgo );
      }
      ElementFilteringImageAlgorithm & operator= ( ElementFilteringImageAlgorithm & other )
      {
        _ealgo = other._ealgo->clone();
        ImageAlgorithm<T>::_algo.reset( _ealgo );
        return *this;
      }
    public:
      virtual ~ElementFilteringImageAlgorithm() {}

      //----------------------------------------------------------------------
      //   member
      //----------------------------------------------------------------------
    protected:
      SingleChannelImageAlgorithmType * _ealgo;
  };

  namespace singlechannel {

    //========================================================================
    //   LINEAR FILTERING IMAGE ALGORITHM: SINGLE CHANNEL
    //========================================================================
    template <typename T>
    class LinearFilteringImageAlgorithm: public ImageAlgorithmInterface<T>
    {
      public:
        //--------------------------------------------------------------------
        //   image algorithm interface: implementation
        //--------------------------------------------------------------------
        virtual void execute( const carto::VolumeRef<T> & in,
                                    carto::VolumeRef<T> & out ) const;

        virtual void setOptions( const carto::Object & options )
        {
          _func->setOptions( options );
        }

        virtual void updateOptions( const carto::Object & options )
        {
          _func->updateOptions( options );
        }

        virtual ::aims::singlechannel::LinearFilteringImageAlgorithm<T> * clone() const {
          return new ::aims::singlechannel::LinearFilteringImageAlgorithm<T>(*this);
        }

        //--------------------------------------------------------------------
        //   constructor (default+copy) & destructor
        //--------------------------------------------------------------------
      protected:
        LinearFilteringImageAlgorithm();

      public:
        LinearFilteringImageAlgorithm( const LinearFilteringFunction<T> & f ):
          _func(f.clone())
        {}

        LinearFilteringImageAlgorithm( const ::aims::singlechannel::LinearFilteringImageAlgorithm<T> & other ):
          _func(other._func->clone())
        {}

        LinearFilteringImageAlgorithm & operator= (
          const LinearFilteringImageAlgorithm & other )
        {
          assert( typeid(other) == typeid(*this) );
          if( this != &other )
            _func.reset( other._func->clone() );
          return *this;
        }

        virtual ~LinearFilteringImageAlgorithm() {}

        //--------------------------------------------------------------------
        //   member
        //--------------------------------------------------------------------
      protected:
        carto::rc_ptr<LinearFilteringFunction<T> > _func;
    };

    template <typename T>
    inline
    void LinearFilteringImageAlgorithm<T>::execute(
      const carto::VolumeRef<T> & in,
            carto::VolumeRef<T> & out ) const
    {
      int sx, sy, sz, ex, ey, ez, x, y, z, t;
      typename carto::Volume<T>::Position4Di pos(0,0,0,0), size(1,1,1,1);

      // set voxel size
      carto::Object vs = carto::Object::value( carto::PropertySet() );
      vs->setProperty( "voxel_size", in.header().getProperty( "voxel_size" ) );
      _func->updateOptions( vs );

      // Manage borders / structuring element size
      const std::vector<int> & border = in->getBorders();
      const std::vector<int> & amplitude = _func->getAmplitude();

      // When volume has borders, it is possible to use it to process filtering
      sz = amplitude[4] - border[4];
      sy = amplitude[2] - border[2];
      sx = amplitude[0] - border[0];
      ez = in->getSizeZ() - amplitude[5] + border[5];
      ey = in->getSizeY() - amplitude[3] + border[3];
      ex = in->getSizeX() - amplitude[1] + border[1];

      if( ImageAlgorithmInterface<T>::_verbose > 1 ) {
        std::cout << "Filter amplitude (voxels): [ ";
        for(int i = 0; i < 6; ++i)
          std::cout << carto::toString(amplitude[i]) << ( i==8 ? " " : ", ");
        std::cout << "]" << std::endl;
        std::cout << "Processing with borders (voxels): [ ";
        for(int i = 0; i < 8; ++i)
          std::cout << carto::toString(border[i]) << ( i==8 ? " " : ", ");
        std::cout << "]" << std::endl;
        std::cout << "Start: [" << carto::toString(sx) << ", "
                                << carto::toString(sy) << ", "
                                << carto::toString(sz) << "]" << std::endl;
        std::cout << "End: [" << carto::toString(ex) << ", "
                              << carto::toString(ey) << ", "
                              << carto::toString(ez) << "]" << std::endl;
      }

      size[0] = amplitude[0] + amplitude[1] + 1;
      size[1] = amplitude[2] + amplitude[2] + 1;
      size[2] = amplitude[4] + amplitude[5] + 1;
      carto::VolumeRef<T> win( new carto::Volume<T>( in, pos, size ) );

      aims::Progression progress( out->getSizeT() * (ez - sz) * (ey - sy) - 1 );
      if( ImageAlgorithmInterface<T>::_verbose > 0 )
        std::cout << "Filtering progress: ";

      for( t = 0; t < in->getSizeT(); ++t )
        for( z = sz; z < ez; ++z )
          for( y = sy; y < ey; ++y, ++progress ) {
            if( ImageAlgorithmInterface<T>::_verbose > 0 ) {
              // Display progression
              // The "normal" operator << should be as:
              // std::cout << progress << std::flush;
              // but it doesn't work in gcc 4.0, there is a namespace
              // confusion, so we have to specify ::operator <<
              ::operator << ( std::cout, progress ) << std::flush;
            }
            for( x = sx; x < ex; ++x )
            {
              // Set view position in the volume
              pos[0] = x - amplitude[0];
              pos[1] = y - amplitude[2];
              pos[2] = z - amplitude[4];
              pos[3] = t;
              win->setPosInRefVolume( pos );
              (*out)( x, y, z, t ) = _func->execute( win );
            }
          }

      if( ImageAlgorithmInterface<T>::_verbose > 0 )
        std::cout << std::endl;
    }


    //========================================================================
    //   ELEMENT FILTERING IMAGE ALGORITHM: SINGLE CHANNEL
    //========================================================================
    template <typename T>
    class ElementFilteringImageAlgorithm: public ImageAlgorithmInterface<T>
    {
      public:
        //--------------------------------------------------------------------
        //   image algorithm interface: implementation
        //--------------------------------------------------------------------
        virtual void execute( const carto::VolumeRef<T> & in,
                                    carto::VolumeRef<T> & out ) const;

        virtual void setOptions( const carto::Object & options )
        {
          _func->setOptions( options );
        }

        virtual void updateOptions( const carto::Object & options )
        {
          _func->updateOptions( options );
        }

        virtual ::aims::singlechannel::ElementFilteringImageAlgorithm<T> * clone() const {
          return new ::aims::singlechannel::ElementFilteringImageAlgorithm<T>(*this);
        }

        //--------------------------------------------------------------------
        //   member method
        //--------------------------------------------------------------------
        virtual void setStructuringElement( const StructuringElement & se )
        {
          _strel.reset( se.clone() );
        }

        //--------------------------------------------------------------------
        //   constructor (default+copy) & destructor
        //--------------------------------------------------------------------
      protected:
        ElementFilteringImageAlgorithm();

      public:
        ElementFilteringImageAlgorithm( const ElementFilteringFunction<T> & f,
                                        const StructuringElement & se = strel::Cube(1.) ):
          _func(f.clone()),
          _strel(se.clone())
        {}

        ElementFilteringImageAlgorithm( const ::aims::singlechannel::ElementFilteringImageAlgorithm<T> & other ):
          _func(other._func->clone()),
          _strel(other._strel->clone())
        {}

        ::aims::singlechannel::ElementFilteringImageAlgorithm<T> & operator=(
          const ::aims::singlechannel::ElementFilteringImageAlgorithm<T> & other )
        {
          assert( typeid(other) == typeid(*this) );
          if( *this != other )
          {
            _func.reset( other._func->clone() );
            _strel.reset( other._strel->clone() );
          }
          return *this;
        }

        virtual ~ElementFilteringImageAlgorithm() {}

        //--------------------------------------------------------------------
        //   members
        //--------------------------------------------------------------------
      protected:
        carto::rc_ptr<ElementFilteringFunction<T> > _func;
        carto::rc_ptr<StructuringElement>           _strel;
    };

    template <typename T>
    inline
    void ElementFilteringImageAlgorithm<T>::execute(
      const carto::VolumeRef<T> & in,
            carto::VolumeRef<T> & out
    ) const
    {
      int sx, sy, sz, ex, ey, ez, x, y, z, t;
      typename carto::Volume<T>::Position4Di pos(0,0,0), size(1,1,1);

      // Manage borders / structuring element size
      const std::vector<int> & border = in->getBorders();
      const std::vector<int> amplitude = _strel->getAmplitude();

      // When volume has borders, it is possible to use it to process filtering
      sz = amplitude[4] - border[4];
      sy = amplitude[2] - border[2];
      sx = amplitude[0] - border[0];
      ez = in->getSizeZ() - amplitude[5] + border[5];
      ey = in->getSizeY() - amplitude[3] + border[3];
      ex = in->getSizeX() - amplitude[1] + border[1];

      if( ImageAlgorithmInterface<T>::_verbose > 1 ) {
        std::cout << "Filter amplitude (voxels): [ ";
        for(int i = 0; i < 6; ++i)
          std::cout << carto::toString(amplitude[i]) << ( i==8 ? " " : ", ");
        std::cout << "]" << std::endl;
        std::cout << "Processing with borders (voxels): [ ";
        for(int i = 0; i < 8; ++i)
          std::cout << carto::toString(border[i]) << ( i==8 ? " " : ", ");
        std::cout << "]" << std::endl;
        std::cout << "Start: [" << carto::toString(sx) << ", "
                                << carto::toString(sy) << ", "
                                << carto::toString(sz) << "]" << std::endl;
        std::cout << "End: [" << carto::toString(ex) << ", "
                              << carto::toString(ey) << ", "
                              << carto::toString(ez) << "]" << std::endl;
      }

      carto::VolumeRef<T> win( new carto::Volume<T>( in, pos, size ) );

      aims::Progression progress( out->getSizeT() * (ez - sz) * (ey - sy) - 1 );
      if( ImageAlgorithmInterface<T>::_verbose > 0 )
        std::cout << "Filtering progress: ";

      for( t = 0; t < in->getSizeT(); ++t )
        for( z = sz; z < ez; ++z )
          for( y = sy; y < ey; ++y, ++progress ) {
            if( ImageAlgorithmInterface<T>::_verbose > 0 ) {
              // Display progression
              // The "normal" operator << should be as:
              // std::cout << progress << std::flush;
              // but it doesn't work in gcc 4.0, there is a namespace
              // confusion, so we have to specify ::operator <<
              ::operator << ( std::cout, progress ) << std::flush;
            }
            for( x = sx; x < ex; ++x )
            {
              // Set view position in the volume
              pos[0] = x;
              pos[1] = y;
              pos[2] = z;
              pos[3] = t;
              win->setPosInRefVolume( pos );
              (*out)( x, y, z, t ) = _func->execute( win, _strel );
            }
          }

      if( ImageAlgorithmInterface<T>::_verbose > 0 )
        std::cout << std::endl;
    }

  } // namespace singlechannel

} // namespace aims

#endif
