/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Ecat6-6 data reader class
 */
#include <aims/io/ecatR.h>
#include <cartobase/exception/assert.h>
#include <ecat+/io/io.h>
#include <cartobase/exception/format.h>
#include <cartobase/allocator/mappingcopy.h>
#include <cartobase/datasource/filedatasource.h>
#include <vector>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


template< class T >
string EcatReader< T >::removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 2 )
    ext = res.substr( int(res.length() - 2), 2 );
  if (ext == ".v")
    res = res.substr( 0, res.length() - 2 );
  return res;
}

namespace aims
{

template<>
void EcatReader< short >::frameRead( AimsData<short>&, 
                                     const carto::AllocatorContext &, 
                                     int, int )
{
  throw format_error( "frame by frame input capability is not supported for "
		      "Ecat short reading", _name );
}

template<>
void EcatReader< float >::frameRead( AimsData<float>& thing, 
                                     const carto::AllocatorContext & context, 
                                     int frame,
                                     int border )
{
  UnifiedEcatInfo    *uei;
  string             fileName;

  // -------------------------Initialisation du header propriétaire and Fix it
  EcatHeader *hdr = new EcatHeader( _name );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      throw( e );
    }

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
  ASSERT( volDim[3] != 0 ); //It is not a dynamic series ?? 

  ASSERT( (frame >= 0) && (frame < volDim[3]) );


