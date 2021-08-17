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
#ifndef AIMS_UTILITY_CONVERTER_BUCKET_H
#define AIMS_UTILITY_CONVERTER_BUCKET_H

#include <cartobase/type/converter.h>
#include <aims/bucket/bucket.h>
#include <aims/data/data.h>

namespace carto
{

  /** Partial specialization of Converter between buckets of different types.
  */
  template <class INP,class OUTP>
  class RawConverter<aims::BucketMap<INP>, aims::BucketMap<OUTP> >
  {
  public :
    void convert( const aims::BucketMap<INP> &in, 
                  aims::BucketMap<OUTP> & out ) const;
  };


  /** Partial specialization of Converter between buckets and volumes.
  */
  template <class INP,class OUTP>
  class RawConverter<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >
  {
  public :
    void convert( const aims::BucketMap<INP> &in,
      rc_ptr<Volume<OUTP> > & out ) const;
    /// writes bucket in an already allocated data
    void printToVolume( const aims::BucketMap<INP> &in,
                        rc_ptr<Volume<OUTP> > & out,
                        const Point3d & offset = Point3d( 0, 0, 0 ) ) const;
  };


  /** Partial specialization of Converter between buckets and volumes.
  */
  template <class INP,class OUTP>
  class RawConverter<aims::BucketMap<INP>, VolumeRef<OUTP> >
    : public RawConverter<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >
  {
  public :
    void convert( const aims::BucketMap<INP> &in,
      VolumeRef<OUTP> & out ) const
    {
      RawConverter<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >::convert(
        in, out );
    }
    /// writes bucket in an already allocated data
    void printToVolume( const aims::BucketMap<INP> &in,
                        VolumeRef<OUTP> & out,
                        const Point3d & offset = Point3d( 0, 0, 0 ) ) const
    {
      RawConverter<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >
        ::printToVolume( in, out, offset );
    }
  };


  /** Partial specialization of Converter between void buckets and volumes.
  */
  template <class OUTP>
  class RawConverter<aims::BucketMap<Void>, rc_ptr<Volume<OUTP> > >
  {
  public :
    RawConverter( bool timeislabel = false, bool withConstantValue = false,
      OUTP value = 0 )
      : _timeIsLabel( timeislabel ), _hasValue( withConstantValue ),
        _value( value ) {}
    void convert( const aims::BucketMap<Void> &in,
                  rc_ptr<Volume<OUTP> > & out ) const;
    void printToVolume( const aims::BucketMap<Void> &in,
                        rc_ptr<Volume<OUTP> > & out,
                        const Point3d & offset = Point3d( 0, 0, 0 ) ) const;
    void setTimeIsLabel( bool x ) { _timeIsLabel = x; }
    bool timeIsLabel() const { return( _timeIsLabel ); }

  private:
    bool	_timeIsLabel;
    bool        _hasValue;
    OUTP        _value;
  };


  /** Partial specialization of Converter between void buckets and volumes.
  */
  template <class OUTP>
  class RawConverter<aims::BucketMap<Void>, VolumeRef<OUTP> >
    : public RawConverter<aims::BucketMap<Void>, rc_ptr<Volume<OUTP> > >
  {
  public :
    void convert( const aims::BucketMap<Void> &in,
                  VolumeRef<OUTP> & out ) const
    {
      RawConverter<aims::BucketMap<Void>, rc_ptr<Volume<OUTP> > >::convert(
        in, out );
    }
    void printToVolume( const aims::BucketMap<Void> &in,
                        VolumeRef<OUTP> & out,
                        const Point3d & offset = Point3d( 0, 0, 0 ) ) const
    {
      RawConverter<aims::BucketMap<Void>, rc_ptr<Volume<OUTP> > >
        ::printToVolume( in, out, offset );
    }
  };


  /** Partial specialization of Converter between volumes and buckets.
  */
  template <class INP,class OUTP>
  class RawConverter<rc_ptr<Volume<INP> >, aims::BucketMap<OUTP> >
  {
  public:
    void convert( const rc_ptr<Volume<INP> > &in,
                  aims::BucketMap<OUTP> & out ) const;
  };


  /** Partial specialization of Converter between volumes and buckets.
  */
  template <class INP,class OUTP>
  class RawConverter<VolumeRef<INP>, aims::BucketMap<OUTP> >
    : public RawConverter<rc_ptr<Volume<INP> >, aims::BucketMap<OUTP> >
  {
  public:
//     void convert( const VolumeRef<INP> &in,
//                   aims::BucketMap<OUTP> & out ) const;
  };


  /** Partial specialization of Converter between volumes and void buckets.
  */
  template <class INP>
  class RawConverter<rc_ptr<Volume<INP> >, aims::BucketMap<Void> >
  {
  public:
    RawConverter( bool timeislabel = false ) 
      : _timeIsLabel( timeislabel ) {}

