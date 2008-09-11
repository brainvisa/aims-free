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


#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/random.h>
#include <aims/bucket/bucketMap.h>
#include <aims/classification/mixtureOfPpca_d.h>
#include <cartobase/type/string_conversion.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace aims ;
using namespace std ;
using namespace carto ;


template<class T>
static bool classif( Process & p, const string & filename, Finder & f ) ;


class IterativeClassif : public Process{
public:
  IterativeClassif( const string & labelImageIn, const string & fileOut, const string& filemask, int significantNbOfVp, int nbOfClasses, int maxNbOfIterations ) ;

  template<class T>
  bool classif_m( AimsData<T> * data, const string & filename, Finder & f) ;

private:
  string myLabelImageIn ;
  string myFileOut ;
  string myFileMask ;
  int myNbOfVp ;
  int myNbOfClasses ;
  int myMaxNbOfIterations ;
} ;


IterativeClassif::IterativeClassif( const string & labelImageIn, const string & fileOut, const string& filemask,
				    int significantNbOfVp, int nbOfClasses, 
				    int maxNbOfIterations ) :
  Process(), myLabelImageIn( labelImageIn ), myFileOut( fileOut ), myFileMask(filemask),
  myNbOfVp( significantNbOfVp ), myNbOfClasses( nbOfClasses ), 
  myMaxNbOfIterations( maxNbOfIterations ) 
{
  registerProcessType( "Volume", "U8", &classif< unsigned char > ) ;
  registerProcessType( "Volume", "S8", &classif< char > ) ;
  registerProcessType( "Volume", "S16", &classif< short > ) ;
  registerProcessType( "Volume", "U16", &classif< unsigned short > ) ;
  registerProcessType( "Volume", "FLOAT", &classif< float > ) ;
  registerProcessType( "Volume", "DOUBLE", &classif< double > ) ;
}


template<class T>
bool classif( Process & p, const string & filename, Finder & f )
{
  IterativeClassif & dp = ( IterativeClassif & ) p ;
  AimsData<T> * data = new AimsData<T> ;  
  return( dp.classif_m( data, filename, f ) ) ;
} 


