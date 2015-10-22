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

#include <aims/def/assert.h>
#include <aims/io/ecatheader.h>
#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <aims/io/ecatW.h>
#include <ecat+/io/io.h>
#include <cartobase/exception/file.h>
#include <string>
#include <vector>
#include <time.h>

using namespace aims;
using namespace std;
using namespace carto;   


template<class T>
static void fillEcatHdr( UnifiedEcatInfo *uei, const T *ahdr, bool useflt )
{
	  vector<int> st, dt;
	  if( ! ahdr->getProperty( "start_time", st ) )
	    {
	      st.resize( EcatSizeT( uei ) ); 
	    }
	  if( ! ahdr->getProperty( "duration_time", dt ) )
	    {
	      dt.resize( EcatSizeT( uei ) );
	    }
	  for (int t = 0; t < EcatSizeT( uei ); t++)
	    {
	      EcatVolStartTime(uei, t) = st[t];
	      EcatVolDurTime(uei, t) = dt[t];
	    }
	  string tmp;
	  if (ahdr->getProperty("image_unit",tmp))
	    {
	      strcpy( EcatCalibUnit(uei),  tmp.c_str());
	      if( useflt && strcmp(EcatCalibUnit(uei),ECATSHFJ_BQCC) == 0)
		{
		  EcatDataCalibrated(uei) = 1;
		  EcatCalib(uei) = 1.0;
		}
	    }
	  if (ahdr->getProperty( "radiopharmaceutical", tmp))
	    {
	      strcpy (uei->radiopharmaceutical, tmp.c_str() );
	    }
	  float isotope_halflife;
	  if (ahdr->getProperty( "isotope_halflife",isotope_halflife ))
	    {
	      uei->isotope_halflife = isotope_halflife;
	      //cout << "uei->isotope_halflife = "<< uei->isotope_halflife << endl <<endl ;
	    } //else cout << "isotope halflife is missing" << endl ;
	  int proc_code;
	  if (ahdr->getProperty("process_code",proc_code))
	    {
	      EcatProcCode( uei ) = proc_code;
		
	      if (ahdr->getProperty("proc_list",tmp))
		{
		  strcpy( uei->proc_list,  tmp.c_str());
		}
	    }
	  if (ahdr->getProperty("category",tmp))
	    {
	      strcpy( uei->category,  tmp.c_str());
	    }
	  time_t zero_start_time;
	  if (ahdr->getProperty("zero_start_time", zero_start_time))
	    {
	      uei->zero_start_time = zero_start_time;
	    }
	  if (ahdr->getProperty( "original_filename", tmp))
	    {
	      strcpy ( uei->original_filename, tmp.c_str() );
	    }
}


void EcatWriter::write(const AimsData<short>& thing)
{
  //string fileName = removeExtension(_name);
  string fileName = _name;
  
  UnifiedEcatInfo *uei =  EcatOpen((char*)fileName.c_str(),
                                    const_cast<char*>( "w" ) );
  if (uei == ECATSHFJ_FAIL)
    throw file_error("Cannot create file ", fileName);
 

  // Champs initialisable depuis les informations contenues 
  // dans AimsData sans recours au Header de AimsData
  EcatSizeX(uei) = thing.dimX();     // Champs triviaux
  EcatSizeY(uei) = thing.dimY();
  EcatSizeZ(uei) = thing.dimZ();
  EcatSizeT(uei) = thing.dimT();
  
  uei->voxelsize.x = thing.sizeX();
  uei->voxelsize.y = thing.sizeY();
  uei->voxelsize.z = thing.sizeZ();
  uei->voxelsize.t = 0.0;
				    // init de internes
  uei->rawlist = (int *)calloc(EcatSizeT(uei), sizeof(int));
  uei->sortuniqlist = (long *)calloc(EcatSizeT(uei), sizeof(long));
  uei->start_time = ( int *)calloc(EcatSizeT(uei), sizeof( int));
  uei->duration_time = ( int *)calloc(EcatSizeT(uei), sizeof( int));
  uei->scale = (float *)calloc(EcatSizeT(uei), sizeof(float));
  for(int t =0;t<EcatSizeT(uei);t++)
    {
      uei->rawlist[t] = t+1;    
      uei->sortuniqlist[t] = t;    
    }
  
  // Acces au Header si disponible
  // ic tester si le header est attribue ou non.
  int	dtype = ECAT_LABEL;
  EcatHeader	hdr( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		     thing.sizeX(), thing.sizeY(), thing.sizeZ(), 
		     thing.sizeT(), dtype, fileName );
  const PythonHeader 
    *ph = dynamic_cast<const PythonHeader *>( thing.header() );
  if( ph )
    {
      fillEcatHdr( uei, ph, false );
      hdr.copy( *ph );
      vector<int>	dims(4);
      dims[0] = thing.dimX();
      dims[1] = thing.dimY();
      dims[2] = thing.dimZ();
      dims[3] = thing.dimT();
      hdr.setProperty( "volume_dimension", dims );
      vector<float>	vs(4);
      vs[0] = thing.sizeX();
      vs[1] = thing.sizeY();
      vs[2] = thing.sizeZ();
      vs[3] = thing.sizeT();
      hdr.setProperty( "voxel_size", vs );
    }

  /* 512 bytes added because in EcatWriteVolume_S16, a call to
     file_data_to_host() (libecat) is performed, which only works on chunks
     of 512 bytes (!)
  */
  size_t sbuf = thing.dimX() * thing.dimY() * thing.dimZ()
    + 512 / sizeof(int16_t);
  short *s_pt,*s_vol = new short[ sbuf ];
  ASSERT(s_vol);
  
  for (int t=0;t<EcatSizeT(uei);t++)
    {
      s_pt = s_vol;
      for (int slice=0;slice<EcatSizeZ(uei);slice++)
	{
	for(int line=0;line<EcatSizeY(uei);line++)
	  {
	    memcpy((char *)s_pt,(char *) &thing(0,line,slice,t),
		   EcatSizeX(uei) * sizeof(short));
	    s_pt  += EcatSizeX(uei);
	  }
	} 
      if(EcatWriteVolume_S16(uei,s_vol,t) == -1)
        throw logic_error( "Internal error: EcatWriteVolume_S16 failed" );
    }

  delete[] s_vol;
  EcatClose(uei);

  hdr.write();
}


