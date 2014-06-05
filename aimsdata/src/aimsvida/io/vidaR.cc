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

#include <aims/def/general.h>
#include <aims/io/vidaheader.h>
#include <aims/data/data.h>
#include <aims/io/vidaR.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/filedatasource.h>
#include <vector>
#include <iostream>

// protection against MacOS macros
#ifdef MAP_COPY
#undef MAP_COPY
#endif

using namespace aims;
using namespace carto;
using namespace std;

template<typename T>
AllocatorContext VidaReader<T>::setupMemMode( const VidaHeader & hdr, 
                                              const AllocatorContext & context,
                                              int border )
{
  string fileName = hdr.removeExtension(_name) + ".vimg";
  int	bswap = false;
  hdr.getProperty( "byte_swapping", bswap );
  bool	dszok = ( sizeof(T) == 2 && !hdr.imageIsProbablyLabel() );

  return AllocatorContext
    ( context.accessMode(), 
      rc_ptr<DataSource>
      ( new FileDataSource
        ( fileName, 0, 
          context.accessMode() == AllocatorStrategy::ReadWrite ? 
          DataSource::ReadWrite : DataSource::Read ) ), 
      AllocatorStrategy::isMMapCompatible
      ( dszok, bswap ? 0 : AIMS_MAGIC_NUMBER, border ), 
      context.useFactor() );
}


namespace aims
{

template <>
void VidaReader< int16_t >::frameRead(AimsData<int16_t>& thing,
                                      const carto::AllocatorContext & context, 
                                      int frame,
                                      int border)
{
  VidaHeader * hdr;

  hdr = new VidaHeader(_name);
  try
    {
      hdr->read();  // Read header and fix the vector Start/Duration Time
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      delete hdr;
      throw;
    }

  carto::AllocatorContext	al = setupMemMode( *hdr, context, border );

  vector< int > sdt, sdt_s, sdt_d;
  if (!hdr->getProperty("start_time", sdt))
    sdt_s.push_back( 0 );
  else
    sdt_s.push_back( sdt[frame] );
  hdr->setProperty("start_time", sdt_s); 

  if (!hdr->getProperty("duration_time", sdt))
    sdt_d.push_back( 0 );
  else
    sdt_d.push_back( sdt[frame] );
  hdr->setProperty("duration_time", sdt_d);  

  vector< int > volDim;
  hdr->getProperty("volume_dimension",volDim);
  if( volDim[3] == 0 || frame < 0 || frame >= volDim[3] )
    throw invalid_format_error( _name );

  string fileName = hdr->removeExtension( _name ) + ".vimg";
  VIDAim *vp = VidaOpen((char*)fileName.c_str(),
                        VidaConsigne(VMODE,"r",VMDTYPE,hdr->itemType(),NULL));
  if( !vp )
    {
      io_error::launchErrnoExcept( fileName );
      throw wrong_format_error( fileName );
    }
 
  AimsData<int16_t> data( VIDA_X(vp), VIDA_Y(vp), VIDA_Z(vp), 1, border,
                          al );
  data.setSizeXYZT( VIDA_PX(vp), VIDA_PY(vp), VIDA_PZ(vp), 1 );


  AllocatorStrategy::MappingMode 
    mode = data.allocator().allocatorType();

  if( mode == AllocatorStrategy::Unallocated
      && !data.volume()->refVolume().isNull() )
    mode = data.volume()->refVolume()->allocatorContext().allocatorType();

  if( mode == AllocatorStrategy::Memory || mode == AllocatorStrategy::CopyMap 
      || mode == AllocatorStrategy::MAP 
      || mode == AllocatorStrategy::Unallocated )
    {
      int16_t *s_pt,*s_ima = new int16_t[ VIDA_X(vp) * VIDA_Y(vp) ];
 
      for(int slice=0;slice<VIDA_Z(vp);slice++)
        {
          if(VidaRead((char *)s_ima,slice,frame,vp) == 0)
            {
              io_error::launchErrnoExcept( fileName );
              throw invalid_format_error( fileName );
            }
          s_pt = s_ima;
          for(int line=0;line<VIDA_Y(vp);line++)
            { memcpy((char*)&((data)(0,line,slice,0)),(char*)s_pt,
                     VIDA_X(vp)*sizeof(int16_t));
            s_pt += VIDA_X(vp);
            }
        }
 
      delete[] s_ima;
    }
  VidaClose(vp);

  data.setHeader( hdr );  
  thing = data;
}


template <>
void VidaReader< float >::frameRead(AimsData<float>& thing,
                                    const AllocatorContext & context, 
                                    int frame,
                                    int border)
{
  VidaHeader * hdr = new VidaHeader(_name);
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      delete hdr;
      throw;
    }