template<class T>
bool IterativeClassif::classif_m( AimsData<T> * data, const string & filename, Finder & f)
{

// ENLEVER TOUS LES POINTS QUI ONT UNE VALEUR (t) <= 0
// -> parcourir l'image et mettre les valeurs des points "interessants" (qui n'ont pas de valeur <= 0) dans une matrice

  AimsData<short> mask ;
  Reader< AimsData< short > > mr( myFileMask ) ;
  if( !mr.read( mask ) )
    return( false ) ;

  Reader< AimsData< T > > vr( filename ) ;
  string format = f.format() ;
  if( !vr.read( *data, 0, &format ) )
    return( false ) ;

  cout << "dimensions de l'image en x, y, z, t = " << data->dimX() << " " << data->dimY() 
       << " " << data->dimZ() << " "<< data->dimT() << endl ;
  cout << "tailles de l'image en x, y, z, t = " << data->sizeX() << " " << data->sizeY() 
       << " " << data->sizeZ() << " "<< data->sizeT() << endl ;

  // image 3D de int = index dans la matrice des individus
//   AimsData< int > indexImage( data->dimX(), data->dimY(), data->dimZ() ) ;
//   indexImage.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ() ) ;
//   indexImage = 0 ;

  int x, y, z, t, cpt = 0 ;
  bool bruit ;

  // 1er passage pour connaitre le nb d'individus à prendre en compte
  for( x = 0 ; x < data->dimX() ; ++x )
    for( y = 0 ; y < data->dimY() ; ++y )
      for( z = 0 ; z < data->dimZ() ; ++z ){
	if( mask(x, y, z ) ){
	  bruit = false ;
	  for( t = 0 ; t < data->dimT() ; ++t )
	    if( (*data)(x, y, z, t) <= 0 ){
	      bruit = true ;
	      break ;
	    }
	  if( bruit == false )
	    ++cpt ;          // nb de voxels qui ne s'annulent jamais
	}
      }
  cout << "nb d'individus interessants = " << cpt << endl ;

  // matrice des individus dont aucune valeur ne s'annule
  AimsData<double> indivMatrix( cpt, data->dimT() ) ;
 // vecteur de correspondance cpt <-> x,y,z
  vector< Point3d > corresVector(cpt) ;
  Point3d voxelPos ;
  cpt = 0 ;

  for( x = 0 ; x < data->dimX() ; ++x )
    for( y = 0 ; y < data->dimY() ; ++y )
      for( z = 0 ; z < data->dimZ() ; ++z )
	if( mask(x, y, z) ){
	  bruit = false ;
	  for( t = 0 ; t < data->dimT() ; ++t )
	    if( (*data)(x, y, z, t) <= 0 ){
	      bruit = true ;
	      break ;
	    }
	  if( bruit == false ){
	    // creer en meme temps un vecteur contenant dans le meme ordre les (x,y,z)
	    voxelPos[0] = x ;
	    voxelPos[1] = y ;
	    voxelPos[2] = z ;
	    corresVector[cpt] =  voxelPos ;
	    for( t = 0 ; t < data->dimT() ; ++t )
	      indivMatrix( cpt, t ) = (*data)(x, y, z, t) ;
	    // indexImage( x, y, z ) = cpt ;
	    ++cpt ;
	  }
	}
  
  AimsData<short> segmentation( data->dimX(), data->dimY(), data->dimZ() ) ;
  segmentation.setSizeXYZT(data->sizeX(), data->sizeY(), data->sizeZ() ) ;
  
  
  delete data ;

  list<int> increasingKinetic ;
  list<int> decreasingKinetic ;
  
  AimsData<float> incKin( indivMatrix.dimY() ) ;
  for( int t = 0 ; t < indivMatrix.dimY() ; ++t ) 
    incKin(t) = float(t) / indivMatrix.dimY() - 0.5 ;
  
  AimsData<float> indiv( indivMatrix.dimY() ) ;
  AimsData<float> centeredIndiv( indivMatrix.dimY() ) ;
  float mean, corr ;
  for( int n = 0 ; n < indivMatrix.dimX() ; ++n ){
    mean = 0. ; corr = 0. ;
    for( int t = 0 ; t < indivMatrix.dimY() ; ++t ){
      indiv(t) = indivMatrix(n, t) ;
      mean += indiv(t) ;
    }
    mean /= float(indivMatrix.dimY()) ;
    
    for( int t = 0 ; t < indivMatrix.dimY() ; ++t ){
      corr += ( indiv(t) - mean ) * incKin(t) ;
    }
    if( corr >= 0. )
      increasingKinetic.push_back(n) ;
    else
      decreasingKinetic.push_back(n) ;
  }
  
  vector< list< int > > initialClasses( myNbOfClasses ) ;

//   if( myLabelImageIn == "" ){
//     int i = 0 ;
//     for( list<int>::iterator it = increasingKinetic.begin() ; it != increasingKinetic.end() ; ++it ){
//       i = (int)( (float)(myNbOfClasses/3) * rand() / (RAND_MAX+1.0) ) ; // tire au hasard un nb entre 0 et nbOfClasses-1
//       initialClasses[i].push_back( *it ) ;      
//     }
//     for( list<int>::iterator it = decreasingKinetic.begin() ; it != decreasingKinetic.end() ; ++it ){
//       i = (int)( (float)(myNbOfClasses - myNbOfClasses/3) * rand() / (RAND_MAX+1.0) + myNbOfClasses/3 ) ; // tire au hasard un nb entre 0 et nbOfClasses-1
//       initialClasses[i].push_back( *it ) ;
//     }
    
//     cout << "Initialisation aleatoire... " << endl ;
//   }
  if( myLabelImageIn == "" ){
    int i = 0 ;
    for( int ind = 0 ; ind < indivMatrix.dimX() ; ++ind ){
      i = (int)( (float)myNbOfClasses * rand() / (RAND_MAX+1.0) ) ; // tire au hasard un nb entre 0 et nbOfClasses-1
      initialClasses[i].push_back( ind ) ;
    }
    cout << "Initialisation aleatoire... " << endl ;
  }
  else{
    // ********************************************************
    
    cout << "Initialisation a partir d'une image de labels... " << endl ;
    AimsData< short > labelImage ;
    Reader< AimsData< short > > imreader( myLabelImageIn ) ;
    string format = f.format() ;
    if( !imreader.read( labelImage, 0, &format ) )
      return( false ) ;
    
    int ind = 0 ;
    int classNb = 0 ;
    for( x = 0 ; x < labelImage.dimX() ; ++x )
      for( y = 0 ; y < labelImage.dimY() ; ++y )
	for( z = 0 ; z < labelImage.dimZ() ; ++z )
	  if(mask(x, y, z) ){
	    if( labelImage( x, y, z ) != 0 ){
	      classNb = labelImage( x, y, z ) ;
	      initialClasses[classNb-1].push_back( ind ) ;  // classNb-1 : la classe 0 c'est le fond --> decalage...
	      ++ind ;
	    }
	  }  
    cout << "nb d'individus dans l'image de labels = " << ind << endl << endl ;
  }
  // ********************************************************
  

  for( int i = 0 ; i < myNbOfClasses ; ++i )
    cout << "Nb d'individus initialement dans la classe " << i+1 << " = " << initialClasses[i].size() << endl ;

	  

// LANCEMENT DE LA MOULINETTE MIXTURE OF PPCA
  int r = 1 ;
  MixtureOfPPCA<double> MPpca( myNbOfClasses, myNbOfVp, myMaxNbOfIterations, 
			       indivMatrix, corresVector, initialClasses, myFileOut, r, true ) ;
  
  initialClasses = vector< list<int> >() ;

  MPpca.doIt() ;
  
  segmentation = -1 ;
  AimsData<double> rni = MPpca.getRni() ;
  for( int ind = 0 ; ind < rni.dimX() ; ++ind ){
    short bestLabel = 0 ;
    for( int c = 1 ; c < rni.dimY() ; ++c )
      if( rni(ind, c) > rni(ind, bestLabel) )
	bestLabel = c ;
    segmentation( corresVector[ind] ) = bestLabel ;
  }

  Writer< AimsData<short> > wOut( myFileOut ) ;
  wOut.write( segmentation ) ;
  
//  const list< int > & nullPtnIndiv = MPpca.doIt() ;
//  const vector< vector< list< int > > >& finalClasses = MPpca.doIt() ;


  // image 3D de int, contenant 0 ou 1 
//   AimsData< short > problematicIndivImage( data->dimX(), data->dimY(), data->dimZ() ) ;
//   problematicIndivImage.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ() ) ;
//   problematicIndivImage = 0 ;

//   //recuperer le x,y,z du voxel dont la valeur est *iter
//   list<int>::const_iterator iter( nullPtnIndiv.begin() ), 
//     last( nullPtnIndiv.end() ) ;
//   while( iter != last ){
//     ForEach3d( indexImage, x, y, z )
//       {
// 	if( indexImage( x, y, z ) == *iter ) 
// 	  problematicIndivImage(x, y, z) = 1 ;
//       }
//     ++iter ;
//   }

//   Writer< AimsData< short > > writer( "problematic_indiv" + myLabelImageOut ) ;
//   string fformat = f.format() ;
//   if( !writer.write( problematicIndivImage, false, &fformat ) )
//     return( false ) ;

//  cout << "sauvegarde des classes finales... " ;
  // image 3D de int, contenant le numero de la classe 
//   AimsData< short > resultImage( data->dimX(), data->dimY(), data->dimZ() ) ;
//   resultImage.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ() ) ;
//   resultImage = 0 ;
//   Point3d theVoxel ;

//   // recuperer le x,y,z du voxel dont la valeur est *iter
//   for( short c = 0 ; c < myNbOfClasses ; ++c ){
//     list<int>::const_iterator iter( finalClasses[c].begin() ), 
//       last( finalClasses[c].end() ) ;
//     while( iter != last ){
//       theVoxel = corresVector[*iter] ;
//       resultImage( theVoxel ) = c + 1 ;
//       ++iter ;
//     }
//   }

//   Writer< AimsData< short > > writer( "result_" + myLabelImageOut ) ;
//   string fformat = f.format() ;
//   if( !writer.write( resultImage, false, &fformat ) )
//     return( false ) ;

  // *********************************************************
  // image des classes à chaque iteration
//  int nbOfFrames = finalClasses.size() ;
  // image 3D de int, contenant le numero de la classe 
//   AimsData< short > resultImage( data->dimX(), data->dimY(), data->dimZ(), nbOfFrames ) ;
//   resultImage.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ(), nbOfFrames ) ;
//   resultImage = 0 ;
//   Point3d theVoxel ;

//   for( int i = 0 ; i < nbOfFrames ; ++i ){ // boucle sur les iterations
//     // recuperer le x,y,z du voxel dont la valeur est *iter
//     for( short c = 0 ; c < myNbOfClasses ; ++c ){
//       list<int>::const_iterator iter( finalClasses[i][c].begin() ), 
// 	last( finalClasses[i][c].end() ) ;
//       while( iter != last ){
// 	theVoxel = corresVector[*iter] ;
// 	resultImage( theVoxel[0], theVoxel[1], theVoxel[2], i ) = c + 1 ;
// 	++iter ;
//       }
//     }
//   }

//   Writer< AimsData< short > > writer( "result_" + myLabelImageOut ) ;
//   string fformat = f.format() ;
//   if( !writer.write( resultImage, false, &fformat ) )
//     return( false ) ;


  cout << endl ;

  return true ;
}