    void convert( const rc_ptr<Volume<INP> > &in,
                  aims::BucketMap<Void> & out ) const;

    void setTimeIsLabel( bool x ) { _timeIsLabel = x; }
    bool timeIsLabel() const { return( _timeIsLabel ); }

  private:
    bool	_timeIsLabel;
  };


  /** Partial specialization of Converter between volumes and void buckets.
  */
  template <class INP>
  class RawConverter<VolumeRef<INP>, aims::BucketMap<Void> >
    : public RawConverter<rc_ptr<Volume<INP> >, aims::BucketMap<Void> >
  {
  public:
    RawConverter( bool timeislabel = false )
      : RawConverter<rc_ptr<Volume<INP> >, aims::BucketMap<Void> >(
          timeislabel )
    {}
  };


  template <typename INP,typename OUTP>
  class ConverterAllocator<aims::BucketMap<INP>,aims::BucketMap<OUTP> >
  {
  public:
    static aims::BucketMap<OUTP>* alloc( const aims::BucketMap<INP> &in );
  };


  template <typename INP,typename OUTP>
  class ConverterAllocator<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >
  {
  public:
    static rc_ptr<Volume<OUTP> >* alloc( const aims::BucketMap<INP> &in );
  };


  template <typename INP,typename OUTP>
  class ConverterAllocator<aims::BucketMap<INP>, VolumeRef<OUTP> >
    : public ConverterAllocator<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >
  {
  public:
    static VolumeRef<OUTP>* alloc( const aims::BucketMap<INP> &in )
    {
      return static_cast<VolumeRef<OUTP>*>(
        ConverterAllocator<aims::BucketMap<INP>,
                           rc_ptr<Volume<OUTP> > >::alloc( in ) );
    }
  };


  // implementation

  template <class INP,class OUTP> inline
  void RawConverter<aims::BucketMap<INP>,aims::BucketMap<OUTP> >::convert
  ( const aims::BucketMap<INP> &in, aims::BucketMap<OUTP> &out ) const
  {
    out.setSizeXYZT( in.sizeX(), in.sizeY(), in.sizeZ(), in.sizeT() );
    typename aims::BucketMap<INP>::const_iterator i, e = in.end();
    typename aims::BucketMap<INP>::Bucket::const_iterator ib, eb;
    OUTP	outval;
    RawConverter<INP,OUTP>	itemconv;
    out.setHeader( in.header() );
    out.header().setProperty( "data_type", DataTypeCode<OUTP>::name() );

    for( i=in.begin(); i!=e; ++i )
      {
        typename aims::BucketMap<OUTP>::Bucket	& b = out[ i->first ];
        for( ib=i->second.begin(), eb=i->second.end(); ib!=eb; ++ib )
          {
            itemconv.convert( ib->second, outval );
            b[ ib->first ] = outval;
          }
      }
  }


  template <class INP,class OUTP> inline
  void RawConverter<aims::BucketMap<INP>,
                    carto::rc_ptr<Volume<OUTP> > >::printToVolume
  ( const aims::BucketMap<INP> & in, carto::rc_ptr<Volume<OUTP> > & out,
    const Point3d & offset ) const
  {
    typename aims::BucketMap<INP>::const_iterator	 it, et = in.end();
    typename aims::BucketMap<INP>::Bucket::const_iterator ib, eb;
    unsigned						 i = 0;

    RawConverter<INP, OUTP>	itemconv;

    for( it=in.begin(); it!=et; ++it, ++i )
    {
      for( ib=it->second.begin(), eb=it->second.end(); ib!=eb; ++ib )
      {
        const Point3d	& pos = ib->first;
        itemconv.convert( ib->second,
                          out->at( pos[0] + offset[0], pos[1] + offset[1],
                                   pos[2] + offset[2], i ) );
      }
    }
  }


  namespace internal
  {
    template <typename OUTP, bool MODE> 
    struct VolumePrinter
    {
      static
      void printToVolume( const aims::BucketMap<Void> & in, 
                          carto::rc_ptr<Volume<OUTP> > & out,
                          const Point3d & offset, bool timelabel );
    };

    template <typename OUTP>
    struct VolumePrinter<OUTP, true>
    {
      template <typename INP>
      static
      void printToVolume( const aims::BucketMap<INP> & in, 
                          carto::rc_ptr<Volume<OUTP> > & out, const Point3d & offset,
                          bool timelabel )
      {
        if( !timelabel )
        {
          VolumePrinter<OUTP, false>::printToVolume( in, out, offset, false );
          return;
        }

        aims::BucketMap<Void>::const_iterator		it, et = in.end();
        aims::BucketMap<Void>::Bucket::const_iterator	ib, eb;
        unsigned						i = 0;

        for( it=in.begin(); it!=et; ++it, ++i )
        {
          for( ib=it->second.begin(), eb=it->second.end(); ib!=eb; ++ib )
          {
            const Point3d	& pos = ib->first;
            out->at( pos[0] + offset[0], pos[1] + offset[1],
                     pos[2] + offset[2] )
              = (OUTP) it->first;
          }
        }
      }
    };


