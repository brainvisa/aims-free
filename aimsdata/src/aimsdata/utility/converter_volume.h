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

#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <cartobase/type/converter.h>

namespace std
{

  template<typename _Tp>
  struct numeric_limits<AimsData<_Tp> > : public numeric_limits<_Tp>
  {
  public:
  };

}

namespace carto
{

  template <typename INP,typename OUTP>
  class RawConverter<AimsData<INP>, AimsData<OUTP> >
  {
  public:
    void convert( const AimsData<INP> &in, AimsData<OUTP> & out ) const;
  };


  template <typename INP,typename OUTP>
  class RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >
  {
  public:
    void convert( const carto::VolumeRef<INP> &in,
                  carto::VolumeRef<OUTP> & out ) const;
  };


  /// Low-level rescaling converter partial specialization
  template<typename INP, typename OUTP>
  class Rescaler<AimsData<INP>, AimsData<OUTP> >
  {
  public:
    Rescaler();
    Rescaler( const RescalerInfo & info );

    void convert( const AimsData<INP> &in, AimsData<OUTP> & out ) const;

  private:
    RescalerInfo _info;
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
  class RawConverter<AimsData<INP>, AimsData<INP> >
  {
  public:
    RawConverter( bool shallowcopy = false ) : _shallowcopy( shallowcopy ) {}
    void convert( const AimsData<INP> &in, AimsData<INP> & out ) const;

  private:
    bool _shallowcopy;
  };


  template <typename INP>
  class ShallowConverter<AimsData<INP>, AimsData<INP> >
    : public Converter<AimsData<INP>, AimsData<INP> >
  {
  public:
    ShallowConverter( bool rescale = false )
      : Converter<AimsData<INP>, AimsData<INP> >( rescale ) {}
    ShallowConverter( bool rescale, const RescalerInfo & info )
      : Converter<AimsData<INP>, AimsData<INP> >( rescale, info ) {}
    virtual void convert( const AimsData<INP> &in, AimsData<INP> & out ) const;
  };

  template <class INP,class OUTP>
  class ConverterAllocator<AimsData<INP>,AimsData<OUTP> >
  {
  public:
    static AimsData<OUTP>* alloc( const AimsData<INP> &in );
  };


  template <class INP,class OUTP>
  class ConverterAllocator<carto::VolumeRef<INP>,
    carto::VolumeRef<OUTP> >
  {
  public:
    static carto::VolumeRef<OUTP>* alloc( const carto::VolumeRef<INP> &in );
  };


  template <class INP>
  class ConverterAllocator<AimsData<INP>,AimsData<INP> >
  {
  public:
    static AimsData<INP>* alloc( const AimsData<INP> &in );
  };


  template <class INP>
  class ConverterAllocator<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
  {
  public:
    static carto::VolumeRef<INP>* alloc( const carto::VolumeRef<INP> &in );
  };


  // implementation

  template <class INP,class OUTP> inline
  AimsData<OUTP>* ConverterAllocator<AimsData<INP>,AimsData<OUTP> >::alloc
    ( const AimsData<INP> &in )
  {
    return new AimsData<OUTP>( in.dimX(), in.dimY(), in.dimZ(),
                               in.dimT(), in.borderWidth() );
  }


  template <class INP> inline
  AimsData<INP>* ConverterAllocator<AimsData<INP>,AimsData<INP> >::alloc
    ( const AimsData<INP> & )
  {
    return new AimsData<INP>( 0 );
  }


  // inline is a bit abusive here...
  template<class INP,class OUTP> inline
  void RawConverter<AimsData<INP>,AimsData<OUTP> >::convert
  ( const AimsData<INP> &in, AimsData<OUTP> &out ) const
  {
    out.setSizeX( in.sizeX() );
    out.setSizeY( in.sizeY() );
    out.setSizeZ( in.sizeZ() );
    out.setSizeT( in.sizeT() );

    if( in.header() )
      out.setHeader( in.header()->cloneHeader( true ) );

    int x, y, z, t, dx = out.dimX(), dy = out.dimY(), dz = out.dimZ(),
      dt = out.dimT(), ox = dx, oy = dy, oz = dz, ot = dt;
    if( in.dimX() < dx )
      dx = in.dimX();
    if( in.dimY() < dy )
      dy = in.dimY();
    if( in.dimZ() < dz )
      dz = in.dimZ();
    if( in.dimT() < dt )
      dt = in.dimT();

    RawConverter<INP,OUTP> itemconv;

    for( t=0; t<dt; ++t )
      {
        for( z=0; z<dz; ++z )
          {
            for( y=0; y<dy; ++y )
              {
                for( x=0; x<dx; ++x )
                  itemconv.convert( in( x, y, z, t ), out( x, y, z, t ) );
                for( ; x<ox; ++x )
                  out( x, y, z, t ) = OUTP(0);
              }
            for( ; y<oy; ++y )
              for( x=0; x<ox; ++x )
                out( x, y, z, t ) = OUTP(0);
          }
        for( ; z<oz; ++z )
          for( y=0; y<oy; ++y )
            for( x=0; x<ox; ++x )
              out( x, y, z, t ) = OUTP(0);
      }

    for( ; t<ot; ++t )
      for( z=0; z<oz; ++z )
        for( y=0; y<oy; ++y )
          for( x=0; x<ox; ++x )
            out( x, y, z, t ) = OUTP(0);

    aims::PythonHeader  *ph
        = dynamic_cast<aims::PythonHeader *>( out.header() );
    if( ph )
    {
      ph->setProperty( "data_type", DataTypeCode<OUTP>::dataType() );
      if( ph->hasProperty( "disk_data_type" ) )
        ph->removeProperty( "disk_data_type" );
    }
  }


