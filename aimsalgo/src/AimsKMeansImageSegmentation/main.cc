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

#include <cartobase/smart/rcptr.h>
#include <aims/data/fastAllocationData.h>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/classification/iterativeclassification_d.h>
#include <aims/classification/kmeansstrategy.h>
#include <aims/classification/individuals.h>
#include <cmath>
#include <map>
#include <set>
#include <queue>
#include <list>
#include <utility>


using namespace carto;
using namespace aims;
using namespace std;

struct PointLess : public std::binary_function< Point3d, Point3d , bool>
{
  bool operator () ( const Point3d & p1, const Point3d & p2 ) const
  {
    return( p1[2] < p2[2] 
	    || ( (p1[2] == p2[2]) && (p1[1] < p2[1])  )
	    || ( (p1[2] == p2[2]) 
		 && (p1[1] == p2[1]) && (p1[0] < p2[0]) ) ) ;
  }
};

inline
bool 
in( const Point3d& dims, const Point3d& p )
{
  if ( p[0] < 0 || p[0] > dims[0] - 1 ||  
       p[1] < 0 || p[1] > dims[1] - 1 ||
       p[2] < 0 || p[2] > dims[2] - 1 )
    return false ;
  
  return true ;
}

template <class T>
void
data2OptimizedFormat( AimsData<T>& data, const AimsData<short>& mask, 
		      const Point3d& radiusAround, 
		      AimsData<float>& totalIndivMatrix, AimsData<int>& localisationMatrix, 
		      AimsData<short>& dilatedMask )
{
  cout << "radius around = " << radiusAround << endl ;
  int x, y, z ;
  int nbVoxels = 0 ;
  
  AllocatorContext ac( &MemoryAllocator::singleton() ) ; 
  dilatedMask = AimsData<short>( mask.clone() ) ; // = AimsMorphoChamferDilation( mask, radiusAround ) ;

  ForEach3d(mask, x, y, z)
    if( mask(x, y, z) ){
      for(int dx = -radiusAround[0] ; dx <= radiusAround[0] ; ++dx )
	for(int dy = -radiusAround[1] ; dy <= radiusAround[1] ; ++dy )
	  for(int dz = -radiusAround[2] ; dz <= radiusAround[2] ; ++dz )
	    if( x+dx >= 0 && x+dx < dilatedMask.dimX() &&
		y+dy >= 0 && y+dy < dilatedMask.dimY() && 
		z+dz >= 0 && z+dz < dilatedMask.dimZ() )
	      dilatedMask(x+dx, y+dy, z+dz) = 1 ;
    }
  ForEach3d(dilatedMask, x, y, z)
    if( dilatedMask(x, y, z) )
      ++nbVoxels ;
  
  float minVoxelSize = data.sizeX() ;
  if( minVoxelSize > data.sizeY() )
    minVoxelSize = data.sizeY() ;
  if( minVoxelSize > data.sizeZ() )
    minVoxelSize = data.sizeZ() ;

  totalIndivMatrix = AimsData<float>( nbVoxels, data.dimT(), 1, 1, 0, ac ) ;
  localisationMatrix = AimsData<int>( data.dimX(), data.dimY(), data.dimZ(), 1, 0, ac/*, 
				      int(radiusAround/ minVoxelSize )*/ ) ;
  localisationMatrix.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ()) ;
  
  int ind = 0 ; 
  
  ForEach3d( data, x, y, z )
    if( dilatedMask(x, y, z) ){
      localisationMatrix(x, y, z) = ind ;
      for(int t = 0 ; t < data.dimT() ; ++t )
	totalIndivMatrix(ind, t) =  data( x, y, z, t ) ;
      
      ++ind ;
    } else
      localisationMatrix(x, y, z) = -1 ;
  
  return ;
}

template<class T>
inline void
extractIndividualFromMatrix( const AimsData<float>& totalIndivMatrix, int ind, AimsData<T>& indiv )
{
  if( indiv.dimX() != totalIndivMatrix.dimY() )
    indiv = AimsFastAllocationData<T>( totalIndivMatrix.dimY() ) ;
  for( int t = 0 ; t < totalIndivMatrix.dimY() ; ++t )
    indiv(t) = T(totalIndivMatrix(ind, t)) ;
}

