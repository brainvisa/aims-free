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

                                
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>    
#include <aims/io/io_g.h>	 
#include <aims/connectivity/component.h>
#include <aims/utility/utility_g.h>
#include <aims/pyramid/pyramid_g.h>
#include <aims/morphology/morphology_g.h>
#include <aims/math/math_g.h>
#include "individu.h"

using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  int result = EXIT_SUCCESS;
 
  try {


    // Ligne de commande
    Reader<AimsData<float> >  inReader;
    AimsData<float>           imageIn;
    bool                      verbose = false;
   
    AimsApplication application( argc, argv,     "LocalPCAnalysis " );   
				                		
    application.addOption( inReader, "-i",       "input files ");
    application.addOption( verbose,  "-verbose", "yields some more info", true );
   
    application.initialize(); 
   
    inReader >> imageIn;


    // Lecture & affichage des dimensions et taille de voxel de l'image
    // (lecture d'infos du header d'image)
    const PythonHeader *ph = 
     dynamic_cast<const PythonHeader *>( imageIn.header() );

    if( ph )
      {
	// la liste des attributs est donnée dans <aims/data/pheader.h>
	vector<int> dims = ph->getProperty( "volume_dimension" )
          ->value<vector<int> >();
	// variante équivalente:
	vector<float>	voxsize;
	ph->getProperty( "voxel_size", voxsize );
	cout << "volume dims: " ;
	cout << dims[0] << ", " << dims[1] << ", " << dims[2];
	if( dims.size() >= 4 )
	  cout << ", " << dims[3];
	cout << endl;
	cout << "voxel size : " ;
	cout << voxsize[0] << ", " << voxsize[1] << ", "  << voxsize[2];
	cout << endl;
      }; 


    // Ecriture du masque --> selection des individus
    ////////////// = 1       : individu pris en compte pour la PCA
    ////////////// = 0       : individu ignore = indiv du fond
    AimsData<short>  masque( imageIn.dimX(),imageIn.dimY(),imageIn.dimZ() );

    masque.setSizeXYZT( imageIn.sizeX(),imageIn.sizeY(),imageIn.sizeZ() );

    int              nbFrame= imageIn.dimT();
    int              nbVox= 0 ;  
    int              nn;
    int              x, y, z;

    // SEUILLAGE DECOURS JAMAIS NUL
    ForEach3d( imageIn, x, y, z )
      {
	if ( imageIn(x,y,z,nbFrame/2) > 0 ) 
	  // soit on regarde les decours des vox non nul a la 19 frame
	  //if ( imageIn(x,y,z,nbFrame/2) >= 0 )
	  // soit on regarde les decours pour tous les voxels  
	  {
	    nn=0; 
	    for (int t=0; t < imageIn.dimT() ;t++)	      
	      {
		if ( imageIn(x,y,z,t) !=0 )
		  nn+=1;  
	      }
	    if ( nn == imageIn.dimT() ) // décours non nul
	      { 	
		masque(x,y,z)= 1;
		nbVox += 1;        // nbre indiv ACP
	      }
	  }
      }

    // SEUILLAGE CLASSIQUE GREATER THAN
//     ForEach3d( imageIn, x, y, z )
//       {
// 	if ( imageIn(x,y,z,nbFrame/2) > 5000 ) 	 
// //	if ( imageIn(x,y,z,nbFrame/2) > 500 ) 	  
// 	  {	    
// 	    masque(x,y,z)= 1;
// 	    nbVox += 1;        
// 	  }
//       }

    Writer<AimsData<short> > masq("mask");
    masq <<  masque ;   

    cout << " masque ok " << endl;


   // Serie des individus sélectionnés
    AimsData<Individu> SERIE(nbVox);
    int                indic=0;

    ForEach3d( masque, x, y, z )
      {
	
	if ( masque(x,y,z) == 1 )
	  {
	    Individu individu( nbFrame ); 
	    for ( int t=0; t < nbFrame ;t++ )
	      {
		individu.myDecours[t] = imageIn( x,y,z,t );
		individu.myNorm += square(imageIn( x,y,z,t ) );
	      };
	    individu.myNorm = sqrt(  individu.myNorm );
	    individu.myLoc  = Point3d( x ,y, z );
	    SERIE(indic)= individu;
	    indic +=1;
	  }
	
      };
    
    cout << "serie global PCA  ok " << endl;
    
    
    // Analyse PCA au voisinage de chaque  voxel/'individu'  
    AimsData<double>   carteArite( imageIn.dimX(),imageIn.dimY(),imageIn.dimZ() );
    AimsData<double>   carteErsion( imageIn.dimX(),imageIn.dimY(),imageIn.dimZ() );
    AimsData<double>   carteErsion2( imageIn.dimX(),imageIn.dimY(),imageIn.dimZ() );
    AimsData<float>    imageNew( masque.dimX(),masque.dimY(),masque.dimZ(), imageIn.dimT() );

    imageNew.setSizeXYZT( imageIn.sizeX(),imageIn.sizeY(),imageIn.sizeZ() );
    carteArite.setSizeXYZT( imageIn.sizeX(),imageIn.sizeY(),imageIn.sizeZ() );
    carteErsion.setSizeXYZT( imageIn.sizeX(),imageIn.sizeY(),imageIn.sizeZ() );
    carteErsion2.setSizeXYZT( imageIn.sizeX(),imageIn.sizeY(),imageIn.sizeZ() );

    double             vmin, vmax; 
    int                c= 5 ;      // lg d'un cote  du masque   
    AimsData<Individu> serie(c*c*c); 
  
   
    // IL FAUDRAIT NE PRENDRE QUE LES VOXELS DONT LE MASQUE EST CONTENU DANS LE RAT///////////////////////////////////////////
    // LES ACP LOCALES SONT FAITES SUR LES DONNEES BRUTES .....

    for (int s=0; s < SERIE.dimX() ;s++)
      { //- - - - - - - - - - -  - - - - - - - - - - - - - -  - -

	// voisinage d'un voxel 
	int     x= SERIE(s).myLoc[0];
	int     y= SERIE(s).myLoc[1];
	int     z= SERIE(s).myLoc[2];	
	int     ind= 0;
	int     min_i= max( 0, x-c/2 );
	int     min_j= max( 0, y-c/2 );
	int     min_k= max( 0, z-c/2 );
	int     max_i= min( int(masque.dimX()), x+1+c/2);
	int     max_j= min( int(masque.dimY()), y+1+c/2);
	int     max_k= min( int(masque.dimZ()), z+1+c/2);
	
	for (int i= min_i;  i < max_i  ;i++)
	  for (int j= min_j;  j < max_j  ;j++)
	    for (int k= min_k;  k < max_k  ;k++) 
	      { 
		// nouvelle selection des individus
		if ( masque(i, j, k) == 1 )
		  { 
		    Individu individu( nbFrame );
		    for ( int t=0;  t < nbFrame  ;t++ )
		      {
			individu.myDecours[t] = imageIn(i,j, k, t);
		      };
		    individu.myLoc = Point3d( i, j, k);
		    serie(ind)= individu;
		    ind +=1;
		  } 
	      }; 

	if ( ind > 1)  // test le nbre d'indi ... 5? 10?????? 
	  {	    
	    
	    // Matrice des individus (donnees brutes )
	    AimsData<float> matriceIndiv( ind, nbFrame );
	   
	    for (int y0=0; y0 < matriceIndiv.dimY() ;y0++ )
	      for (int x0=0; x0 < matriceIndiv.dimX() ;x0++ )
		  matriceIndiv( x0, y0 )= serie(x0).myDecours[y0];
	
	  
	    // Matrice des correlations
	    AimsData< double >  matVarCov(nbFrame, nbFrame);
	    int                 x4, y4;

	    ForEach2d( matVarCov, x4, y4 )
	      {
		for(int k=0;  k < matriceIndiv.dimX()  ;++k)
		  matVarCov(x4, y4) += matriceIndiv(k, x4) * matriceIndiv(k, y4);	
		matVarCov(x4, y4) /= (ind-1); /// A FAIRE ??? meme si ca 
		                              // ne change pas les resultats
	      }
	   

	    // Decomposition SVD 
	    AimsSVD< double >  svd;
	    AimsData< double > eigenVect = matVarCov.clone();
	    
	    svd.setReturnType( AimsSVD< double >::VectorOfSingularValues );
	    
	    AimsData< double > eigenVal  = svd.doit( eigenVect );
	    
	    svd.sort(eigenVect, eigenVal);

	    // cout << "svd ok" << endl;


	    // Cartes de dispersion & disparite
	    vmax= eigenVal(0);
	    vmin= eigenVal( eigenVal.dimX()-1 );
	 
	    carteArite(x, y, z)= vmax - vmin;
	    carteErsion(x, y, z)= vmin / vmax;
	    carteErsion2(x, y, z)= eigenVal(1)/ vmax;

	    // cout << "cartes ok" << endl;


	    // Modification de la cinetique des 
	    // voxels de l'ACP globale
	    for ( int t=0; t < imageIn.dimT() ;t++ )
	        imageNew( x, y, z, t)= -eigenVect(t,0);
	    // justification du signe - ??? aucune !!!!! 

	    // cout << "modif decours ok" << endl;

	  }

       	
      };//- - - - - - - - - - -  - - - - - - - - - - - - - -  - -
    
    cout << " ANALYSE LOCALE PCA OK"  << endl;

    //ecriture de la nouvelle serie et des cartes
    Writer<AimsData<float> >   N("ratFiltre");
    N <<  imageNew ;

    // Unrecognized data type !!!!Unrecognized data type !!!!
    // Unrecognized data typ! 

    Writer<AimsData< double> > wA("disparite");
    wA  << carteArite;
    Writer<AimsData< double> > wE( "dispersion" );
    wE  << carteErsion;
    Writer<AimsData< double> > wE2( "dispersion2" );
    wE2  << carteErsion2;

    cout << " ecriture cartes et nouveau rat  OK"  << endl;


  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  
  return result;
}


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume< Individu >;