  // inline is a bit abusive here...
  template<class INP,class OUTP> inline
  void Rescaler<AimsData<INP>,AimsData<OUTP> >::convert
  ( const AimsData<INP> &in, AimsData<OUTP> &out ) const
  {
    // Accommodate the case where out is allocated with zero size by
    // AllocatorConverter.  This happens when the INP and OUTP types are equal.
    if( out.dimX() == 0 )
      out = AimsData<OUTP>( in.dimX(), in.dimY(), in.dimZ(),
                            in.dimT(), in.borderWidth() );

    out.setSizeX( in.sizeX() );
    out.setSizeY( in.sizeY() );
    out.setSizeZ( in.sizeZ() );
    out.setSizeT( in.sizeT() );

    if( in.header() )
      out.setHeader( in.header()->cloneHeader( true ) );

    RescalerInfo info( _info );
    if( std::isnan( info.vmin ) ) {
      if ( ! info.usevtypelimits ) {
        info.vmin = (double) in.minimum();
      }
    }

    if( std::isnan( info.vmax ) ) {
      if ( ! info.usevtypelimits ) {
        info.vmax = (double) in.maximum();
      }
    }

    DefaultedRescalerInfo<INP, OUTP> defaultedinfo( info );

    int x, y, z, t, dx = out.dimX(), dy = out.dimY(), dz = out.dimZ(),
      dt = out.dimT(), ox = dx, oy = dy, oz = dz, ot = dt;
    if( in.dimX() < dx )
      dx = in.dimX();
    if( in.dimY() < dy )
      dy = in.dimY();
    if( in.dimZ() < dz )
      dz = in.dimZ();
    if( in.dimT() < dt )
      dt = in.dimT();

    for( t=0; t<dt; ++t )
      {
        for( z=0; z<dz; ++z )
          {
            for( y=0; y<dy; ++y )
              {
                for( x=0; x<dx; ++x )
                  out( x, y, z, t ) =
                    defaultedinfo.getScaledValue( in( x, y, z, t ) );
                for( ; x<ox; ++x )
                  out( x, y, z, t ) = OUTP(0);
              }
            for( ; y<oy; ++y )
              for( x=0; x<ox; ++x )
                out( x, y, z, t ) = OUTP(0);
          }
        for( ; z<oz; ++z )
          for( y=0; y<oy; ++y )
            for( x=0; x<ox; ++x )
              out( x, y, z, t ) = OUTP(0);
      }

    for( ; t<ot; ++t )
      for( z=0; z<oz; ++z )
        for( y=0; y<oy; ++y )
          for( x=0; x<ox; ++x )
            out( x, y, z, t ) = OUTP(0);

    float scf = 1.;
    aims::PythonHeader
      *h = dynamic_cast<aims::PythonHeader *>( out.header() );
    if( !h )
      h = new aims::PythonHeader;
    h->getProperty( "scale_factor", scf );
    scf *= defaultedinfo.getScale();
    h->setProperty( "scale_factor", scf );
    h->setProperty( "data_type", DataTypeCode<OUTP>::dataType() );
    if( h->hasProperty( "disk_data_type" ) )
      h->removeProperty( "disk_data_type" );
  }


