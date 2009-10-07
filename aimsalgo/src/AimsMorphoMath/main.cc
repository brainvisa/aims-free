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
#include <aims/io/io_g.h>
#include <aims/io/writer.h> 
#include <aims/io/reader.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/process.h>


using namespace aims;
using namespace carto;
using namespace std;


class Morpho : public Process
{
public:
  Morpho();
  virtual ~Morpho();

  template <typename T> static bool morpho( Process & p, const string & fileIn, Finder & f );
  template <typename T> AimsData< T > doErosion( AimsData< T >& dataIn, float & radius ); 
  template <typename T> AimsData< T > doDilation( AimsData< T >& dataIn, float & radius ); 
  template <typename T> AimsData< T > doClosing( AimsData< T >& dataIn, float & radius ); 
  template <typename T> AimsData< T > doOpening( AimsData< T >& dataIn, float & radius ); 

  vector< Point3d > doStructElement( float & radius, Point4df voxelsize );
  Point3d computeIntRadius( float & radius, Point4df voxelsize);

  float radius;
  Point3d int_radius;
  vector< Point3d > list;
  string fileOut, mode;
};


Morpho::Morpho() 
{
  registerProcessType( "Volume", "S8",     &morpho<int8_t> );
  registerProcessType( "Volume", "U8",     &morpho<uint8_t> );
  registerProcessType( "Volume", "U16",    &morpho<uint16_t> );
  registerProcessType( "Volume", "S16",    &morpho<int16_t> );
  registerProcessType( "Volume", "S32",    &morpho<int32_t> );
  registerProcessType( "Volume", "U32",    &morpho<uint32_t> );
  registerProcessType( "Volume", "FLOAT",  &morpho<float> );
  registerProcessType( "Volume", "DOUBLE", &morpho<double> );
}


Morpho::~Morpho()
{
}

Point3d Morpho::computeIntRadius( float & radius, Point4df voxelsize )
{
  int_radius[0] = int(rint( radius/voxelsize[0] ));
  int_radius[1] = int(rint( radius/voxelsize[1] ));
  int_radius[2] = int(rint( radius/voxelsize[2] ));

  cout << "Radius = " << radius << " mm" << endl;
  cout << "Voxel size = " << voxelsize << " mm" << endl;
  cout << "Integer radius = " << int_radius << " voxel(s)" << endl;

  // Test on dimension
  if( (int_radius[0]==0) || (int_radius[1]==0) || (int_radius[2]==0) )
    {
      cout << "WARNING: Radius too small according to voxelsize !!! STOP.\n" << endl;
      exit(1);
    }

  if( (int_radius[0]==1) || (int_radius[1]==1) || (int_radius[2]==1) )
    {
      cout << "WARNING: Radius generate a single voxel !!! STOP.\n" << endl;
      exit(2);
    }

  return int_radius;
}



vector< Point3d > Morpho::doStructElement( float & radius, Point4df voxelsize )
{
//   AimsData< int16_t > sphere( 2*int_radius[0]-1,
// 			      2*int_radius[1]-1,
// 			      2*int_radius[2]-1 );
//   sphere.setSizeXYZT( voxelsize[0],
// 		      voxelsize[1],
// 		      voxelsize[2] );
//   sphere = 0;

  //   Test if list have been already created
  //   List of the voxel of the strust elem
  if( !list.size() )
    {
      for(int u = 0; u < 2*int_radius[0]-1; ++u)
	for(int v = 0; v < 2*int_radius[1]-1; ++v)
	  for(int w = 0; w < 2*int_radius[2]-1; ++w)
	    {
	      if( sqrt(  sqr( float(u - (int_radius[0]-1)) * voxelsize[0] ) +
			 sqr( float(v - (int_radius[1]-1)) * voxelsize[1] ) +
			 sqr( float(w - (int_radius[2]-1)) * voxelsize[2] )) <= radius ) 
		{
		  list.push_back( Point3d(u, v, w) );
		  // 	      sphere(u, v, w) = 1;
		}
	    }
      //   Writer< AimsData< int16_t > > ws( "sphere.ima" );
      //   ws.write( sphere );
      cout << "Number of voxels in the list = " << list.size() << endl << endl;
      return list;
    }
  else return list;
}


template<typename T>
AimsData< T > Morpho::doErosion( AimsData< T >& dataIn, float & radius )
{
  cout << "EROSION" << endl;
  T value;
  value = dataIn.maximum();
    
  Point4df voxelsize( dataIn.sizeX(),
		      dataIn.sizeY(),
		      dataIn.sizeZ(),
		      dataIn.sizeT() );

  Point4d dim( dataIn.dimX(),
	       dataIn.dimY(),
	       dataIn.dimZ(),
	       dataIn.dimT() );

  AimsData< T > dataOut;
  dataOut = dataIn.clone();
  dataOut = T(0);

  int_radius = computeIntRadius( radius, voxelsize );
  list = doStructElement( radius, voxelsize );
    
  for(int t = 0; t < dim[3]; ++t)
    for(int z = 0; z < dim[2]; ++z)
      {
	//	float pct = float(z*100./dim[2]);
	// 	cout << "Pourcentage effectue : " << pct << " %\t";
	for(int y = 0; y < dim[1]; ++y)
	  for(int x = 0; x < dim[0]; ++x)
	    {
	      T min = value, tmp;
	      for(int l = 0; l < int(list.size()); ++l)
		{
		  Point3d coord( x + list[l][0] - (int_radius[0]-1),
				 y + list[l][1] - (int_radius[1]-1),
				 z + list[l][2] - (int_radius[2]-1) ); 
		  
		  if( (coord[0]>=0) && (coord[0]<dim[0] )
		      && (coord[1]>=0) && (coord[1]<dim[1] )
		      && (coord[2]>=0) && (coord[2]<dim[2] ) )
		    {
		      tmp = dataIn( coord[0], coord[1], coord[2] );
		      if( tmp < min ) min = tmp; 
		    }
		}
	      dataOut(x, y, z, t) = min;
	    }
      }
  cout << endl;
  return dataOut;
}  


