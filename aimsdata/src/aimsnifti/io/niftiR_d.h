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
 *  NIFTI reader template functions
 */
 
#include <cstdlib>
#include <aims/io/niftiheader.h>
#include <aims/io/niftilib/nifti1_io.h>
#include <aims/io/niftiR.h>
#include <aims/io/datatypecode.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/niftiheader.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/resampling/motion.h>
#include <aims/utility/flip.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/datasource/filedatasource.h>

namespace aims
{

  template <typename T>
  class NiftiReadProcess : public Process
  {
  public:
    NiftiReadProcess( AimsData<T> & data, nifti_image *nim, Motion & m,
                      std::vector<float> & _scale, int t, int nt );

    template <typename U>
    static bool readNifti( Process &, const std::string &, Finder & );

  private:
    AimsData<T> & _data;
    nifti_image *_nim;
    Motion      & _motion;  // from NIFTI-disk to AIMS-memory
    std::vector<float> & _scale;
    int _t0;
    int _nt;
  };


  template <typename T>
  NiftiReadProcess<T>::NiftiReadProcess( AimsData<T> & data, nifti_image* nim,
                                         Motion & m , std::vector<float> & s,
                                         int t, int nt )
    : Process(), _data( data ), _nim( nim ), _motion( m ), _scale( s ),
      _t0( t ), _nt( nt )
  {
    registerProcessType( "Volume", "S8",     &readNifti<int8_t> );
    registerProcessType( "Volume", "U8",     &readNifti<uint8_t> );
    registerProcessType( "Volume", "S16",    &readNifti<int16_t> );
    registerProcessType( "Volume", "U16",    &readNifti<uint16_t> );
    registerProcessType( "Volume", "S32",    &readNifti<int32_t> );
    registerProcessType( "Volume", "U32",    &readNifti<uint32_t> );
    registerProcessType( "Volume", "FLOAT",  &readNifti<float> );
    registerProcessType( "Volume", "DOUBLE", &readNifti<double> );
  }


  template <>
  NiftiReadProcess<AimsRGB>::NiftiReadProcess( AimsData<AimsRGB> & data,
                                               nifti_image* nim, Motion & m ,
                                               std::vector<float> & s,
                                               int t, int nt )
  : Process(), _data( data ), _nim( nim ), _motion( m ), _scale( s ),
    _t0( t ), _nt( nt )
  {
    registerProcessType( "Volume", "RGB",
      &NiftiReadProcess::readNifti<AimsRGB> );
    registerProcessType( "Volume", "RGBA",
      &NiftiReadProcess::readNifti<AimsRGBA> );
  }


  template <>
  NiftiReadProcess<AimsRGBA>::NiftiReadProcess( AimsData<AimsRGBA> & data,
                                                nifti_image* nim, Motion & m ,
                                                std::vector<float> & s,
                                                int t, int nt )
  : Process(), _data( data ), _nim( nim ), _motion( m ), _scale( s ),
    _t0( t ), _nt( nt )
  {
    registerProcessType( "Volume", "RGB",
      &NiftiReadProcess::readNifti<AimsRGB> );
    registerProcessType( "Volume", "RGBA",
      &NiftiReadProcess::readNifti<AimsRGBA> );
  }


