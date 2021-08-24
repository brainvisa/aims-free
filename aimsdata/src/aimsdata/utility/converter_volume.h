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

/*
 *  Converter between the different A.I.M.S. types
 */
#ifndef AIMS_UTILITY_CONVERTER_VOLUME_H
#define AIMS_UTILITY_CONVERTER_VOLUME_H

#include <cartodata/volume/volume.h>
#include <cartobase/type/converter.h>
#include <cartobase/containers/nditerator.h>

namespace std
{

  template<typename _Tp>
  struct numeric_limits<carto::Volume<_Tp> > : public numeric_limits<_Tp>
  {
  public:
  };


  template<typename _Tp>
  struct numeric_limits<carto::VolumeRef<_Tp> > : public numeric_limits<_Tp>
  {
  public:
  };

}

namespace carto
{

  template <typename INP,typename OUTP>
  class RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >
  {
  public:
    void convert( const carto::VolumeRef<INP> &in,
                  carto::VolumeRef<OUTP> & out ) const;
  };


  template<typename INP, typename OUTP>
  class Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >
  {
  public:
    Rescaler();
    Rescaler( const RescalerInfo & info );

    void convert( const carto::VolumeRef<INP> &in,
                  carto::VolumeRef<OUTP> & out ) const;

  private:
    RescalerInfo _info;
  };


  // special case of same input and output types

  template <typename INP>
  class RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
  {
  public:
    RawConverter( bool shallowcopy = false ) : _shallowcopy( shallowcopy ) {}
    void convert( const carto::VolumeRef<INP> &in, carto::VolumeRef<INP> & out ) const;

  private:
    bool _shallowcopy;
  };


  template <typename INP>
  class ShallowConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
    : public Converter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
  {
  public:
    ShallowConverter( bool rescale = false )
      : Converter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >( rescale ) {}
    ShallowConverter( bool rescale, const RescalerInfo & info )
      : Converter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >( rescale, info ) {}
    virtual void convert( const carto::VolumeRef<INP> &in, carto::VolumeRef<INP> & out ) const;
  };


  template <class INP,class OUTP>
  class ConverterAllocator<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >
  {
  public:
    static carto::VolumeRef<OUTP>* alloc( const carto::VolumeRef<INP> &in );
  };


  template <class INP>
  class ConverterAllocator<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
  {
  public:
    static carto::VolumeRef<INP>* alloc( const carto::VolumeRef<INP> &in );
  };


  template<typename INP>
  class ConverterSwitch<carto::VolumeRef<INP>, carto::VolumeRef<INP>,false>
    : public RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
  {
  public:
    ConverterSwitch( bool shallowcopy = false )
      : RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >(
          shallowcopy ) {}
    ConverterSwitch( const RescalerInfo&,  bool shallowcopy = false )
      : RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >(
          shallowcopy ) {}
  };


  template<typename INP>
  class ConverterSwitch<carto::VolumeRef<INP>, carto::VolumeRef<INP>,true>
    : public Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
  {
  public:
    ConverterSwitch( bool = false )
      : Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<INP> >() {}
    ConverterSwitch( const RescalerInfo& info, bool )
      : Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<INP> >(info) {}
  };


  template<typename INP>
  class SmartConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
    : public ConverterSwitch<carto::VolumeRef<INP>, carto::VolumeRef<INP>,
                             std::numeric_limits<INP>::is_specialized
  && std::numeric_limits<INP>::is_bounded >
  {
  public:
    SmartConverter( bool shallowcopy = false )
      : ConverterSwitch<carto::VolumeRef<INP>, carto::VolumeRef<INP>,
          std::numeric_limits<INP>::is_specialized
          && std::numeric_limits<INP>::is_bounded >( shallowcopy ) {}
    SmartConverter( const RescalerInfo& info, bool shallowcopy = false )
      : ConverterSwitch<carto::VolumeRef<INP>, carto::VolumeRef<INP>,
          std::numeric_limits<INP>::is_specialized
          && std::numeric_limits<INP>::is_bounded >( info, shallowcopy ) {}
  };


  // implementation