  AllocatorContext	al = setupMemMode( *hdr, context, border );

  vector< int > sdt, sdt_s, sdt_d;
  if (!hdr->getProperty("start_time", sdt))
    sdt_s.push_back( 0 );
  else
    sdt_s.push_back( sdt[frame] );
  hdr->setProperty("start_time", sdt_s); 

  if (!hdr->getProperty("duration_time", sdt))
    sdt_d.push_back( 0 );
  else
    sdt_d.push_back( sdt[frame] );
  hdr->setProperty("duration_time", sdt_d);  

  vector< int > volDim;
  hdr->getProperty("volume_dimension",volDim);
  if( volDim[3] == 0 || frame < 0 || frame >= volDim[3] )
    throw invalid_format_error( _name );

  string fileName = hdr->removeExtension(_name) + ".vimg";
  VIDAim *vp = VidaOpen((char*)fileName.c_str(),
                        VidaConsigne(VMODE,"r",VMDTYPE,FL_FLOAT_FL,NULL));
  if( !vp )
    {
      io_error::launchErrnoExcept( fileName );
      throw wrong_format_error( fileName );
    }
 
  AimsData<float> data( VIDA_X(vp), VIDA_Y(vp), VIDA_Z(vp), 1, border,
                        al );
  data.setSizeXYZT( VIDA_PX(vp), VIDA_PY(vp), VIDA_PZ(vp), 1 );

  AllocatorStrategy::MappingMode 
    mode = data.allocator().allocatorType();

  if( mode == AllocatorStrategy::Unallocated
      && !data.volume()->refVolume().isNull() )
    mode = data.volume()->refVolume()->allocatorContext().allocatorType();

  if( mode == AllocatorStrategy::Memory || mode == AllocatorStrategy::CopyMap 
      || mode == AllocatorStrategy::MAP 
      || mode == AllocatorStrategy::Unallocated )
    {
      float *f_pt,*f_ima = new float[ VIDA_X(vp) * VIDA_Y(vp) ];

      for(int slice=0;slice<VIDA_Z(vp);slice++)
        {
          if(VidaRead((char *)f_ima,slice,frame,vp) == 0)
            {
              io_error::launchErrnoExcept( fileName );
              throw invalid_format_error( fileName );
            }
          f_pt = f_ima;
          for(int line=0;line<VIDA_Y(vp);line++)
            { memcpy((char*)&((data)(0,line,slice,0)),(char*)f_pt,
                     VIDA_X(vp)*sizeof(float));
            f_pt += VIDA_X(vp);
            }
        }
 
      delete[] f_ima;
    }
  VidaClose(vp);

  data.setHeader( hdr );  
  thing = data;
}