int main( int argc, const char **argv )
{
  string fileIn, labelImageIn, fileOut, filemask ;
  int significantNbOfVp, nbOfClasses, maxNbOfIterations ;

  AimsApplication app( argc, argv, "Performs a Mixture of\nProbabilistic Principal Component Analysers" ) ;

  app.addOption( fileIn, "-i", "input image" ) ;
  app.alias( "-input", "-i" ) ;

//   app.addOption( labelImageOut, "-o", "output image of labels" ) ;
//   app.alias( "-outlabel", "-o" ) ;

  app.addOption( labelImageIn, "-in", "input image of labels", true ) ;
  app.alias( "-inlabel", "-in" ) ;

  app.addOption( filemask, "-mask", "input organism mask image" ) ;

  app.addOption( fileOut, "-out", "output 4D image" ) ;
  app.alias( "-output", "-out" ) ;

  app.addOption( significantNbOfVp, "-v", "significant number of eigen values" ) ;
  app.alias( "-significantNbOfVp", "-v" ) ;
  
  app.addOption( nbOfClasses, "-c", "number of classes" ) ;
  app.alias( "-nbOfClasses", "-c" ) ;
  
  app.addOption( maxNbOfIterations, "-n", "maximum number of iterations" ) ;
  app.alias( "-maxNbOfIter", "-n" ) ;
  
  try{
    app.initialize() ;
  } catch( exception& e){
    cerr << e. what() << endl ;
  };

  IterativeClassif proc( labelImageIn, fileOut, filemask, significantNbOfVp, 
			 nbOfClasses, maxNbOfIterations ) ;
  if( !proc.execute( fileIn ) )
    {
      cerr << "Couldn't process\n" ;
      return( 1 ) ;
    }

  return( EXIT_SUCCESS ) ;
}