    template <typename OUTP>
    struct VolumePrinter<OUTP, false>
    {
      template <typename INP>
      static
      void printToVolume( const aims::BucketMap<INP> & in, 
                          carto::rc_ptr<Volume<OUTP> > & out,
                          const Point3d & offset, bool )
      {
        typename aims::BucketMap<INP>::const_iterator	it, et = in.end();
        typename aims::BucketMap<INP>::Bucket::const_iterator	ib, eb;
        unsigned					i = 0;

        RawConverter<INP,OUTP>	itemconv;

        for( it=in.begin(); it!=et; ++it, ++i )
        {
          for( ib=it->second.begin(), eb=it->second.end(); ib!=eb; ++ib )
          {
            const Point3d	& pos = ib->first;
            itemconv.convert( ib->second,
                              out->at( pos[0] + offset[0], pos[1] + offset[1],
                                       pos[2] + offset[2], i ) );
          }
        }
      }
    };

    template <typename OUTP> inline
    void printToVolume_smart( const aims::BucketMap<Void> & in, 
                              carto::rc_ptr<Volume<OUTP> > & out,
                              const Point3d & offset, bool timelabel )
    {
      VolumePrinter<OUTP, std::numeric_limits<OUTP>::is_integer>::printToVolume
        ( in, out, offset, timelabel );
    }

  }


  template <typename OUTP> inline
  void RawConverter<aims::BucketMap<Void>,
                    carto::rc_ptr<Volume<OUTP> > >::printToVolume
  ( const aims::BucketMap<Void> & in, carto::rc_ptr<Volume<OUTP> > & out,
    const Point3d & offset ) const
  {
    if( _hasValue )
    {
      typename aims::BucketMap<Void>::const_iterator   it, et = in.end();
      typename aims::BucketMap<Void>::Bucket::const_iterator   ib, eb;
      unsigned                                        i = 0;
      for( it=in.begin(); it!=et; ++it, ++i )
      {
        for( ib=it->second.begin(), eb=it->second.end(); ib!=eb; ++ib )
        {
          const Point3d     & pos = ib->first;
          out->at( pos[0] + offset[0], pos[1] + offset[1],
                   pos[2] + offset[2], i ) = _value;
        }
      }
    }
    else
      internal::printToVolume_smart( in, out, offset, timeIsLabel() );
  }


  template <class INP,class OUTP> inline
  void RawConverter<carto::rc_ptr<Volume<INP> >,
                    aims::BucketMap<OUTP> >::convert
  ( const carto::rc_ptr<Volume<INP> > &in, aims::BucketMap<OUTP> &out ) const
  {
    unsigned		t, nt = in->getSizeT();
    int x, y, z, nx = in->getSizeX(), ny = in->getSizeY(), nz = in->getSizeZ();
    INP			val;
    OUTP	outval;
    RawConverter<INP,OUTP>	itemconv;

    std::vector<float> vs = in->getVoxelSize();
    out.setSizeXYZT( vs[0], vs[1], vs[2], vs[3] );
    for( t=0; t<nt; ++t )
    {
      //cout << "time : " << t << endl;
      typename aims::BucketMap<OUTP>::Bucket	& bck = out[t];
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          for( x=0; x<nx; ++x )
          {
            /*if( y == 0 && x == 0 )
              cout << "slice : " << z << endl;*/
            val = in->at( x, y, z, t );
            if( val != (INP)0 )
            {
              itemconv.convert( val, outval );
              bck.insert( std::pair<Point3d, OUTP>
                          ( Point3d( x, y, z ), outval ) );
            }
          }
      if( bck.empty() )
        out.erase( t );
    }
  }