template<typename T>
AimsData< T > Morpho::doDilation( AimsData< T >& dataIn, float & radius )
{
  cout << "DILATION" << endl;
  T value;
  value = dataIn.minimum();

  Point4df voxelsize( dataIn.sizeX(),
		      dataIn.sizeY(),
		      dataIn.sizeZ(),
		      dataIn.sizeT() );

  Point4d dim( dataIn.dimX(),
	       dataIn.dimY(),
	       dataIn.dimZ(),
	       dataIn.dimT() );
  
  AimsData< T > dataOut( dim[0], dim[1], dim[2], dim[3] );
  dataOut = dataIn.clone();
  dataOut = T(0);

  int_radius = computeIntRadius( radius, voxelsize );
  list = doStructElement( radius, voxelsize );
  
  for(int t = 0; t < dim[3]; ++t)
    for(int z = 0; z < dim[2]; ++z)
      {
	float pct = float(z*100./dim[2]);
	cout << "Pourcentage effectue : " << pct << " %\t";
	for(int y = 0; y < dim[1]; ++y)
	  for(int x = 0; x < dim[0]; ++x)
	    {
	      T max = value, tmp;
	      for(int l = 0; l < int(list.size()); ++l)
		{
		  Point3d coord( x + list[l][0] - (int_radius[0]-1),
				 y + list[l][1] - (int_radius[1]-1),
				 z + list[l][2] - (int_radius[2]-1) );
		  
		  if( (coord[0]>=0) && (coord[0]<dim[0] )
		      && (coord[1]>=0) && (coord[1]<dim[1] )
		      && (coord[2]>=0) && (coord[2]<dim[2] ) )
		    {
		      tmp = dataIn( coord[0], coord[1], coord[2] );
		      if( tmp > max ) max = tmp;
		    }
		}
	      dataOut(x, y, z, t) = max;
	    }   
      }
  cout << endl;
  return dataOut;
}  


template<typename T>
AimsData< T > Morpho::doClosing( AimsData< T >& dataIn, float & radius )
{
  AimsData< T > dataOut;
  dataOut = doDilation( dataIn, radius );
  dataOut = doErosion( dataOut, radius );
  
  return dataOut;
}


template<typename T>
AimsData< T > Morpho::doOpening( AimsData< T >& dataIn, float & radius )
{
  AimsData< T > dataOut;
  dataOut = doErosion( dataIn, radius );
  dataOut = doDilation( dataOut, radius );
  
  return dataOut;
}


template<typename T>
bool Morpho::morpho( Process & p, const string & fileIn, Finder & f )
{
  Morpho &m = (Morpho &) p;

  AimsData<T>		dataIn, dataOut;
  Reader< AimsData<T> >	r( fileIn );
  string		format = f.format();
  r.read( dataIn, 0, &format );

  Point4df voxelsize( dataIn.sizeX(),
		      dataIn.sizeY(),
		      dataIn.sizeZ(),
		      dataIn.sizeT() );

  Point4d dim = ( dataIn.dimX(),
		  dataIn.dimY(),
		  dataIn.dimZ(),
		  dataIn.dimT() );

  // Specific operation
  int choice;
  if(m.mode=="ero") choice = 0;
  if(m.mode=="dil") choice = 1;
  if(m.mode=="clo") choice = 2;
  if(m.mode=="ope") choice = 3;

  switch ( choice )
    { 
    case 0 :
      cout << "\nMorphological operator: EROSION" << endl << endl; 
      dataOut = m.doErosion( dataIn, m.radius );
      break;

    case 1 :
      cout << "\nMorphological operator: DILATION" << endl << endl; 
      dataOut = m.doDilation( dataIn, m.radius );
      break;  

    case 2 :
      cout << "\nMorphological operator: CLOSING" << endl << endl;
      dataOut = m.doClosing( dataIn, m.radius );
      break;  

    case 3 :
      cout << "\nMorphological operator: OPENING" << endl << endl;
      dataOut = m.doOpening( dataIn, m.radius );
      break;  
    }
  
  // Ecriture du fichier
  Writer< AimsData< T > > w( m.fileOut );
  cout << "Writing result..." << endl;
  w.write( dataOut );
  cout << "End of the process." << endl;
  
  return true;
}   

int main( int argc, const char **argv )
  
{
  int result = EXIT_SUCCESS;

  Morpho proc;
  ProcessInput	fileIn( proc );
  string fileOut;
  string mode;
  float radius;

  AimsApplication application( argc, argv, "Morphological operators (erosion, dilation, closing, opening)"
			       "\nfor both binary and gray level intensity images" );
  application.addOption( fileIn, "-i", "Input image", 1 );
  application.addOption( radius, "-r", "Radius parameter (in mm)", 1 );
  application.addOption( mode, "-m", "Mode that can be:\n\n" 
			 "ero --> erosion\n" 
			 "dil --> dilation\n" 
			 "clo --> closing\n"
			 "ope --> opening", 1 );
  application.addOption( fileOut, "-o", "Output image", 1 );
  
  try 
    {
      
      application.initialize();
      
      proc.radius = radius;
      proc.fileOut = fileOut;
      proc.mode = mode;

      if( !proc.execute( fileIn.filename ) )
	cout << "Couldn't process file - aborted\n";
      
    }
  catch( user_interruption &e ) {}
  catch( std::exception &e ) 
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl; 
      result = EXIT_FAILURE;
    }
  return result;  
}