  template <class INP,class OUTP>
  carto::VolumeRef<OUTP>*
  ConverterAllocator<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::alloc
      ( const carto::VolumeRef<INP> &in )
  {
    return new VolumeRef<OUTP>( in.getSizeX(), in.getSizeY(), in.getSizeZ(),
                                in.getSizeT() );
  }


  template <class INP> inline
  carto::VolumeRef<INP>*
  ConverterAllocator<carto::VolumeRef<INP>, carto::VolumeRef<INP> >::alloc
      ( const carto::VolumeRef<INP> & )
  {
    return new carto::VolumeRef<INP>( 0 );
  }


  // inline is a bit abusive here...
  template <typename INP, typename OUTP>
  void RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::convert
    ( const carto::VolumeRef<INP> &in, carto::VolumeRef<OUTP> & out ) const
  {
    out.setVoxelSize( in.getVoxelSize() );
    out.copyHeaderFrom( in.header() );

    std::vector<int> isz = in.getSize();
    std::vector<int> osz = out.getSize();
    bool erased = false;

    int i, n = isz.size(), m = osz.size();
    for( i=0; i<std::min(n, m); ++i )
      if( isz[i] < osz[i] && !erased )
      {
        out.fill( OUTP( 0 ) );
        erased = true;
      }
      else if( isz[i] > osz[i] )
        throw std::runtime_error(
          "Converter output volume should be as large as input" );
    if( !erased )
      for( ; i<n; ++i )  // input is larger in more dimensions
        if( isz[i] > 1 )
        {
          out.fill( OUTP( 0 ) );
          break;
        }
    for( ; i<m; ++i )
      if( osz[i] > 1 )  // output is larger in more dimensions
        throw std::runtime_error(
          "Converter output volume should be as large as input" );

    RawConverter<INP,OUTP> itemconv;

    carto::const_line_NDIterator<INP> it( &in.at( 0 ), in.getSize(),
                                          in.getStrides() );
    carto::line_NDIterator<OUTP> oit( &out.at( 0 ), out.getSize(),
                                      out.getStrides() );
    const INP *p, *pp;
    OUTP *op;

    for( ; !it.ended(); ++it, ++oit )
    {
      p = &*it;
      op = &out->at( it.position() );
      for( pp=p + it.line_length(); p!=pp;
           it.inc_line_ptr( p ), oit.inc_line_ptr( op ) )
        itemconv.convert( *p, *op );
    }

    out.header().setProperty( "data_type", DataTypeCode<OUTP>::dataType() );
    if( out.header().hasProperty( "disk_data_type" ) )
        out.header().removeProperty( "disk_data_type" );
  }


  template<typename INP, typename OUTP>
  Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::Rescaler()
  : _info()
  {
  }


  template<typename INP, typename OUTP>
  Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::Rescaler
      ( const RescalerInfo & info )
  : _info( info )
  {
  }


  // inline is a bit abusive here...
  template<typename INP, typename OUTP>
  void Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::convert
      ( const carto::VolumeRef<INP> &in, carto::VolumeRef<OUTP> &out ) const
  {
    // Accommodate the case where out is allocated with zero size by
    // AllocatorConverter.  This happens when the INP and OUTP types are equal.
    if( out.getSizeX() == 0 )
      out.reset( new Volume<OUTP>( in.getSize(), in.getBorders(),
                                   out.allocatorContext() ) );

    out.setVoxelSize( in.getVoxelSize() );
    out.copyHeaderFrom( in.header() );

    RescalerInfo info( _info );
    if( std::isnan( info.vmin ) )
    {
      if ( ! info.usevtypelimits )
      {
        info.vmin = (double) in.min();
      }
    }

    if( std::isnan( info.vmax ) )
    {
      if ( ! info.usevtypelimits )
      {
        info.vmax = (double) in.max();
      }
    }

    std::vector<int> isz = in.getSize();
    std::vector<int> osz = out.getSize();
    bool erased = false;

    int i, n = isz.size(), m = osz.size();
    for( i=0; i<std::min(n, m); ++i )
      if( isz[i] < osz[i] && !erased )
      {
        out.fill( OUTP( 0 ) );
        erased = true;
      }
      else if( isz[i] > osz[i] )
        throw std::runtime_error(
          "Converter output volume should be as large as input" );
    if( !erased )
      for( ; i<n; ++i )  // input is larger in more dimensions
        if( isz[i] > 1 )
        {
          out.fill( OUTP( 0 ) );
          break;
        }
    for( ; i<m; ++i )
      if( osz[i] > 1 )  // output is larger in more dimensions
        throw std::runtime_error(
          "Converter output volume should be as large as input" );

    DefaultedRescalerInfo<INP, OUTP> defaultedinfo( info );

    carto::const_line_NDIterator<INP> it( &in.at( 0 ), in.getSize(),
                                          in.getStrides() );
    carto::line_NDIterator<OUTP> oit( &out.at( 0 ), out.getSize(),
                                      out.getStrides() );
    const INP *p, *pp;
    OUTP *op;

    for( ; !it.ended(); ++it, ++oit )
    {
      p = &*it;
      op = &out->at( it.position() );
      for( pp=p + it.line_length(); p!=pp;
           it.inc_line_ptr( p ), oit.inc_line_ptr( op ) )
        *op = defaultedinfo.getScaledValue( *p );
    }

    float scf = 1.;
    out.header().getProperty( "scale_factor", scf );
    scf *= defaultedinfo.getScale();
    out.header().setProperty( "scale_factor", scf );
    out.header().setProperty( "data_type", DataTypeCode<OUTP>::dataType() );
    if( out.header().hasProperty( "disk_data_type" ) )
      out.header().removeProperty( "disk_data_type" );
  }