  template <typename T>
  template <typename U>
  bool NiftiReadProcess<T>::readNifti( Process & proc, const std::string &,
                                       Finder & )
  {
    NiftiReadProcess &np = static_cast<NiftiReadProcess &>( proc );
    AimsData<T> &data = np._data;
    nifti_image *nim = np._nim;
    Motion & m = np._motion;
    std::vector<float> & s = np._scale;

    PythonHeader *ph = dynamic_cast<PythonHeader *>( data.header() );

    Motion  m2s = m.inverse();
    int dx = data.dimX(), dy = data.dimY(), dz = data.dimZ();
    Point3df pdims = m2s.transform( Point3df( data.dimX(), data.dimY(),
                                    data.dimZ() ) )
        - m2s.transform( Point3df( 0, 0, 0 ) );
    Point3d idims = Point3d( (int) rint( fabs( pdims[0] ) ),
                              (int) rint( fabs( pdims[1] ) ),
                              (int) rint( fabs( pdims[2] ) ) );
    Point3df incf = m2s.transform( Point3df( 1, 0, 0 ) )
        - m2s.transform( Point3df( 0, 0, 0 ) );
    Point3d inc = Point3d( int16_t( rint( incf[0] ) ),
                           int16_t( rint( incf[1] ) ),
                           int16_t( rint( incf[2] ) ) );
    Point3df d0f;
    Point3d d0;

    std::vector<U> src( dx * dy * dz );
    T *d = 0;
    void *buf = &src[0];
    size_t ntot = dx * dy * dz * sizeof(U);
    int ii;
    size_t yoff = idims[0];
    size_t zoff = yoff * idims[1];
//     size_t toff = zoff * idims[2];
//     long offmax = (long) toff * data.dimT();
    long offmax = (long) zoff * idims[ 2 ];
    long off;
    bool fail = false;
    int t2, nt = np._nt;
    int subbb0[7] = { 0, 0, 0, 0, 0, 0, 0 },
    subbb1[7] = { 0, 0, 0, 1, 1, 1, 1 };
    subbb1[0] = idims[0];
    subbb1[1] = idims[1];
    subbb1[2] = idims[2];
    if( nt < 0 )
      nt = data.dimT();
    if (((carto::DataTypeCode<T>::name() == "FLOAT")
      || (carto::DataTypeCode<T>::name() == "DOUBLE"))&& (s[0] != 0.0))
    {
      if( ph )
        ph->setProperty( "scale_factor_applied", true );
      for( int t=0; t<nt; ++t )
      {
        t2 = t + np._t0;

        subbb0[3] = t2;
        ii = nifti_read_subregion_image( nim, subbb0, subbb1, &buf );
        if( ii < 0 || (size_t) ii < ntot )
        {
          return false;
        }

        for( int z=0; z<dz; ++z )
          for( int y=0; y<dy; ++y )
          {
            d0f = m2s.transform( Point3df( 0, y, z ) );
            d0 = Point3d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                          int16_t( rint( d0f[2] ) ) );
            d = &data( 0, y, z, t2 );

            for( int x=0; x<dx; ++x, d0+=inc )
            {
              off = /* toff * t + */ zoff * d0[2] + yoff * d0[1] + d0[0];
              if( off >= 0 && off < offmax )
                *d++ = (T) ( ((T) src[ off ] ) * s[0] + s[1] );
              else
              {
                *d++ = 0;
                fail = true;
              }
            }
          }
      }
    }
    else
    {
      for( int t=0; t<nt; ++t )
      {
        t2 = t + np._t0;

        subbb0[3] = t2;
        ii = nifti_read_subregion_image( nim, subbb0, subbb1, &buf );
        if( ii < 0 || (size_t) ii < ntot )
        {
          return false;
        }

        for( int z=0; z<dz; ++z )
          for( int y=0; y<dy; ++y )
          {
            d0f = m2s.transform( Point3df( 0, y, z ) );
            d0 = Point3d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                          int16_t( rint( d0f[2] ) ) );
            d = &data( 0, y, z, t2 );
            for( int x=0; x<dx; ++x, d0+=inc )
            {
              off = /* toff * t + */ zoff * d0[2] + yoff * d0[1] + d0[0];
              if( off >= 0 && off < offmax )
                *d++ = (T) src[ off ];
              else
              {
                *d++ = 0;
                fail = true;
              }
            }
          }
      }
    }
    if( fail )
      std::cerr << "Warning: storage_to_memory transformation in NIFTI1 file "
          "seems to be wrong" << std::endl;
    return true;
  }


  template <typename T>
  void NiftiReader<T>::read(AimsData<T>& thing, 
                          const carto::AllocatorContext & context, 
                          carto::Object options )
  {
    NiftiHeader *hdr = new NiftiHeader( _name );

    try
    {
      hdr->read();
    }
    catch( std::exception & e )
    {
      delete hdr;
      throw e;
    }
    // std::cout << "This is NIFTI Reader: use at your own risk." << std::endl;

    std::vector< int > dims;
    hdr->getProperty( "volume_dimension", dims );

    int	frame = -1, border = 0;
    options->getProperty( "frame", frame );
    options->getProperty( "border", border );

    bool series = false;
    std::vector<carto::Object>	fnames;
    std::string bname;
    if( dims[3] > 1 )
    {
      series = hdr->getProperty( "series_filenames", fnames );
      if( series)
      {
        if( fnames.size() < (unsigned int)dims[3] )
          throw std::domain_error( "NIFTI reader: not enough filenames for "
                                   "all frames");
          bname = carto::FileUtil::dirname( _name ) +
              carto::FileUtil::separator();
          hdr->removeProperty( "series_filenames" );
      }
    }

    carto::AllocatorContext	al
        ( context.accessMode(),
          carto::rc_ptr<carto::DataSource>
        ( new carto::FileDataSource( _name, 0, carto::DataSource::Read ) ), 
        false, context.useFactor() );

    AimsData<T> data;
    data = AimsData<T>( hdr->dimX(), hdr->dimY(), hdr->dimZ(), hdr->dimT(),
                          border, al );
    data.setHeader( hdr->cloneHeader() );

    int t = 0;
    readSubImage( hdr, t, data );
    t += hdr->niftiNim()->dim[4];

    if( series )
    {
      int i, n = fnames.size();
      std::string fnm;
      for( i=1; i<n; ++i )
      {
        fnm = bname + carto::FileUtil::basename( fnames[i]->getString() );
        NiftiHeader hdr2( fnm );
        hdr2.read();
        readSubImage( &hdr2, t, data );
        t += hdr2.niftiNim()->dim[4];
        hdr2.freeNiftiStruct();
      }
    }

    thing = data;
    hdr->freeNiftiStruct();
    delete hdr;
  }


  template <typename T> void
  NiftiReader<T>::readSubImage( NiftiHeader* hdr, int t, AimsData<T> & data )
  {
    nifti_image *nim = hdr->niftiNim();

    // fix erroneous null dimT
    if( nim->dim[4] == 0 )
      nim->dim[4] = 1;

    std::vector< float > storage_to_memory;
    hdr->getProperty( "storage_to_memory", storage_to_memory );

    Motion m( storage_to_memory );

    std::vector<float> s(2);
    hdr->getProperty( "scale_factor", s[0] );
    hdr->getProperty( "scale_offset", s[1] );

    NiftiReadProcess<T> proc( data, nim, m, s, t, nim->dim[4] );
    Finder		f2;
    f2.setObjectType( "Volume" );
    std::string dt;
    hdr->getProperty( "disk_data_type", dt );
    f2.setDataType( dt );
    if( !proc.execute( f2, "" ) )
      throw carto::corrupt_stream_error( _name );
  }

}