template <>
void VidaReader< int16_t >::read(AimsData<int16_t>& thing, 
                                 const AllocatorContext & context, 
                                 Object options )
{
  int	frame = -1, border = 0;
  options->getProperty( "frame", frame );
  options->getProperty( "border", border );

  if( frame >= 0 )
    {
      frameRead( thing, context, frame, border );
      return;
    }

  VidaHeader * hdr;

  hdr = new VidaHeader(_name);
  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  carto::AllocatorContext	al = setupMemMode( *hdr, context, border );

  string fileName = hdr->removeExtension(_name) + ".vimg";

  VIDAim *vp = VidaOpen((char*)fileName.c_str(),
                        VidaConsigne(VMODE,"r",VMDTYPE,hdr->itemType(),NULL));
  if( !vp )
    {
      io_error::launchErrnoExcept( fileName );
      throw wrong_format_error( fileName );
    }

  AimsData<int16_t> data( VIDA_X(vp), VIDA_Y(vp), VIDA_Z(vp), VIDA_T(vp), 
                          border, al );
  data.setSizeXYZT( VIDA_PX(vp), VIDA_PY(vp), VIDA_PZ(vp), VIDA_PT(vp)  );

  AllocatorStrategy::MappingMode 
    mode = data.allocator().allocatorType();

  if( mode == AllocatorStrategy::Memory || mode == AllocatorStrategy::CopyMap 
      || mode == AllocatorStrategy::MAP 
      || mode == AllocatorStrategy::Unallocated )
    {
      int16_t *s_pt,*s_ima = new int16_t[ VIDA_X(vp) * VIDA_Y(vp) ];

      for(int frame=0;frame<VIDA_T(vp);frame++)
        {
          for(int slice=0;slice<VIDA_Z(vp);slice++)
            {
              if(VidaRead((char *)s_ima,slice,frame,vp) == 0)
                {
                  io_error::launchErrnoExcept( fileName );
                  throw invalid_format_error( fileName );
                }
              s_pt = s_ima;
              for(int line=0;line<VIDA_Y(vp);line++)
                { memcpy((char*)&data(0,line,slice,frame),(char*)s_pt,
                         VIDA_X(vp)*sizeof(int16_t));
                s_pt += VIDA_X(vp);
                }
            }
        }
      delete[] s_ima;
    }
  VidaClose(vp);

  data.setHeader( hdr );
  thing = data;
}



template <>
void VidaReader< float >::read(AimsData< float >& thing, 
                               const carto::AllocatorContext & context, 
                               carto::Object options )
{
  int	frame = -1, border = 0;
  options->getProperty( "frame", frame );
  options->getProperty( "border", border );

  if( frame >= 0 )
    {
      frameRead( thing, context, frame, border );
      return;
    }

  VidaHeader * hdr;

  hdr = new VidaHeader(_name);
  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  carto::AllocatorContext	al = setupMemMode( *hdr, context, border );

  string fileName = hdr->removeExtension(_name) + ".vimg";
  VIDAim *vp = VidaOpen((char*)fileName.c_str(),
                        VidaConsigne(VMODE,"r",VMDTYPE,FL_FLOAT_FL,NULL));
  if( !vp )
    {
      io_error::launchErrnoExcept( fileName );
      throw wrong_format_error( fileName );
    }

  AimsData<float> data( VIDA_X(vp), VIDA_Y(vp), VIDA_Z(vp), VIDA_T(vp), border,
                        al );
  data.setSizeXYZT( VIDA_PX(vp), VIDA_PY(vp), VIDA_PZ(vp), VIDA_PT(vp) );

  AllocatorStrategy::MappingMode 
    mode = data.allocator().allocatorType();

  if( mode == AllocatorStrategy::Memory || mode == AllocatorStrategy::CopyMap 
      || mode == AllocatorStrategy::MAP 
      || mode == AllocatorStrategy::Unallocated )
    {
      float *s_pt,*s_ima = new float[ VIDA_X(vp) * VIDA_Y(vp) ];

      for(int frame=0;frame<VIDA_T(vp);frame++)
        {
          for(int slice=0;slice<VIDA_Z(vp);slice++)
            {
              if(VidaRead((char *)s_ima,slice,frame,vp) == 0)
                {
                  io_error::launchErrnoExcept( fileName );
                  throw invalid_format_error( fileName );
                }
              s_pt = s_ima;
              for(int line=0;line<VIDA_Y(vp);line++)
                {
                  memcpy((char*)&data(0,line,slice,frame),(float*)s_pt,
                         VIDA_X(vp)*sizeof(float));
                  s_pt += VIDA_X(vp);
                }
            }
        }
      delete[] s_ima;
    }
  VidaClose(vp);


  data.setHeader( hdr );
  thing = data;
}

}

template class VidaReader< int16_t >;
template class VidaReader< float >;