  template<class INP> inline
  void RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >::convert
  ( const carto::VolumeRef<INP> &in, carto::VolumeRef<INP> &out ) const
  {
    if( out.getSizeX() == 0
        || ( in.getSize() == out.getSize()
             && in.getBorders() == out.getBorders() ) )
    {
      if( this->_shallowcopy )
        out = in;
      else
      {
        // using deep-copy to be consistent with other cases
        out = in.deepcopy();
        // keep the same UUID
        out->copyUuid( *in );
      }
    }
    else
    {
      out.copyHeaderFrom( in.header() );
      out.setVoxelSize( in.getVoxelSize() );

      std::vector<int> isz = in.getSize();
      std::vector<int> osz = out.getSize();
      bool erased = false;

      int i, n = isz.size(), m = osz.size();
      for( i=0; i<std::min(n, m); ++i )
        if( isz[i] < osz[i] && !erased )
        {
          out.fill( INP( 0 ) );
          erased = true;
        }
        else if( isz[i] > osz[i] )
          throw std::runtime_error(
            "Converter output volume should be as large as input" );
      if( !erased )
        for( ; i<n; ++i )  // input is larger in more dimensions
          if( isz[i] > 1 )
          {
            out.fill( INP( 0 ) );
            break;
          }
      for( ; i<m; ++i )
        if( osz[i] > 1 )  // output is larger in more dimensions
          throw std::runtime_error(
            "Converter output volume should be as large as input" );

      carto::const_line_NDIterator<INP> it( &in.at( 0 ), in.getSize(),
                                            in.getStrides() );
      carto::line_NDIterator<INP> oit( &out.at( 0 ), out.getSize(),
                                       out.getStrides() );
      const INP *p, *pp;
      INP *op;

      for( ; !it.ended(); ++it, ++oit )
      {
        p = &*it;
        op = &out->at( it.position() );
        for( pp=p + it.line_length(); p!=pp;
            it.inc_line_ptr( p ), oit.inc_line_ptr( op ) )
          *op = *p;
      }
    }
  }

  template<class INP>
  inline void
  ShallowConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >::convert
  ( const carto::VolumeRef<INP> &in, carto::VolumeRef<INP> & out ) const
  {
    if( this->_rescale )
    {
      SmartConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
        sc( this->_info, true );
      sc.convert( in, out );
    }
    else
    {
      RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> > rc( true );
        rc.convert( in, out );
    }
  }

}


// RGB conversion
#include <aims/rgb/rgb.h>

namespace carto
{

  template <typename T, int D>
  class Rescaler<VolumeRef<AimsVector<T,D> >, VolumeRef<AimsRGB> >
  {
  public:
    Rescaler();
    Rescaler( const RescalerInfo & info );

    void convert( const VolumeRef<AimsVector<T,D> > &in, VolumeRef<AimsRGB> & out ) const;

