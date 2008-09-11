/* Copyright (c) 1995-2007 CEA
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


#include <aims/data/data_g.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/io/io_g.h>
#include <aims/data/pheader.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <iostream>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;

template<typename T> static 
bool makemapvol( Process &, const string &, Finder & );
template<typename T> static 
bool makemapbck( Process &, const string &, Finder & );
template<> /*static*/
bool makemapbck<Void>( Process &, const string &, Finder & );


class FrontMap : public Process
{
public:
  FrontMap();
  virtual ~FrontMap() {}

  string	fileout;
  double	val_domain;
  double	val_outside;
  int		xmask;
  int		ymask;
  int		zmask;
  float		factor;
};


FrontMap::FrontMap()
  : Process(), val_domain( 0 ), val_outside( -1 ), 
    xmask( 3 ), ymask( 3 ), zmask( 3 ), factor( 50 )
{
  registerProcessType( "Volume", "S16", &makemapvol<int16_t> );
  registerProcessType( "Volume", "FLOAT", &makemapvol<float> );
  registerProcessType( "Bucket", "S16", &makemapbck<int16_t> );
  registerProcessType( "Bucket", "FLOAT", &makemapbck<float> );
  registerProcessType( "Bucket", "VOID", &makemapbck<Void> );
}


template<typename T> 
bool makemapvol( Process & p, const string & filein, Finder & f )
{
  FrontMap		& fp = (FrontMap &) p;
  AimsData<T>		vol;
  string		format = f.format();
  Reader<AimsData<T> >	reader( filein );
  assert( reader.read( vol,1, &format ) );
 
  AimsDistanceFrontPropagation( vol, (T) fp.val_domain, (T) fp.val_outside,
                                fp.xmask, fp.ymask, fp.zmask, fp.factor, 
				false );

  PythonHeader	*h = dynamic_cast<PythonHeader *>( vol.header() );
  if ( h )
    h->removeProperty( "scale_factor" );
  Writer<AimsData<T> > writer( fp.fileout );
  writer.write( vol );
  return true;
}


template<typename T> 
bool makemapbck( Process & p, const string & filein, Finder & f )
{
  FrontMap		& fp = (FrontMap &) p;
  BucketMap<T>		vol;
  string		format = f.format();
  Reader<BucketMap<T> >	reader( filein );
  assert( reader.read( vol,1, &format ) );
 
  AimsDistanceFrontPropagation( vol, (T) fp.val_domain, (T) fp.val_outside,
                                fp.xmask, fp.ymask, fp.zmask, fp.factor, 
				false );

  Writer<BucketMap<T> > writer( fp.fileout );
  writer.write( vol );
  return true;
}


template <>
bool makemapbck<Void>( Process & p, const string & filein, Finder & f )
{
  FrontMap		& fp = (FrontMap &) p;
  BucketMap<Void>	vol;
  string		format = f.format();
  Reader<BucketMap<Void> >	reader( filein );
  assert( reader.read( vol,1, &format ) );

  BucketMap<float>	bck;
  bck.setSizeXYZT( vol.sizeX(), vol.sizeY(), vol.sizeZ(), vol.sizeT() );
  BucketMap<Void>::iterator	i, e = vol.end();
  BucketMap<Void>::Bucket::iterator	ib, eb;
  for( i=vol.begin(); i!=e; ++i )
    {
      BucketMap<float>::Bucket	& bk = bck[ i->first ];
      for( ib=i->second.begin(), eb=i->second.end(); ib!=eb; ++ib )
        bk[ ib->first ] = fp.val_domain;
    }

  AimsDistanceFrontPropagation( bck, (float) fp.val_domain, 
                                (float) fp.val_outside,
                                fp.xmask, fp.ymask, fp.zmask, fp.factor, 
				false );

  Writer<BucketMap<float> > writer( fp.fileout );
  writer.write( bck );
  return true;
}


int main( int argc, const char **argv )
{
  FrontMap	proc;
  ProcessInput	pi( proc );

  AimsApplication	app( argc, argv, "Chamfer distance map by front " 
                             "propagation\n" 
                             "Propagation starts from voxels with any value " 
                             "not in (<forbidden>, <domain>) and propagates " 
                             "inside voxels with value <domain>. All voxel " 
                             "values different from <forbidden> and <domain> " 
                             "are considered to be the seed (zero distance)" );

  app.addOption( pi, "-i", "source label data" );
  app.addOption( proc.fileout, "-o", "output distance map" );
  app.addOption( proc.val_domain, "-d", "valid domain label where to " 
                 "propagate the diagram [default=0]", true );
  app.addOption( proc.val_outside, "-f", "forbidden domain [default=-1]", 
                 true );
  app.addOption( proc.xmask, "-x", "X size of the distance mask [default=3]", 
                 true );
  app.addOption( proc.ymask, "-y", "Y size of the distance mask [default=3]", 
                 true );
  app.addOption( proc.zmask, "-z", "Z size of the distance mask [default=3]", 
                 true );
  app.addOption( proc.factor, "-F", "chamfer multiplication factor " 
                 "[default=50]", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--domain", "-d" );
  app.alias( "--forbidden", "-f" );
  app.alias( "--xmask", "-x" );
  app.alias( "--ymask", "-y" );
  app.alias( "--zmask", "-z" );
  app.alias( "--factor", "-F" );

  try
    {
      app.initialize();

      if ( !proc.execute( pi.filename ) )
        {
          cerr << "Failed\n";
          return EXIT_FAILURE;
        }
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