inline bool
extractIndividualsFromMatrix( const AimsData<float>& totalIndivMatrix, 
			      const AimsData<int>& localisationMatrix,
			      const list<Point3d>& selectedPoints,
			      AimsData<float>& mean,
			      AimsData<float>& individuals )
{
  mean = AimsFastAllocationData<float>( totalIndivMatrix.dimY() ) ;

  if( individuals.dimX() != int(selectedPoints.size()) || individuals.dimY() != totalIndivMatrix.dimY() )
    individuals = AimsFastAllocationData<float>( selectedPoints.size(), totalIndivMatrix.dimY() ) ;
  
  int ind = 0 ;
  for(list<Point3d>::const_iterator it = selectedPoints.begin() ; 
      it != selectedPoints.end() ; ++it, ++ind )
    for( int t = 0 ; t < totalIndivMatrix.dimY() ; ++t ){
      individuals(ind, t) = totalIndivMatrix( localisationMatrix(*it), t ) ;
      mean(t) += individuals(ind, t) ;
    }
  for( int t = 0 ; t < totalIndivMatrix.dimY() ; ++t )
    mean(t) /= individuals.dimX() ;
  
  if( selectedPoints.size() < 1.5 * totalIndivMatrix.dimT() )
    return false ;
  return true ;
}

inline bool
extractIndividualsFromMatrix( const AimsData<float>& totalIndivMatrix, 
			      const AimsData<int>& localisationMatrix,
			      const Point3d& center,
			      const Point3d& radius,
			      AimsData<float>& mean,
			      AimsData<float>& individuals )
{
  mean = AimsFastAllocationData<float>( totalIndivMatrix.dimY() ) ;
  if( individuals.dimX() != (2*radius[2] + 1) * (2*radius[1] + 1) * (2*radius[0] + 1) 
      || individuals.dimY() != totalIndivMatrix.dimY() )
    individuals = AimsFastAllocationData<float>( (2*radius[2] + 1) * (2*radius[1] + 1) * (2*radius[0] + 1), 
				   totalIndivMatrix.dimY() ) ;
  
  int indTotal, nbInd = 0, ind = 0 ;
  float val ;
  
  Point3d neigh, dims(localisationMatrix.dimX(), localisationMatrix.dimY(), localisationMatrix.dimZ() ) ;
  for( int dz = -radius[2] ; dz <= radius[2] ; ++dz )
    for( int dy = -radius[1] ; dy <= radius[1] ; ++dy )
      for( int dx = -radius[0] ; dx <= radius[0] ; ++dx, ++ind ){
	neigh = center + Point3d(dx, dy, dz) ;
	if( in( dims, neigh) ){
	  indTotal = localisationMatrix( neigh ) ;
	  if( indTotal >= 0 && indTotal < totalIndivMatrix.dimX() ){
	    ++nbInd ;
	    for( int t = 0 ; t < totalIndivMatrix.dimY() ; ++t )
	      {
		val = totalIndivMatrix( indTotal, t ) ;
		individuals(ind, t) = val  ;
		mean(t) += val ;
	      }
	  } else
	    cout << "Point p " << neigh << " indiv outside totalIndiv Matrix : ind " << localisationMatrix( neigh ) 
		 << " & nb indivs : "  << totalIndivMatrix.dimX() << endl ;
	}
      }
  for( int t = 0 ; t < totalIndivMatrix.dimY() ; ++t )
    mean(t) /= individuals.dimX() ;
  
  if( nbInd < 1.5 * totalIndivMatrix.dimT() )
    return false ;
  
  return true ;
}


inline void
weightTimeVectors( AimsData<float>& indivMatrix, 
		   const AimsData<float>& weights )
{
  if( weights.dimX() != indivMatrix.dimY() ){
    cerr << "weights.dimX() != indivMatrix.dimY() !!!" << endl ;
    throw runtime_error("weights.dimX() != indivMatrix.dimY()") ;
  }
  
  for(int ind = 0 ; ind < indivMatrix.dimX() ; ++ind )
    for( int t = 0 ; t < indivMatrix.dimY() ; ++t )
      indivMatrix(ind, t) = indivMatrix( ind, t ) * weights[t] ;
}