  template<class INP> inline
  void RawConverter<AimsData<INP>,AimsData<INP> >::convert
  ( const AimsData<INP> &in, AimsData<INP> &out ) const
  {
    if( out.dimX() == 0
        || ( in.dimX() == out.dimX() && in.dimY() == out.dimY()
             && in.dimZ() == out.dimZ() && in.dimT() == out.dimT()
             && in.borderWidth() == out.borderWidth() ) )
    {
      if( this->_shallowcopy )
        out = in;
      else
      {
        // using deep-copy to be consistent with other cases
        out = in.clone();
        // keep the same UUID
        out.volume()->copyUuid( *in.volume() );
      }
    }
    else
      {
        if( in.header() )
          out.setHeader( in.header()->cloneHeader( true ) );

        out.setSizeXYZT( in.sizeX(), in.sizeY(), in.sizeZ(), in.sizeT() );
        int     x, y, z, t, dx = out.dimX(), dy = out.dimY(), dz = out.dimZ(),
          dt = out.dimT(), ox = dx, oy = dy, oz = dz, ot = dt;
        if( in.dimX() < dx )
          dx = in.dimX();
        if( in.dimY() < dy )
          dy = in.dimY();
        if( in.dimZ() < dz )
          dz = in.dimZ();
        if( in.dimT() < dt )
          dt = in.dimT();

        for( t=0; t<dt; ++t )
          {
            for( z=0; z<dz; ++z )
              {
                for( y=0; y<dy; ++y )
                  {
                    for( x=0; x<dx; ++x )
                      out( x, y, z, t ) = in( x, y, z, t );
                    for( ; x<ox; ++x )
                      out( x, y, z, t ) = INP(0);
                  }
                for( ; y<oy; ++y )
                  for( x=0; x<ox; ++x )
                    out( x, y, z, t ) = INP(0);
              }
            for( ; z<oz; ++z )
              for( y=0; y<oy; ++y )
                for( x=0; x<ox; ++x )
                  out( x, y, z, t ) = INP(0);
          }

        for( ; t<ot; ++t )
          for( z=0; z<oz; ++z )
            for( y=0; y<oy; ++y )
              for( x=0; x<ox; ++x )
                out( x, y, z, t ) = INP(0);
      }
  }

  template<typename INP, typename OUTP>
  inline
  Rescaler<AimsData<INP>, AimsData<OUTP> >::Rescaler() : _info()
  {
  }

  template<typename INP, typename OUTP>
  inline
  Rescaler<AimsData<INP>, AimsData<OUTP> >::Rescaler(const RescalerInfo & info)
  : _info(info)
  {
  }

  template<typename INP>
  class ConverterSwitch<AimsData<INP>,AimsData<INP>,false>
    : public RawConverter<AimsData<INP>,AimsData<INP> >
  {
  public:
    ConverterSwitch( bool shallowcopy = false )
      : RawConverter<AimsData<INP>,AimsData<INP> >( shallowcopy ) {}
    ConverterSwitch( const RescalerInfo&,  bool shallowcopy = false )
      : RawConverter<AimsData<INP>,AimsData<INP> >( shallowcopy ) {}
  };


  template<typename INP>
  class ConverterSwitch<AimsData<INP>,AimsData<INP>,true>
    : public Rescaler<AimsData<INP>,AimsData<INP> >
  {
  public:
    ConverterSwitch( bool = false )
      : Rescaler<AimsData<INP>,AimsData<INP> >() {}
    ConverterSwitch( const RescalerInfo& info, bool )
      : Rescaler<AimsData<INP>,AimsData<INP> >(info) {}
  };


  template<typename INP>
  class SmartConverter<AimsData<INP>, AimsData<INP> >
    : public ConverterSwitch<AimsData<INP>, AimsData<INP>,
                             std::numeric_limits<INP>::is_specialized
  && std::numeric_limits<INP>::is_bounded >
  {
  public:
    SmartConverter( bool shallowcopy = false )
      : ConverterSwitch<AimsData<INP>, AimsData<INP>,
          std::numeric_limits<INP>::is_specialized
          && std::numeric_limits<INP>::is_bounded >( shallowcopy ) {}
    SmartConverter( const RescalerInfo& info, bool shallowcopy = false )
      : ConverterSwitch<AimsData<INP>, AimsData<INP>,
          std::numeric_limits<INP>::is_specialized
          && std::numeric_limits<INP>::is_bounded >( info, shallowcopy ) {}
  };


  template<class INP>
  inline void
  ShallowConverter<AimsData<INP>, AimsData<INP> >::convert
  ( const AimsData<INP> &in, AimsData<INP> & out ) const
  {
    if( this->_rescale )
      {
        SmartConverter<AimsData<INP>,AimsData<INP> > sc( this->_info, true );
        sc.convert( in, out  );
      }
    else
      {
        RawConverter<AimsData<INP>,AimsData<INP> > rc( true );
        rc.convert( in, out );
      }
  }


  // VolumeRef implementation

  template <typename INP,typename OUTP>
  void RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::convert
    ( const carto::VolumeRef<INP> &in, carto::VolumeRef<OUTP> & out ) const
  {
    AimsData<OUTP> data( out );
    RawConverter<AimsData<INP>, AimsData<OUTP> > c;
    c.convert( in, data );
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


  template<typename INP, typename OUTP>
  void Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >::convert
      ( const carto::VolumeRef<INP> &in, carto::VolumeRef<OUTP> &out ) const
  {
    AimsData<OUTP> data( out );
    Rescaler<AimsData<INP>,AimsData<OUTP> > c( _info );
    c.convert( in, data );
  }

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

}

#endif
