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
#ifndef AIMS_UTILITY_CONVERTER_AIMSDATA_H
#define AIMS_UTILITY_CONVERTER_AIMSDATA_H

#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <aims/utility/converter_volume.h>

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
  class ConverterAllocator<AimsData<INP>, AimsData<OUTP> >
  {
  public:
    static AimsData<OUTP>* alloc( const AimsData<INP> &in );
  };


  template <class INP>
  class ConverterAllocator<AimsData<INP>, AimsData<INP> >
  {
  public:
    static AimsData<INP>* alloc( const AimsData<INP> &in );
  };


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


  template<class INP,class OUTP> inline
  void RawConverter<AimsData<INP>,AimsData<OUTP> >::convert
  ( const AimsData<INP> &in, AimsData<OUTP> &out ) const
  {
    RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> > c;
    c.convert( in.volume(), out.volume() );
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

  template<class INP,class OUTP> inline
  void Rescaler<AimsData<INP>,AimsData<OUTP> >::convert
  ( const AimsData<INP> &in, AimsData<OUTP> &out ) const
  {
    Rescaler<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> > c( _info );
    c.convert( in.volume(), out.volume() );
  }


  template<class INP> inline
  void RawConverter<AimsData<INP>,AimsData<INP> >::convert
  ( const AimsData<INP> &in, AimsData<INP> &out ) const
  {
    RawConverter<carto::VolumeRef<INP>, carto::VolumeRef<INP> >
      conv( _shallowcopy );
    conv.convert( in.volume(), out.volume() );
  }

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

}

#endif