inline void
projectIndividualOnLinearModel( const AimsData<float>& indivMatrix, int ind, 
				const AimsData<float>& modelMean, const AimsData<float>& modelDirVect,
				AimsData<float>& projection, AimsData<float>& residual )
{
  if( projection.dimX() != indivMatrix.dimY() )
    projection = AimsFastAllocationData<float>(indivMatrix.dimY()) ;
  if( residual.dimX() != indivMatrix.dimY() )
    residual = AimsFastAllocationData<float>(indivMatrix.dimY()) ;
  
  float dot = 0. ;
  for(int t = 0 ; t < indivMatrix.dimY() ; ++t )
    dot += (indivMatrix(ind, t) - modelMean(t)) * modelDirVect(t) ;
  
  projection = modelMean + modelDirVect * dot ;
  for( int t = 0 ; t < indivMatrix.dimY() ; ++t )
    residual(t) = indivMatrix(ind, t) - projection(t) ;
}

template <class T>
AimsData<T>
getImageFromMapped( const AimsData<int>& locMap,
		    const AimsData<T>& mapped, const AimsData<short>& mask )
{
  AllocatorContext ac( &MemoryAllocator::singleton() ) ; 

  AimsData<T> res(locMap.dimX(), locMap.dimY(), locMap.dimZ(), mapped.dimY(), 0, ac ) ;
  res.setSizeXYZT( locMap.sizeX(), locMap.sizeY(), locMap.sizeZ(), 1.0 ) ;
  int x, y, z ;
  ForEach3d(locMap, x, y, z ){
    int ind  = locMap( x, y, z ) ;
    if( mask(x, y, z) && ind < mapped.dimX() ){
      for(int t = 0 ; t < mapped.dimY() ; ++t )
	res(x, y, z, t ) = mapped( locMap(x, y, z), t ) ;
    }
  }
  return res ;
}

template <class T>
AimsData<T>
getImageFromVector( const AimsData<int>& locMap,
		    const vector<T>& mapped, const AimsData<short>& mask )
{
  AllocatorContext ac( &MemoryAllocator::singleton() ) ; 

  AimsData<T> res(locMap.dimX(), locMap.dimY(), locMap.dimZ(), 1, 0, ac ) ;
  res.setSizeXYZT( locMap.sizeX(), locMap.sizeY(), locMap.sizeZ(), 1.0 ) ;
  res = -1 ;
  int x, y, z ;
  ForEach3d(locMap, x, y, z )
    if( mask( x, y, z ) ){
      int ind = locMap(x, y, z) ;
      if( ind > int(mapped.size()) ){
	//cerr << "Transformation from vector to image : Bad vector to map !! " << endl ;
      }else
	res(x, y, z ) = mapped[ locMap(x, y, z) ] ;
    }
  return res ;
}

template<class T>
static bool doit( Process &, const string &, Finder & );

class KMeansSegmentation : public Process
{
public:
  KMeansSegmentation( const string & fout, const string & fileMask,  
		      int nbOfClasses, int nbRuns ) ;
  
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

private:
  string	_fileout;
  string	_filemask;
  int           _nbClasses ;
  int           _nbRuns ;
};