  private:
    RescalerInfo _info;
  };

  template <typename T, int D>
  inline
  Rescaler<VolumeRef<AimsVector<T,D> >, VolumeRef<AimsRGB> >::Rescaler():
    _info()
  {}

  template <typename T, int D>
  inline
  Rescaler<VolumeRef<AimsVector<T,D> >, VolumeRef<AimsRGB> >::Rescaler(
    const RescalerInfo & info ):
    _info(info)
  {}

  template <typename T, int D>
  void
  Rescaler<VolumeRef<AimsVector<T,D> >, VolumeRef<AimsRGB> >::convert(
    const VolumeRef<AimsVector<T,D> > &in, VolumeRef<AimsRGB> &out ) const
  {
    if( out.getSizeX() == 0 )
      out.reset( new Volume<AimsRGB>( in.getSize(), in.getBorders(),
                                      out.allocatorContext() ) );

    out.setVoxelSize( in.getVoxelSize() );
    out.copyHeaderFrom( in.header() );

    std::vector<int> isz = in.getSize();
    std::vector<int> osz = out.getSize();
    bool erased = false;

    int i, n = isz.size(), m = osz.size();
    for( i=0; i<std::min(n, m); ++i )
      if( isz[i] < osz[i] && !erased )
      {
        out.fill( AimsRGB( 0 ) );
        erased = true;
      }
      else if( isz[i] > osz[i] )
        throw std::runtime_error(
          "Converter output volume should be as large as input" );
    if( !erased )
      for( ; i<n; ++i )  // input is larger in more dimensions
        if( isz[i] > 1 )
        {
          out.fill( AimsRGB( 0 ) );
          break;
        }
    for( ; i<m; ++i )
      if( osz[i] > 1 )  // output is larger in more dimensions
        throw std::runtime_error(
          "Converter output volume should be as large as input" );

    carto::const_line_NDIterator<AimsVector<T,D> >
      it( &in.at( 0 ), in.getSize(), in.getStrides() );
    const AimsVector<T,D> *p, *pp;

    RescalerInfo info( _info );
    if( std::isnan( info.vmin ) )
    {
      if ( ! info.usevtypelimits )
      {
        info.vmin = std::numeric_limits<double>::max();
        for( ; !it.ended(); ++it )
        {
          p = &*it;
          for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
            {
              for( int c = 0; c < D; ++c )
              if( (double) (*p)[c] < info.vmin )
                info.vmin = (double) (*p)[c];
            }
        }
      }
    }

    if( std::isnan( info.vmax ) )
    {
      if ( ! info.usevtypelimits )
      {
        info.vmax = std::numeric_limits<double>::min();
        for( it.reset(); !it.ended(); ++it )
        {
          p = &*it;
          for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
            {
              for( int c = 0; c < D; ++c )
              if( (double) (*p)[c] > info.vmin )
                info.vmax = (double) (*p)[c];
            }
        }
      }
    }

    DefaultedRescalerInfo<T, uint8_t> defaultedinfo( info );

    carto::line_NDIterator<AimsRGB> oit( &out.at( 0 ), out.getSize(),
                                         out.getStrides() );
    AimsRGB *op;

    for( it.reset(); !it.ended(); ++it, ++oit )
    {
      p = &*it;
      op = &out->at( it.position() );
      for( pp=p + it.line_length(); p!=pp;
           it.inc_line_ptr( p ), oit.inc_line_ptr( op ) )
        for( int c=0; c < D && c<3; ++c )
          (*op)[c] = defaultedinfo.getScaledValue( (*p)[c] );
    }

    float scf = 1.;
    out.header().getProperty( "scale_factor", scf );
    scf *= defaultedinfo.getScale();
    out.header().setProperty( "scale_factor", scf );
    out.header().setProperty( "data_type", DataTypeCode<AimsRGB>::name() );
    if( out.header().hasProperty( "disk_data_type" ) )
      out.header().removeProperty( "disk_data_type" );
  }

  template <typename T, int D>
  class Rescaler<VolumeRef<AimsRGB>, VolumeRef<AimsVector<T,D> > >
  {
  public:
    Rescaler();
    Rescaler( const RescalerInfo & info );