  // ----------------------------------------Ouverture du uei
  fileName = _name;   // .i, .v, .p are different ECAT volumes
  if ( (uei = EcatOpen( (char*) hdr->name().c_str(),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL )
    throw format_error( fileName );

  carto::AllocatorContext 
    cont2( context.accessMode(), 
           rc_ptr<DataSource>
           ( new FileDataSource( hdr->name(), 0, DataSource::Read ) ), 
           false, context.useFactor() );

  AimsData<float> data( uei->size.x, uei->size.y, uei->size.z, 1,
			border, cont2 );

  data.setSizeXYZT( uei->voxelsize.x, uei->voxelsize.y,
		    uei->voxelsize.z, uei->voxelsize.t );
  float *f_pt, *f_ima;

  // --------------------------------------------Lecture frame par frame
  AimsData<float>::iterator wit = data.begin() + data.oFirstPoint();
  if ( ! EcatDataCalibrated(uei) )
    {
	  ASSERT( ( f_ima = EcatReadVolume_FLOAT( uei ,frame))!=ECATSHFJ_FAIL );
	  f_pt = f_ima;
	  for( int z = 0; z < EcatSizeZ(uei); z++ )
	    {
	      for ( int y = 0; y < EcatSizeY(uei); y++ )
		{
		  for ( int x = 0; x < EcatSizeX(uei); x++ )
		    {
		      *wit++ = (float)((double) *f_pt++ * (double) EcatCalib( uei ));
		    }
		  wit += data.oPointBetweenLine();
		}
	      wit += data.oLineBetweenSlice();
	    }
	  wit += data.oSliceBetweenVolume();
	  free( f_ima );
    }
  else
    {
	  ASSERT( ( f_ima = EcatReadVolume_FLOAT( uei ,frame))!=ECATSHFJ_FAIL );
	  f_pt = f_ima;
	  for( int z = 0; z < EcatSizeZ(uei); z++ )
	    {
	      for ( int y = 0; y < EcatSizeY(uei); y++ )
		{
		  for ( int x = 0; x < EcatSizeX(uei); x++ )
		    {
		      *wit++ =  *f_pt++;
		    }
		  wit += data.oPointBetweenLine();
		}
	      wit += data.oLineBetweenSlice();
	    }
	  wit += data.oSliceBetweenVolume();
	  free( f_ima );
    }

  data.setHeader( hdr );

  // ------------------------------Fin.
  thing = data;
}

template<>
void EcatReader< short >::read( AimsData<short>& thing, 
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

  UnifiedEcatInfo    *uei;
  string             fileName;
  EcatHeader         *hdr;
 

  // --------------------------------Initialisation du header propriétaire
  hdr = new EcatHeader( _name );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      throw( e );
    }

  // ----------------------------------------Ouverture du uei
  fileName = _name;   // .i, .v, .p are different ECAT volumes
  if ( (uei = EcatOpen( (char*) hdr->name().c_str(),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL )
    throw format_error( fileName );

    carto::AllocatorContext 
      cont2( context.accessMode(), 
             rc_ptr<DataSource>
             ( new FileDataSource( hdr->name(), 0, carto::DataSource::Read ) ),
             false, context.useFactor() );

  AimsData<short> data( uei->size.x, uei->size.y, uei->size.z, uei->size.t,
			border, cont2 );
  data.setSizeXYZT( uei->voxelsize.x, uei->voxelsize.y,
		    uei->voxelsize.z, uei->voxelsize.t );
  data.setHeader( hdr );
  short *s_pt, *s_ima;

  // --------------------------------------------Lecture frame par frame
  for(int frame=0; frame < uei->size.t; frame++)
    {
      ASSERT( ( s_ima = EcatReadVolume_S16( uei ,frame))!=ECATSHFJ_FAIL );
      s_pt = s_ima;
      for(int slice=0; slice<uei->size.z;slice++)
	for(int line=0;line <uei->size.y;line++)
	  {
	    memcpy((char*)&data(0,line,slice,frame),(char*)s_pt,
		   uei->size.x*sizeof(short));
	    s_pt += uei->size.x;
	  }
      free( s_ima );
    }

  // ------------------------------Remise en coherence des scale factors
  if (string(EcatCalibUnit( uei )) != "Labels")
    {
      AimsData<short>::const_iterator it = data.begin() + data.oFirstPoint();
      double maxi = (double)*it * (double)EcatVolScale(uei,0) 
	* (double)EcatCalib(uei);
      double mini = maxi;
      for(int frame=0; frame< EcatSizeT(uei); frame++)
	{
	  double sf = (double)EcatVolScale(uei,frame ); // EcatCalib( uei ) inutile 
	                                                //car constant pour toutes les frames;
	  for( int z = 0; z < EcatSizeZ(uei); z++ )
	    {
	      for ( int y = 0; y < EcatSizeY(uei); y++ )
		{
		  for ( int x = 0; x < EcatSizeX(uei); x++ )
		    {
		      double tmp =(double)*it * sf;
		      if ( tmp > maxi ) maxi = tmp;
		      if ( tmp < mini ) mini = tmp;
		      it++;
		    }
		  it += data.oPointBetweenLine();
		}
	      it += data.oLineBetweenSlice();
	    }
	  it += data.oSliceBetweenVolume();
	}
      
      double ratio;
      if (maxi > -mini ) ratio = (double) 32767 / maxi;
      else               ratio = (double) 32768 / (-mini);
      _scale = (float )((double) 1.0 / ratio);
      
      
      AimsData<short>::iterator wit = data.begin() + data.oFirstPoint();
      for(int frame=0; frame< EcatSizeT(uei); frame++)
	{
	  double sf = (double)EcatVolScale(uei,frame ) * (double)EcatCalib( uei );
	  for( int z = 0; z < EcatSizeZ(uei); z++ )
	    {
	      for ( int y = 0; y < EcatSizeY(uei); y++ )
		{
		  for ( int x = 0; x < EcatSizeX(uei); x++ )
		    {
		      int tmp =  (int) ((double)*wit * sf * ratio);
		      if (tmp > 32767) *wit++ = 32767;
		      else if (tmp < -32768 ) *wit++ = -32768;
		      else *wit++ = (short) tmp;
		    }
		  wit += data.oPointBetweenLine();
		}
	      wit += data.oLineBetweenSlice();
	    }
	  wit += data.oSliceBetweenVolume();
	}  
    }
  // -------------------------------------------------fermeture de l'uei
  EcatClose( uei );

  // ------------------------------Fin.
  thing = data;
}

template<>
void EcatReader< float >::read( AimsData<float>& thing, 
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

  UnifiedEcatInfo    *uei;
  string             fileName;
  EcatHeader         *hdr;
  float              *f_pt, *f_ima;

  // --------------------------------Initialisation du header propriétaire
  hdr = new EcatHeader( _name );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      throw( e );
    }

  // ----------------------------------------------------------Ouverture du uei
  fileName = _name;   // .i, .v, .p are different ECAT volumes
  if ( (uei = EcatOpen( (char*) hdr->name().c_str(),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL )
    throw format_error( fileName );

  carto::AllocatorContext 
    cont2( context.accessMode(), 
           rc_ptr<DataSource>
           ( new FileDataSource( hdr->name(), 0, DataSource::Read ) ), 
           false, context.useFactor() );

  AimsData<float> data( uei->size.x, uei->size.y, uei->size.z, uei->size.t,
			border, cont2 );
  data.setSizeXYZT( uei->voxelsize.x, uei->voxelsize.y,
		    uei->voxelsize.z, uei->voxelsize.t );
  data.setHeader( hdr );

  // Caracteristiques des images
  cout << "Image units are: "  << EcatCalibUnit(uei) << endl;


  // --------------------------------------------Lecture frame par frame
  AimsData<float>::iterator wit = data.begin() + data.oFirstPoint();
  if ( ! EcatDataCalibrated(uei) )
    {
      for(int frame=0; frame < uei->size.t; frame++)
	{
	  ASSERT( ( f_ima = EcatReadVolume_FLOAT( uei ,frame))!=ECATSHFJ_FAIL );
	  f_pt = f_ima;
	  for( int z = 0; z < EcatSizeZ(uei); z++ )
	    {
	      for ( int y = 0; y < EcatSizeY(uei); y++ )
		{
		  for ( int x = 0; x < EcatSizeX(uei); x++ )
		    {
		      *wit++ = (float)((double) *f_pt++ * (double) EcatCalib( uei ));
		    }
		  wit += data.oPointBetweenLine();
		}
	      wit += data.oLineBetweenSlice();
	    }
	  wit += data.oSliceBetweenVolume();
	  free( f_ima );
	}
    }
  else
    {
      for(int frame=0; frame < uei->size.t; frame++)
	{
	  ASSERT( ( f_ima = EcatReadVolume_FLOAT( uei ,frame))!=ECATSHFJ_FAIL );
	  f_pt = f_ima;
	  for( int z = 0; z < EcatSizeZ(uei); z++ )
	    {
	      for ( int y = 0; y < EcatSizeY(uei); y++ )
		{
		  for ( int x = 0; x < EcatSizeX(uei); x++ )
		    {
		      *wit++ =  *f_pt++;
		    }
		  wit += data.oPointBetweenLine();
		}
	      wit += data.oLineBetweenSlice();
	    }
	  wit += data.oSliceBetweenVolume();
	  free( f_ima );
	}
    }

  // --------------------------------------------------------fermeture de l'uei
  EcatClose( uei );

  // ------------------------------Fin.
  thing = data;
}

}

template class EcatReader< int16_t >;
template class EcatReader< float >;