KMeansSegmentation::KMeansSegmentation( const string & fout, const string & fileMask,  
				        int nbOfClasses, int nbRuns ) :
  _fileout(fout), _filemask(fileMask), _nbClasses(nbOfClasses), _nbRuns(nbRuns)
{
  registerProcessType( "Volume", "S8", &doit<char> );
  registerProcessType( "Volume", "S16", &doit<short> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
}

template<class T> bool 
doit( Process & pr, const string & fname, Finder & f )
{  
  // ----------------------------------------------
  //testDA() ;
  
  KMeansSegmentation		& rgp = (KMeansSegmentation &) pr;
  AimsData<T>		data;
  string		format = f.format();
  Reader<AimsData<T> >	r( fname );
  cout << "reading volume...\n";
  if( !r.read( data, 4, &format ) )
    return( false );

  
  AimsData<short> mask ;
  AimsData<short> dilMask ;
  
  if( rgp._filemask != "" ){
    Reader<AimsData<short> >	r2( rgp._filemask );
    if( !r2.read( mask ) )
      return( false );
  } else {
    mask = AimsData<short>(data.dimX(), data.dimY(), data.dimZ() ) ;
    mask.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ() ) ;
    mask = 1 ;
  }
  
  int x, y, z ;
  ForEach3d(mask, x, y, z){
    bool keepPoint = false ;
    for( int t = 0 ; t < data.dimT() ; ++t )
      if( data( x, y, z, t) != 0. )
	keepPoint = true ;
    if( !keepPoint )
      mask( x, y, z) = 0 ;
  }
  
  AimsData<int> locMap ;
  AimsData<float> indivMatrix ;
  
  cout << "data to opt format" << endl ;

  data2OptimizedFormat( data, mask, 
			Point3d(1, 1, 1), 
			indivMatrix, locMap, 
			dilMask ) ;
  
  cout << "Data2opt done !" << endl ;
  data = AimsData<T>(1) ;
  
  vector<float> values(indivMatrix.dimY()) ;
  std::vector< std::list< aims::Individuals<float> > > pts(rgp._nbClasses+1) ;
  ForEach3d(locMap, x, y, z){
    if( mask( x, y, z) ){
      for(int t = 0 ; t < indivMatrix.dimY() ; ++t )
	values[t] = indivMatrix( locMap(x, y, z), t ) ;
      
      Individuals<float> indiv( Point3df(x, y, z), values) ;
      pts[0].push_back( indiv ) ;
    }
  }
  
  KmeansStrategy<float> kmeansStrat( 1000, aims::NORM2SQR, 0, indivMatrix.dimY() - 1, 
				     std::vector< aims::Individuals<float> >() ) ;
  kmeansStrat.init("RandomIndividuals", rgp._nbClasses, pts ) ;
  
  IterativeClassification<float> iterClass(&pts, rgp._nbClasses, rgp._nbRuns, 0.0001, false, kmeansStrat ) ;
  iterClass.classification() ;
  
  AimsData<char> segmentation( locMap.dimX(), locMap.dimY(), locMap.dimZ() ) ;
  segmentation.setSizeXYZT( locMap.sizeX(), locMap.sizeY(), locMap.sizeZ() ) ;
  segmentation = -1 ;
  
  pts = iterClass.getClasses() ;
  
  cout << "nb of classes : " << pts.size() << endl ;
  for( unsigned char c = 0 ; c < pts.size() ; ++c ){
    cout << "class " << c <<" size : " << pts[c].size() << endl ;
    for( std::list< aims::Individuals<float> >::iterator it = pts[c].begin() ; it != pts[c].end() ; ++it )
      segmentation( int(it->position()[0]+0.01), int(it->position()[1]+0.01), int(it->position()[2]+0.01) ) = c ;
  }
  Writer< AimsData<char> > wri( rgp._fileout ) ;
  wri.write( segmentation )  ;
  
  return true ;
}

int main( int argc, const char **argv )
{
  string filein = "", fileout = "", filemask = "" ;
  int nbClasses = 12, nbRuns = 10 ;
  
  AimsApplication app( argc, argv, "K means based automatic segmentation") ;
  app.addOption( filein, "-i", "dynamic series file" ) ;
  app.addOption( fileout, "-o", "fileout segmentation image filename" ) ;
  app.addOption( nbClasses, "-c", "number of classes" ) ;
  app.addOption( nbRuns, "-r", "number of runs" ) ;
  app.addOption( filemask, "-m", "input mask of organism of interest (example: brain mask)", 1 ) ;

  try{
    app.initialize() ;
  }
  catch( user_interruption&)
  {
    return EXIT_FAILURE;
  }
  catch( exception& e)
  {
    cerr << e. what() << endl ;
    return EXIT_FAILURE;
  };
  
     
  KMeansSegmentation proc( fileout, filemask, nbClasses, nbRuns ) ;
  try
    {
      if( !proc.execute( filein ) )
	{
	  cerr << "Could not process\n";
	  return( 1 );
	}
    }
  catch (exception & e)
    {
      cout << "DBG lecture " << e.what() <<endl;
      return EXIT_FAILURE;
    }
  
  return( EXIT_SUCCESS );
}