    void convert( const VolumeRef<AimsRGB> &in, VolumeRef<AimsVector<T,D> > & out ) const;

  private:
    RescalerInfo _info;
  };

  template <typename T, int D>
  inline
  Rescaler<VolumeRef<AimsRGB>, VolumeRef<AimsVector<T,D> > >::Rescaler():
    _info()
  {}

  template <typename T, int D>
  inline
  Rescaler<VolumeRef<AimsRGB>, VolumeRef<AimsVector<T,D> > >::Rescaler(
    const RescalerInfo & info ):
    _info(info)
  {}

  template <typename T, int D>
  void
  Rescaler<VolumeRef<AimsRGB>, VolumeRef<AimsVector<T,D> > >::convert(
    const VolumeRef<AimsRGB> &in, VolumeRef<AimsVector<T,D> > &out ) const
  {
    if( out.getSizeX() == 0 )
      out.reset( new Volume<AimsVector<T,D> >(
        in.getSize(), in.getBorders(), out.allocatorContext() ) );

    out.setVoxelSize( in.getVoxelSize() );
    out.copyHeaderFrom( in.header() );

    std::vector<int> isz = in.getSize();
    std::vector<int> osz = out.getSize();
    bool erased = false;

    int i, n = isz.size(), m = osz.size();
    for( i=0; i<std::min(n, m); ++i )
      if( isz[i] < osz[i] && !erased )
      {
        out.fill( AimsVector<T,D>( 0 ) );
        erased = true;
      }
      else if( isz[i] > osz[i] )
        throw std::runtime_error(
          "Converter output volume should be as large as input" );
    if( !erased )
      for( ; i<n; ++i )  // input is larger in more dimensions
        if( isz[i] > 1 )
        {
          out.fill( AimsVector<T,D>( 0 ) );
          break;
        }
    for( ; i<m; ++i )
      if( osz[i] > 1 )  // output is larger in more dimensions
        throw std::runtime_error(
          "Converter output volume should be as large as input" );

    carto::const_line_NDIterator<AimsRGB>
      it( &in.at( 0 ), in.getSize(), in.getStrides() );
    const AimsRGB *p, *pp;

    RescalerInfo info( _info );
    if( std::isnan( info.vmin ) )
    {
      if ( ! info.usevtypelimits )
      {
        info.vmin = std::numeric_limits<double>::max();
        for( ; !it.ended(); ++it )
        {
          p = &*it;
          for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
            {
              for( int c = 0; c < 3; ++c )
              if( (double) (*p)[c] < info.vmin )
                info.vmin = (double) (*p)[c];
            }
        }
      }
    }

    if( std::isnan( info.vmax ) )
    {
      if ( ! info.usevtypelimits )
      {
        info.vmax = std::numeric_limits<double>::min();
        for( it.reset(); !it.ended(); ++it )
        {
          p = &*it;
          for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
            {
              for( int c = 0; c < 3; ++c )
              if( (double) (*p)[c] > info.vmin )
                info.vmax = (double) (*p)[c];
            }
        }
      }
    }

    DefaultedRescalerInfo<uint8_t, T> defaultedinfo( info );

    carto::line_NDIterator<AimsVector<T,D> > oit( &out.at( 0 ), out.getSize(),
                                                  out.getStrides() );
    AimsVector<T,D> *op;

    for( it.reset(); !it.ended(); ++it, ++oit )
    {
      p = &*it;
      op = &out->at( it.position() );
      for( pp=p + it.line_length(); p!=pp;
           it.inc_line_ptr( p ), oit.inc_line_ptr( op ) )
        for( int c=0; c < D && c<3; ++c )
          (*op)[c] = defaultedinfo.getScaledValue( (*p)[c] );
    }

    float scf = 1.;
    out.header().getProperty( "scale_factor", scf );
    scf *= defaultedinfo.getScale();
    out.header().setProperty( "scale_factor", scf );
    out.header().setProperty( "data_type",
                              DataTypeCode<AimsVector<T, D> >::name() );
    if( out.header().hasProperty( "disk_data_type" ) )
      out.header().removeProperty( "disk_data_type" );
  }

}


// maintain header compatibility for now (temporary)
#include <aims/utility/converter_aimsdata.h>

#endif
