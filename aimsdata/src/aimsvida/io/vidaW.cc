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
 *  VIDA data writer class
 */
#include <string>
#include <aims/def/assert.h>
#include <aims/io/vidaheader.h>
#include <aims/data/data.h>
#include <aims/io/vidaW.h>
#include <cartobase/exception/ioexcept.h>

using namespace aims;
using namespace carto;
using namespace std;


void VidaWriter::write(const AimsData<short>& thing)
{
  VidaHeader	hdr( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		     thing.sizeX(), thing.sizeY(), thing.sizeZ(), 
		     thing.sizeT(), _name );
  string fileName = hdr.removeExtension(_name);

  VIDAim *vp =  VidaOpen((char*)fileName.c_str(),VidaConsigne(VMODE,"w",
                                          VMDTYPE,   FX_SHORT_FX,
                                          VSIZE_X,   (int)thing.dimX(), 
		                          VSIZE_Y,   (int)thing.dimY(),
		                          VSIZE_Z,   (int)thing.dimZ(),
		                          VSIZE_T,   (int)thing.dimT(),
		                          VSIZE_PX,  (double)thing.sizeX(),
		                          VSIZE_PY,  (double)thing.sizeY(),
		                          VSIZE_PZ,  (double)thing.sizeZ(),
		                          VSIZE_PT,  (double)thing.sizeT(),
		                          NULL));
   if( !vp )
     io_error::launchErrnoExcept( _name );

   short *s_pt,*s_ima = new short[ thing.dimX() * thing.dimY() ];
   ASSERT(s_ima);
   bool ok = true;
   for (int frame=0;frame<VIDA_T(vp);frame++)
   {
     for (int slice=0;slice<VIDA_Z(vp);slice++)
     {
       s_pt = s_ima;
       for(int line=0;line<VIDA_Y(vp);line++)
       {
         memcpy((char *)s_pt,(char *)&thing(0,line,slice,frame),
                VIDA_X(vp)*sizeof(short));
         s_pt  += VIDA_X(vp);
       }
       if(VidaWrite((char *)s_ima,slice,frame,vp) == -1)
         ok = false;
     } 
   }
   delete[] s_ima;
   VidaClose(vp);
   if( !ok )
     io_error::launchErrnoExcept( _name );

    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    if( ph )
    {
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
    hdr.write();
}


void VidaWriter::write(const AimsData<float>& thing)
{
  VidaHeader	hdr( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		     thing.sizeX(), thing.sizeY(), thing.sizeZ(), 
		     thing.sizeT(), _name );
  string fileName = hdr.removeExtension(_name);

  VIDAim *vp = VidaOpen( fileName.c_str(), 
                         VidaConsigne( VMODE,"w",
                                       VMDTYPE,   FL_FLOAT_FL,
                                       VSIZE_X,   (int)thing.dimX(), 
                                       VSIZE_Y,   (int)thing.dimY(),
                                       VSIZE_Z,   (int)thing.dimZ(),
                                       VSIZE_T,   (int)thing.dimT(),
                                       VSIZE_PX,  (double)thing.sizeX(),
                                       VSIZE_PY,  (double)thing.sizeY(),
                                       VSIZE_PZ,  (double)thing.sizeZ(),
                                       VSIZE_PT,  (double)thing.sizeT(),
                                       VUNITE,    VIDA_SANSUNITE,
                                       NULL ) );
   ASSERT( vp != NULL ); 

   float *f_pt,*f_ima = new float[ thing.dimX() * thing.dimY() ];
   ASSERT(f_ima);
            
   for (int frame=0;frame<VIDA_T(vp);frame++)
   { for (int slice=0;slice<VIDA_Z(vp);slice++)
     { f_pt = f_ima;
       for(int line=0;line<VIDA_Y(vp);line++)
       { memcpy((char *)f_pt,(char *)&thing(0,line,slice,frame),
                VIDA_X(vp)*sizeof(float));
         f_pt  += VIDA_X(vp);
       }
       if(VidaWrite((char *)f_ima,slice,frame,vp) == -1)
        throw logic_error( "Internal error: write failed" );
     } 
   }
   delete[] f_ima;
   VidaClose(vp);

    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    if( ph )
      hdr.copy( *ph );
    hdr.write();
}


VidaWriter&
operator << (VidaWriter& writer,
             const AimsData<short>& thing)
{ writer.write(thing);
  return(writer);
}



VidaWriter&
operator << (VidaWriter& writer,
             const AimsData<float>& thing)
{ writer.write(thing);
  return(writer);
}






