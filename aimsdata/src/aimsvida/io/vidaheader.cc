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

#include <aims/io/vidaheader.h>
#include <aims/def/general.h>
#include <cartobase/exception/ioexcept.h>
#include <vector>
#include <iostream>
#include <fstream>

using aims::VidaHeader;
using namespace carto;
using namespace std;

#define _typeINIT FX_SHORT_FX


VidaHeader::VidaHeader(const string& name) :
  PythonHeader(), 
  _name(name), _type( _typeINIT )
{
}


VidaHeader::VidaHeader( int dimx, int dimy, int dimz, int dimt, float sx, 
			float sy, float sz, float st, const string& name ) :
  PythonHeader(), 
  _name(name), _type( _typeINIT )
{
  vector<int>	dims(4);
  dims[0] = dimx;
  dims[1] = dimy;
  dims[2] = dimz;
  dims[3] = dimt;
  vector<float>	vs(4);
  vs[0] = sx;
  vs[1] = sy;
  vs[2] = sz;
  vs[3] = st;
  setProperty( "volume_dimension", dims );
  setProperty( "voxel_size", vs );
}


VidaHeader::~VidaHeader()
{
}


set<string> VidaHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".vhdr" );
  exts.insert( ".vimg" );
  exts.insert( ".vinfo" );
  return exts;
}


string VidaHeader::name() const
{
  return _name;
}


int VidaHeader::itemType() const
{
    return _type;
}


bool VidaHeader::imageIsProbablyLabel() const
{
  return (_type == _typeINIT);
}


int VidaHeader::dimX() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return( vd[0] );
  return( 1 );
}

int VidaHeader::dimY() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return( vd[1] );
  return( 1 );
}

int VidaHeader::dimZ() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return( vd[2] );
  return( 1 );
}

int VidaHeader::dimT() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return( vd[3] );
  return( 1 );
}

float VidaHeader::sizeX() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return( vs[0] );
  return( 1 );
}

float VidaHeader::sizeY() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return( vs[1] );
  return( 1 );
}

float VidaHeader::sizeZ() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return( vs[2] );
  return( 1 );
}

float VidaHeader::sizeT() const
{
  float tr;
  if( getProperty( "time_resolution", tr ) )
    return( tr );
  return( 1 );
}

void VidaHeader::read()
{
  string      fileName;
  VIDAim      *vp;

  fileName = removeExtension(_name) + ".vimg";
  vp = VidaOpen((char*)fileName.c_str(),
                VidaConsigne(VMODE,"r",VMDTYPE,FX_SHORT_FX,NULL));
  if( !vp )
    {
      io_error::launchErrnoExcept( fileName );
      throw wrong_format_error( fileName );
    }

  setProperty( "file_type", string( "VIDA" ) );
  setProperty( "byte_swapping", (int) vp->ana_db->key.hkey_un0 );

  vector< int > volDim;
  volDim.push_back( (int)VIDA_X(vp) ); 
  volDim.push_back( (int)VIDA_Y(vp) ); 
  volDim.push_back( (int)VIDA_Z(vp) ); 
  volDim.push_back( (int)VIDA_T(vp) ); 
  setProperty( "volume_dimension", volDim );

  vector< float > voxSize;
  voxSize.push_back( (float) VIDA_PX(vp) );
  voxSize.push_back( (float) VIDA_PY(vp) );
  voxSize.push_back( (float) VIDA_PZ(vp) );
  setProperty( "voxel_size", voxSize );

  setProperty( "time_resolution", (float) VIDA_PT(vp)); // VIDA_PT(vp) ??

  int flag = 1;
  for (int fr=0;fr<VIDA_T(vp);fr++)
    for (int sl=0;sl<VIDA_Z(vp);sl++)
    {
      flag = flag && (GetCalib(vp->shfj_db,sl,fr) == 1. );
      flag = flag && (GetOffset(vp->shfj_db,sl,fr) == 0. );
    }
  if (flag) _type = (int)FX_SHORT_FX;
  else      _type = (int)FX_SHORT_FL;

  if ( VIDA_T(vp) > 1)
    {
      vector< int > startTime, durTime;
      for (int t = 0; t < VIDA_T(vp); t++)
	{
	  int r =  GetTimeStart(vp->shfj_db, t);
	  r = GetTimeStart(vp->shfj_db, t);
	  startTime.push_back( (int) r);
	  r = GetTimeDura(vp->shfj_db, t);
	  durTime.push_back( (int) r );
	}
      setProperty( "start_time",    startTime);
      setProperty( "duration_time", durTime);
    }

  VidaClose(vp);

  if( imageIsProbablyLabel() )
    setProperty( "data_type", string( "S16" ) );
  else
    setProperty( "data_type", string( "FLOAT" ) );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );
}


void VidaHeader::write()
{
  setProperty( "file_type", string( "VIDA" ) );
  setProperty( "object_type", string( "Volume" ) );
  if( hasProperty( "data_type" ) )
    removeProperty( "data_type" );
  /*
  if( imageIsProbablyLabel() )
    setProperty( "data_type", string( "S16" ) );
  else
    setProperty( "data_type", string( "FLOAT" ) );
  */
  vector<int>	dims;
  dims.push_back( dimX() );
  dims.push_back( dimY() );
  dims.push_back( dimZ() );
  dims.push_back( dimT() );
  setProperty( "volume_dimension", dims );
  vector<float>	vs;
  vs.push_back( sizeX() );
  vs.push_back( sizeY() );
  vs.push_back( sizeZ() );
  vs.push_back( sizeY() );
  setProperty( "voxel_size", vs );
  //cout << "write minf: " << removeExtension( _name ) + extension() << endl;
  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}