  template <class INP> inline
  void RawConverter<carto::rc_ptr<Volume<INP> >,
                    aims::BucketMap<Void> >::convert
  ( const carto::rc_ptr<Volume<INP> > &in, aims::BucketMap<Void> &out ) const
  {
    unsigned		t, nt = in->getSizeT();
    int x, y, z, nx = in->getSizeX(), ny = in->getSizeY(), nz = in->getSizeZ();
    INP			val;

    std::vector<float> vs = in->getVoxelSize();
    out.setSizeXYZT( vs[0], vs[1], vs[2], vs[3] );
    if( nt == 1 && timeIsLabel() )
    {
      size_t		t;
      RawConverter<INP,size_t>	rc;
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          for( x=0; x<nx; ++x )
          {
            val = in->at( x, y, z );
            if( val != INP(0) )
            {
              rc.convert( val, t );
              out[ t ].insert( std::pair<Point3d, Void>
                                ( Point3d( x, y, z ), Void() ) );
            }
        }
    }
    else
      for( t=0; t<nt; ++t )
      {
        //cout << "time : " << t << endl;
        aims::BucketMap<Void>::Bucket	& bck = out[t];
        for( z=0; z<nz; ++z )
          for( y=0; y<ny; ++y )
            for( x=0; x<nx; ++x )
            {
              val = in->at( x, y, z, t );
              if( val != INP(0) )
                bck.insert( std::pair<Point3d, Void>( Point3d( x, y, z ),
                            Void() ) );
            }
        if( bck.empty() )
          out.erase( t );
      }
  }


  template <class INP,class OUTP>
  inline aims::BucketMap<OUTP>* 
  ConverterAllocator<aims::BucketMap<INP>,aims::BucketMap<OUTP> >::alloc
  ( const aims::BucketMap<INP> &in )
  {
    aims::BucketMap<OUTP>	*out = new aims::BucketMap<OUTP>;
    out->setHeader( in.header() );
    out->header().setProperty( "data_type", DataTypeCode<OUTP>::name() );
    return out;
  }


  // VolumeRef implementation

  template <class INP,class OUTP>
  inline rc_ptr<Volume<OUTP> >*
  ConverterAllocator<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >::alloc
  ( const aims::BucketMap<INP> &in )
  {
    typename aims::BucketMap<INP>::const_iterator	it, et = in.end();
    typename aims::BucketMap<INP>::Bucket::const_iterator	ib, eb;
    Point3d	bmin( 0, 0, 0 ), bmax( 0, 0, 0 );
    bool	first = true;

    //	seek bounding box
    for( it=in.begin(); it!=et; ++it )
      for( ib=it->second.begin(), eb=it->second.end(); ib!=eb; ++ib )
      {
        const Point3d	& pos = ib->first;
        if( first )
        {
          bmin[0] = pos[0];
          bmin[1] = pos[1];
          bmin[2] = pos[2];
          bmax[0] = pos[0];
          bmax[1] = pos[1];
          bmax[2] = pos[2];
          first = false;
        }
        else
        {
          if( pos[0] < bmin[0] )
            bmin[0] = pos[0];
          if( pos[1] < bmin[1] )
            bmin[1] = pos[1];
          if( pos[2] < bmin[2] )
            bmin[2] = pos[2];
          if( pos[0] > bmax[0] )
            bmax[0] = pos[0];
          if( pos[1] > bmax[1] )
            bmax[1] = pos[1];
          if( pos[2] > bmax[2] )
            bmax[2] = pos[2];
        }
      }

    if( bmin[0] >= 0 && bmin[1] >= 0 && bmin[2] >= 0 )
      bmin = Point3d( 0, 0, 0 );

    rc_ptr<Volume<OUTP> >
      *out( new VolumeRef<OUTP>( bmax[0] - bmin[0] + 1, bmax[1] - bmin[1] + 1,
                                 bmax[2] - bmin[2] + 1, in.size() ) );
    std::vector<float> vs( 4 );
    vs[0] = in.sizeX();
    vs[1] = in.sizeY();
    vs[2] = in.sizeZ();
    vs[3] = in.sizeT();
    (*out)->header().setProperty( "voxel_size", vs );

    (*out)->fill( 0 );
    return out;
  }


  template <class INP,class OUTP> inline
  void RawConverter<aims::BucketMap<INP>, rc_ptr<Volume<OUTP> > >::convert
  ( const aims::BucketMap<INP> &in, rc_ptr<Volume<OUTP> > &out ) const
  {
    std::vector<float> vs(4);
    vs[0] = in.sizeX();
    vs[1] = in.sizeY();
    vs[2] = in.sizeZ();
    vs[3] = in.sizeT();
    out->header().setProperty( "voxel_size", vs );
    printToVolume( in, out, Point3d( 0, 0, 0 ) );
  }


  template <class OUTP> inline
  void RawConverter<aims::BucketMap<Void>, rc_ptr<Volume<OUTP> > >::convert
  ( const aims::BucketMap<Void> &in, rc_ptr<Volume<OUTP> > &out ) const
  {
    std::vector<float> vs(4);
    vs[0] = in.sizeX();
    vs[1] = in.sizeY();
    vs[2] = in.sizeZ();
    vs[3] = in.sizeT();
    out->header().setProperty( "voxel_size", vs );
    printToVolume( in, out, Point3d( 0, 0, 0 ) );
  }

}

#endif


