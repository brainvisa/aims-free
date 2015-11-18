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


#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/morphology/morphology_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/distancemap/chamfer.h>

using namespace aims;
using namespace carto;
using namespace std;


float AimsDistanceToBorder(AimsData<short> &vol)
{
  int x,y,z;
  int dx = vol.dimX() ,dy = vol.dimY(),dz = vol.dimZ();
  int distance = dx + dy + dz; 
   
  for(x=0; x < dx; ++x)
    for(y=0; y < dy; ++y)
      for(z=0; z < dz; ++z)	
	if ( vol(x,y,z) == 32767 ) 
	  {
	    if (x < distance) 
	      distance = x ;
	    else
	      if ( (dx - x) < distance && (dx - x) >= 0 )
		distance = dx - x;
	    
	    if (y < distance) 
	      distance = y ;
	    else
	      if ( (dy - y) < distance && (dy - y) >= 0 )
		distance = dy - y;

	    if (z < distance) 
	      distance = z ;
	    else
	      if ( (dz - z) < distance && (dz - z) >= 0 )
		distance = dz - z;
	  }

  return (distance);
  
}


int main( int argc, const char **argv )
{
  Reader<AimsData<short> > reader;
  Writer<AimsData<short> > writer;
  int	xmask = 3,ymask = 3,zmask = 3;
  float	radius,factor = 50;

  AimsApplication	app( argc, argv, "Chamfer closing on a volume" );

  app.addOption( reader, "-i", "source volume" );
  app.addOption( writer, "-o", "destination volume" );
  app.addOption( radius, "-e", "radius of the structuring element" );
  app.addOption( xmask, "-x", "X size of the distance mask [default=3]", 
                 true );
  app.addOption( ymask, "-y", "Y size of the distance mask [default=3]", 
                 true );
  app.addOption( zmask, "-z", "Z size of the distance mask [default=3]", 
                 true );
  app.addOption( factor, "-f", "chamfer multiplication factor [default=50]", 
                 true );

  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--eradius", "-e" );
  app.alias( "-r", "-e" );
  app.alias( "--radius", "-e" );
  app.alias( "--xmask", "-x" );
  app.alias( "--ymask", "-y" );
  app.alias( "--zmask", "-z" );
  app.alias( "--factor", "-f" );

  try
    {
      app.initialize();

      AimsData<short> vol;
      
      		        
      set<unsigned> s;
      s.insert(xmask);
      s.insert(ymask);
      s.insert(zmask);
      float dimMax = *s.rbegin();
      set<float> f;
      f.insert( vol.sizeX() );
      f.insert( vol.sizeY() );
      f.insert( vol.sizeZ() ); 
      int offset ;
      offset = (int) (radius / *f.rbegin() )  ;
      int borderW = (int ) (dimMax - 1) / 2 ;
      
      reader.read( vol, borderW );
      
      float dist;
      dist = AimsDistanceToBorder(vol);
      
     
      
      
      if (dist <= offset )
	{
	  cout << "The distance between the object and the border is less than the radius \n";

	  AimsData<short> cloBig;
	  AimsData<short> volBig(vol.dimX() + 2 * offset, 
				 vol.dimY() + 2 * offset, 
				 vol.dimZ() + 2 * offset,
				 1, borderW);

	  volBig.setSizeXYZT(vol);

	  int x,y,z;
	  int dx = vol.dimX() ,dy = vol.dimY(),dz = vol.dimZ();
            
	  for(x=0; x < dx; ++x)
	    for(y=0; y < dy; ++y)
	      for(z=0; z < dz; ++z)
		volBig(x + offset, y + offset, z+offset) 
		  = vol(x,y,z);
 	  
	  cloBig = AimsMorphoChamferClosing( volBig, radius, 
					     xmask, ymask, zmask, 
					     factor );
      
	  
	  AimsData<short> clo(vol.dimX() , vol.dimY() , vol.dimZ() );
	  clo.setSizeXYZT(vol);
	  
	  for(x=0; x < dx; ++x)
	    for(y=0; y < dy; ++y)
	      for(z=0; z < dz; ++z)
		clo(x ,y ,z) = cloBig(x + offset,y + offset,z+offset); 
       
	  writer.write( clo );
	 
	}
      else
	{
	  
	  AimsData<short> clo;
	  clo = AimsMorphoChamferClosing( vol, radius, xmask, ymask, zmask, 
					  factor );
	  
	  writer.write( clo );
      
	}
      
     
      return EXIT_SUCCESS;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return EXIT_FAILURE;
}
