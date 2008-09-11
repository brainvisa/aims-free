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
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;


template<typename T>
static bool subsample( Process &, const string &, Finder & );

class SubSample : public Process
{
public:
  SubSample(int nb, const string & fileout );
  virtual ~SubSample();

  int   _nb;
  string _fileout;
};


SubSample::SubSample( int nb, const string & fileout ) 
          : _nb( nb ), _fileout( fileout )
{
  registerProcessType( "Volume", "S8", &subsample<char> );
  registerProcessType( "Volume", "U8", &subsample<byte> );
  registerProcessType( "Volume", "S16", &subsample<short> );
  registerProcessType( "Volume", "U16", &subsample<unsigned short> );
  registerProcessType( "Volume", "FLOAT", &subsample<float> );
}


SubSample::~SubSample()
{
}


template<typename T>
bool subsample( Process & p, const string & filein, Finder & f )
{

  SubSample&   ss = (SubSample &)p;


  AimsData<T>		datar;
  Reader< AimsData<T> >	r( filein );
  string		format = f.format();


  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), false, 0.1 ) );
  r.read( datar, 0, &format );

  int	nb    = ss._nb;
  string fileout = ss._fileout; 
  assert ( datar.dimX() % nb == 0 || datar.dimY() % nb == 0);
  AimsData< T > data( datar.dimX()/nb, datar.dimY()/nb,datar.dimZ() );
  data.setSizeZ(datar.sizeZ()); 
  data.setSizeX(datar.sizeX()*nb); 
  data.setSizeY(datar.sizeY()*nb); 
 
 int x, y, z, w, v;
 float datatemp;
 
 for ( z = 0; z < datar.dimZ(); ++z )
   for ( y = 0; y < datar.dimY(); y+=nb )
     for ( x = 0; x < datar.dimX(); x+=nb )
       {
	 datatemp = 0;
	 for (w = 0; w < nb; ++w)
	   for (v = 0; v < nb; ++v) 
	     datatemp += datar(x+w,y+w,z);
	 data( int(x/nb), int(y/nb), z ) = (T) (datatemp/(nb*nb)) ;
       }


 //if( datar.header() )
 //   data.setHeader( datar.header()->cloneHeader() );
    Writer< AimsData<T> > wd(fileout);
    wd << (data);


  return true;
}

int main( int argc, const char **argv )
{
  string  filein;
  string  fileout = "subsampled";
  int   nb=2;
  

  AimsApplication	app( argc, argv, "Perform Image subsampling,"
	         			     "using average operator" );
  app.addOption( filein, "-i", "input data" );
  app.alias( "--input", "-i" );
  app.addOption( fileout, "-o", "output data");
  app.alias( "--output", "-o" );  
  app.addOption( nb, "-n", "number of voxels [default = 2 voxels in x and y"
                           " directions]",true);


  try
    {
      app.initialize();
      SubSample proc( nb, fileout );
      if( !proc.execute( filein ) )
	cout << "Couldn't process file - aborted\n";
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return 1;
    }

  return EXIT_SUCCESS;
}