void EcatWriter::write(const AimsData<float>& thing)
{
  //string fileName = removeExtension(_name);
  string fileName = _name;
  
  UnifiedEcatInfo *uei =  EcatOpen((char*)fileName.c_str(),
                                    const_cast<char*>( "w" ) );
  if (uei == ECATSHFJ_FAIL)
    throw file_error("Cannot create file ", fileName);
 

  // Champs initialisable depuis les informations contenues 
  // dans AimsData sans recours au Header de AimsData
  EcatSizeX(uei) = thing.dimX();     // Champs triviaux
  EcatSizeY(uei) = thing.dimY();
  EcatSizeZ(uei) = thing.dimZ();
  EcatSizeT(uei) = thing.dimT();
  uei->voxelsize.x = thing.sizeX();
  uei->voxelsize.y = thing.sizeY();
  uei->voxelsize.z = thing.sizeZ();
  uei->voxelsize.t = 0.0;
				    // init de internes
  uei->rawlist = (int *)calloc(EcatSizeT(uei), sizeof(int));
  uei->sortuniqlist = (long *)calloc(EcatSizeT(uei), sizeof(long));
  uei->start_time = ( int *)calloc(EcatSizeT(uei), sizeof( int));
  uei->duration_time = ( int *)calloc(EcatSizeT(uei), sizeof( int));
  uei->scale = (float *)calloc(EcatSizeT(uei), sizeof(float));
  for(int t =0;t<EcatSizeT(uei);t++)
    {
      uei->rawlist[t] = t+1;    
      uei->sortuniqlist[t] = t;    
    }
  
  // Acces au Header si disponible
  // ic tester si le header est attribue ou non.
  int	dtype = _typeINIT;
  EcatHeader	hdr( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		     thing.sizeX(), thing.sizeY(), thing.sizeZ(), 
		     thing.sizeT(), dtype, fileName );
  const PythonHeader 
    *ph = dynamic_cast<const PythonHeader *>( thing.header() );
  if( ph )
    {
      fillEcatHdr( uei, ph, true );
      hdr.copy( *ph );
      vector<int>	dims(4);
      dims[0] = thing.dimX();
      dims[1] = thing.dimY();
      dims[2] = thing.dimZ();
      dims[3] = thing.dimT();
      hdr.setProperty( "volume_dimension", dims );
      vector<float>	vs(4);
      vs[0] = thing.sizeX();
      vs[1] = thing.sizeY();
      vs[2] = thing.sizeZ();
      vs[3] = thing.sizeT();
      hdr.setProperty( "voxel_size", vs );
    }
  
  /* 512 bytes added because in EcatWriteVolume_S16, a call to
     file_data_to_host() (libecat) is performed, which only works on chunks
     of 512 bytes (!)
  */
  size_t sbuf = thing.dimX() * thing.dimY() * thing.dimZ()
    + 512 / sizeof(float);
  float *f_pt,*f_vol = new float[ sbuf ];
  ASSERT(f_vol);
  
  for (int t=0;t<EcatSizeT(uei);t++)
    {
      f_pt = f_vol;
      for (int slice=0;slice<EcatSizeZ(uei);slice++)
	{
	for(int line=0;line<EcatSizeY(uei);line++)
	  {
	    memcpy((char *)f_pt,(char *) &thing(0,line,slice,t),
		   EcatSizeX(uei) * sizeof(float));
	    f_pt  += EcatSizeX(uei);
	  }
	} 
      if(EcatWriteVolume_FLOAT(uei,f_vol,t) == -1)
        throw logic_error( "Internal error: EcatWriteVolume_FLOAT failed" );
    }

  delete[] f_vol;
  EcatClose(uei);

  hdr.write();
}


EcatWriter&
operator << (EcatWriter& writer,
	     const AimsData<short>& thing)
{
  writer.write(thing);
  return(writer);
}



EcatWriter&
operator << (EcatWriter& writer,
	     const AimsData<float>& thing)
{
  writer.write(thing);
  return(writer);
}
