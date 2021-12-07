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
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/connectivity/component.h>
#include <aims/utility/utility_g.h>
#include <aims/pyramid/pyramid_g.h>
#include <aims/signalfilter/medianfilter.h>
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

    AimsData<float>           imageIn;
    Reader<AimsData<float> >  inReader; 
    bool                      verbose = false;
      
    AimsApplication           application( argc, argv, "PCAnalysis " );   
				                		
    application.addOption( inReader, "-i",             "input files ");
    application.addOption( verbose,  "-verbose",       "yields some more info", true );
   
    application.initialize(); 
   
    inReader  >> imageIn;


    // Lecture & affichage des dimensions et taille de voxel de l'image
    // (lecture d'infos du header d'image)
    const PythonHeader *ph = 
     dynamic_cast<const PythonHeader *>( imageIn.header() );

    if( ph )
      {
	// la liste des attributs est donnée dans <aims/data/pheader.h>
	vector<int>   dims = ph->getProperty( "volume_dimension" )
          ->value<vector<int> >();
	// variante équivalente:
	vector<float> voxsize;
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


    // Pretraitement du rat AUCUN 
    // le rat en input est le résultat de l'ACP locale
    

    // Sélection des individus de l'analyse PCA --> masque du rat 
    AimsData<short>  masque(imageIn.dimX(),imageIn.dimY(),imageIn.dimZ());
 
    int              nbTot= imageIn.dimX()* imageIn.dimY()* imageIn.dimZ();
    int              nbFrame= imageIn.dimT();
    int              nbVox= 0 ;  // nb de voxels pour la PCA
    int              x, y, z;
   
    masque.setSizeXYZT(imageIn.sizeX(), imageIn.sizeY(),imageIn.sizeZ());

    ForEach3d(imageIn, x, y, z)
      {
	if ( imageIn(x,y,z,nbFrame/2) != 0 ) 
	  {	
	    masque(x,y,z)= 1;
	    nbVox += 1;	    
	  }
      }

    Writer<AimsData<short> > masq("mask");
    masq <<  masque ;
    

    // Mise en place des données 
    // 'tableau' des 'individus' PCA     
    AimsData<Individu> serie(nbVox);
    int                ind= 0;
                     
    ForEach3d( masque, x, y, z )
      {
	
	if ( masque(x,y,z) == 1 )
	  {
	    Individu individu( nbFrame );
	    
	    for ( int t=0; t<nbFrame ;t++ )
	      {
		individu.myDecours[t] = imageIn(x,y,z,t);
		individu.myNorm += square( imageIn(x,y,z,t) );
	      };
	    individu.myNorm = sqrt( individu.myNorm );
	    individu.myLoc = Point3d( x , y, z);
	    
	    serie(ind)= individu;
	    ind +=1;
	  }
	
      };
    
    cout << "Matrix of individuals (selection and building) performed\n";
   

    // Matrice des individus Centrée (et Réduite)
    AimsData<float> matriceIndiv( nbVox, nbFrame );
    vector<double>  indivMoy(nbFrame);
    vector<double>  indivVar(nbFrame);

    for (int i=0;  i < nbFrame  ;i++)
      {
	indivMoy[i]= 0.;
	indivVar[i]= 0.;
      }  
    ForEach2d( matriceIndiv, x, y )
      {
	matriceIndiv( x, y )= serie(x).myDecours[y];
	indivMoy[y] +=  matriceIndiv( x, y );
	indivVar[y] +=  square( matriceIndiv(x,y) );
      }
    for (int y=0; y< nbFrame  ;y++)
      {
	indivMoy[y] /= nbVox;
	indivVar[y] /= nbVox;
      }
    
    for ( uint i=0;  i < indivVar.size()  ;i++)
      indivVar[i] = sqrt( indivVar[i] - square(indivMoy[i]) ) ;
 
    for (int x=0;  x < matriceIndiv.dimX()  ;x++)
      for (int y=0;  y < matriceIndiv.dimY()  ;y++)
	matriceIndiv(x,y)= ( matriceIndiv(x,y) - indivMoy[y] ); // / indivVar[y];        
    cout << "Centering of matrix of individuals performed\n.";
    
    
    // Correction de la série  des 'individus' PCA 
    // données centrées écriture du rat centré   
    AimsData<float>  imCentre( imageIn.dimX(),imageIn.dimY(),imageIn.dimZ(), imageIn.dimT() ); 
    imCentre.setSizeXYZT( imageIn.sizeX(), imageIn.sizeY(),imageIn.sizeZ() );

    ForEach1d( serie, x )
      {
	for ( int t=0; t < nbFrame ;t++ )
	  serie(x).myDecours[t] = matriceIndiv(x,t);
	for (int yy=0;  yy < matriceIndiv.dimY()  ;yy++)
	  {
	    serie(x).myNorm = 0;
	    serie(x).myNorm += square( matriceIndiv(x,yy) );
	  }
	serie(x).myNorm = sqrt( serie(x).myNorm ) ;
      }

    //     ForEach1d( serie, x )
    //       {
    // 	for ( int t=0;  t < nbFrame  ;t++)
    // 	  imCentre(serie(x).myLoc[0],
    // 		   serie(x).myLoc[1],
    // 		   serie(x).myLoc[2],t)=  serie(x).myDecours[t];
    //       }
    
    //     Writer<AimsData<float> > I("imaCent");
    //     I << imCentre;
    
    
    // Matrice des correlations
    // matrice symétrique, définie positive
    AimsData< double >  matVarCov(nbFrame, nbFrame);
    
    ForEach2d( matVarCov, x, y )
      {
	for(int k=0;  k < matriceIndiv.dimX()  ;++k)
	  matVarCov(x, y) += matriceIndiv(k, x) * matriceIndiv(k, y);	
	// matVarCov(x, y) /= (nbVox-1); // VIENT DE LA REDUCTION
      }
    
    
    // Decomposition SVD 
    // renvoie les valeurs singulières de matVarCov
    // pour avoir les valeurs propres  ---> prendre la racine 
    // A VERIFIER
    AimsSVD< double >  svd;
    AimsData< double > eigenVect = matVarCov.clone();
    
    svd.setReturnType( AimsSVD< double >::VectorOfSingularValues );
    
    AimsData< double > eigenVal  = svd.doit( eigenVect );
    
    svd.sort(eigenVect, eigenVal);

    cout << endl;

    // VERITABLES VALEURS PROPRES 
    //     for (int i=0;  i < eigenVal.dimX()  ;i++)
    //       eigenVal(i)= sqrt( eigenVal(i)  );

    float         tr= 0;
    vector<float> somP( eigenVal.dimX());
    for (int i=0;  i < eigenVal.dimX()  ;i++)
      { 
	tr +=  eigenVal(i);
	somP[i] = tr;
	// cout << "val        :" << i << "->" << eigenVal(i) << endl;
      }
    
  
    // Creation d'un fichier d'info sur PCA
    ofstream fich;
    fich.open("infoPCA");
 
    fich << "PCA.....rat "                        << endl     << endl;
    fich << "nb total de voxels  :              " << nbTot    << endl ;
    fich << "nb de variables  :                 " << 38       << endl ;
    fich << "nb de voxels PCA, nb d'individus : " << nbVox    << endl ;
    fich << "pas de masquage de la vessie :        "          << endl ;
    fich << endl << "centre de gravité: "                     << endl;
    for (int y=0; y< nbFrame  ;y++)
      {
	fich << "g_"            << y+1 << " = " << indivMoy[y]<< endl ;
	//fich << "           v_" << y+1 << " : " << indivVar[y] << endl;
      }

    fich << endl    <<  "valeurs propres";
    fich << "     " <<  "pourc inertie expliquée" ;
    fich << "    "  <<  "diff "              << endl;  
    for (int n=0;  n < eigenVal.dimX() ;n++ )
      {
	fich << eigenVal(n) << "          "  <<( somP[n]/tr ) * 100 << "%"  ;
	fich << "                         " << ( eigenVal(n)/tr ) * 100 << "%" << endl ;
      }  
    
   
    fich.close();


    // Creation des fichiers pour visualiser les cinetiques 
    // avec gnuplot

    // cinetique moyenne
    ofstream M;
    M.open("Indmoyen");
    for (int n=0;  n < eigenVect.dimX() ;n++)
      M << indivMoy[n] << endl;
    M.close();
   
    // vecteurs unitaires des premiers axes factoriels
    ofstream Ccr, Cdecr, CCcr, CCdecr, C;
  
    int      vpMax=5;
    double   som1, som2;

    for (int k=0; k < vpMax ;k++)
      {
	
	for (int n=0;  n < 4  ;n++)
	  {
	    som1=0;
	    som2=0;
	    som1 += eigenVect(n,k);
	    som2 += eigenVect(n+4,k);
      }
	
	char filename[10];
	char filenameBis[10];
	char filenameMod[10];
	char filenameBisMod[12];


	sprintf(filename,    "vect%d"   ,    k+1);
	sprintf(filenameBis, "vectBis%d",    k+1);
	sprintf(filenameMod, "vectMod%d",    k+1);
	sprintf(filenameBisMod, "vectBisMod%d", k+1);


	Ccr.open(filename);
	CCcr.open(filenameMod);
	Cdecr.open(filenameBis);
	CCdecr.open(filenameBisMod);

	if ( som1 > som2  )//cinetique decroissante ... au debut...
	  { 
	    for (int n=0;  n < eigenVect.dimX() ;n++)
	      {
		Ccr    << -eigenVect(n,k)   << endl;
		// CCcr   << -eigenVect(n,k)*eigenVal(k)  << endl;
		CCcr   << -eigenVect(n,k)*sqrt(eigenVal(k))  << endl; // MODIF
		Cdecr  <<  eigenVect(n,k)              << endl;
		CCdecr <<  eigenVect(n,k)*eigenVal(k)  << endl;
	      }
	  }
	else               //cinetique croissante ... au debut...
	  { 
	    for (int n=0;  n < eigenVect.dimX() ;n++)
	      {
	        Ccr    <<  eigenVect(n,k) << endl;
		// CCcr   <<  eigenVect(n,k)*eigenVal(k)   << endl; 
		CCcr   <<  eigenVect(n,k)*sqrt(eigenVal(k))   << endl; // MODIF
		Cdecr  << -eigenVect(n,k)               << endl;
		CCdecr << -eigenVect(n,k)*eigenVal(k)   << endl;
	      }
 	  }

	Ccr.close();
	CCcr.close();
	Cdecr.close();
	CCdecr.close();

      }

    //  Writer<AimsData< double> > w("eigenVect");
    //     w << eigenVect;
    
    //     ForEach2d(eigenVect, x, y)
    //       {
    // 	if ( !x ) cout << endl;
    // 	cout << eigenVect(x, y) << " ";
    //       }
    

    // Calcul des images propres,
    for (int fac=0; fac < vpMax; ++fac)
      {
	AimsData< float > factor(masque.dimX(),masque.dimY(),masque.dimZ());
	
	factor.setSizeXYZT(imageIn.sizeX(), imageIn.sizeY(),imageIn.sizeZ());  

	ForEach1d( serie, x )
	  { 
	    for (int k=0;  k < nbFrame  ;++k )
		factor( serie(x).myLoc ) += serie(x).myDecours[k] * 
		  ( -eigenVect(k,fac) );  // * eigenVect(k,fac);	  
	  };
	
	char tmp_num1[32]; 
	sprintf(tmp_num1, "imagePropre_%d", fac+1);
	
	Writer<AimsData< float> > w( tmp_num1 );
 	w  << factor;		
      }
    

    // Calcul des  cartes de qualité de représentation des individus /axe factoriel
    // et des cartes de  contribution d'un individu à l'axe factoriel
//     for (int fac=0; fac < vpMax; ++fac)
//       {
// 	AimsData< float > repres(masque.dimX(),masque.dimY(),masque.dimZ()); 
// 	AimsData< float > contrib(masque.dimX(),masque.dimY(),masque.dimZ());
	
// 	repres.setSizeXYZT(imageIn.sizeX(), imageIn.sizeY(),imageIn.sizeZ());
//         contrib.setSizeXYZT(imageIn.sizeX(), imageIn.sizeY(),imageIn.sizeZ());
	
// 	ForEach1d( serie, x )
// 	  { 
// 	    //image contribution
// 	    for (int k= 0;  k < nbFrame;  ++k)
// 	      repres( serie(x).myLoc ) +=  serie(x).myDecours[k] 
// 		                                 * ( -eigenVect(k,fac) );  
// 	                                      // * eigenVect(k,fac); // au choix 
// 	    contrib( serie(x).myLoc ) = square( repres( serie(x).myLoc ) ) 
// 	                                    / sqrt( eigenVal(fac) ) ;  
// 	                                    // / eigenVal(fac) ;  ///// modif
// 	    //image qualité
// 	    //cos ()usually cos carré)
// 	    repres( serie(x).myLoc )= fabs( repres( serie(x).myLoc ) ) 
//                                             / ( serie(x).myNorm );	    
// 	  };
	
// 	char tmp_num2[32], tmp_num3[32]; 
// 	sprintf(tmp_num2, "qualite_%d", fac+1);
// 	sprintf(tmp_num3, "contrib_%d", fac+1);
	
// 	Writer<AimsData< float> > w2( tmp_num3 );
// 	w2 << contrib;
	
// 	Writer<AimsData< float> > w1( tmp_num2 );
// 	w1 << repres;	
//       }


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


