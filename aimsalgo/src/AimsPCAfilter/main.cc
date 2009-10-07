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
#include "individu.h"
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/connectivity/component.h>
#include <aims/utility/utility_g.h>
#include <aims/pyramid/pyramid_g.h>
#include <aims/signalfilter/medianfilter.h>
#include <aims/morphology/morphology_g.h>
#include <aims/math/math_g.h>

using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  int result = EXIT_SUCCESS;
 
  try {

    //______________________________________________________________________
    // Parametre de la ligne de commande
    // --------------------------------------------------------------------
    AimsData<float>           imageIn;
    Reader<AimsData<float> >  inReader; 
    AimsData<float>           imageInFilt;
    int                       clos= 1;
    int                       redu= 0;
    int                       am= 0, fm= 0; 
    int                       rx= 1, ry= 1, rz= 1;
    int                       norm_tps= 0;
    int                       vpMax=    6;
    bool                      verbose=  false;    
      
    AimsApplication           application( argc, argv, "PCAnalysis " );   
				                		
    application.addOption( inReader,    "-i"      , "input file"               );
    application.addOption( clos,        "-c"      , "for mask closing (default 1)"     ,true );
    application.addOption( redu,        "-r"      , "for reduction (default 0)"        ,true );
    application.addOption( norm_tps,    "-n"      , "for temporal normalisation (default 0, 1-->max, 2-->euclid)"    ,true );
    application.addOption( am,          "--am"    , "for  median aggreg (default 0)"   ,true );
    application.addOption( fm,          "--fm"    , "for median filt  (default 0)"     ,true );
    application.addOption( rx,          "--rx"    , "x resolution (default 1)"         ,true );
    application.addOption( ry,          "--ry"    , "y resolution (default 1)"         ,true );
    application.addOption( rz,          "--rz"    , "z resolution (default 1)"         ,true );
    application.addOption( vpMax,       "--vp"    , "eigenvalues kept (default 6)"     ,true );
    application.addOption( verbose,     "-verbose", "yields some more info"            ,true );
   
    application.initialize(); 
   
    //______________________________________________________________________
    // Lecture de l'image initiale imageIn
    // ------------------------------------------------------------------------
    inReader  >> imageIn;


    //______________________________________________________________________   
    // Lecture & affichage des dimensions et taille de voxel de l'image
    // (lecture d'infos du header d'image)
    // ------------------------------------------------------------------------
    const PythonHeader *ph= dynamic_cast<const PythonHeader *>( imageIn.header() );
    if( ph )
      {
	//la liste des attributs est donnée dans <aims/data/pheader.h>
	vector<int>    dims= ph->getProperty( "volume_dimension" ) 
          -> value<vector<int> >();
	//variante équivalente:
	vector<float>  voxsize;
	ph->getProperty( "voxel_size", voxsize );
	cout << "volume dims: " ;
	cout << dims[0] << ", " << dims[1] << ", " << dims[2];
	if ( dims.size() >= 4 )
	  cout << ", " << dims[3];
	cout << endl;
	cout << "voxel size : " ;
	cout << voxsize[0] << ", " << voxsize[1] << ", "  << voxsize[2];
	cout << endl;
      }; 


    //______________________________________________________________________
    // Pas de prétraitement
    // L'ACP se fait sur la série initiale imageIn
    // ------------------------------------------------------------------------
    imageInFilt= imageIn.clone();

    cout << "Cloning done\n";


    //____________________________________________________________________
    //  Pretraitement1 --> Aggregation mediane
    // Clustering de voxels libre, choix en x, en y et en z 
    // 3x3x1 -> permet de prendre en compte l'anisotropie des voxels de départ 
    // réduction de dimension 
    // substitution de l'aggregat par un voxel de valeur médiane
    // ------------------------------------------------------------------------
    if ( am == 1 )	
      {
	imageInFilt= 0.;
	AimsData<float> *PimageInFilt;
	Point3d         dim;
	Point3df        size;  
	
	dim[0]= imageIn.dimX() / rx;
	dim[1]= imageIn.dimY() / ry;
	dim[2]= imageIn.dimZ() / rz;
	size[0]= imageIn.sizeX() * rx;
	size[1]= imageIn.sizeY() * ry;
	size[2]= imageIn.sizeZ() * rz;
	
	PimageInFilt = new AimsData<float> ( dim[0],dim[1],dim[2],imageIn.dimT() );
	PimageInFilt->setSizeXYZT( size[0],size[1],size[2],imageIn.sizeT() );
	
	cout << endl << "Dim X, Dim Y, Dim Z = " << endl ;
	cout << "AVANT " << endl << imageIn.dimX() <<", "<< imageIn.dimY();
	cout << ", " << imageIn.dimZ() << endl << "APRES" << endl; 
	cout << PimageInFilt->dimX() << ", " << PimageInFilt->dimY() ;
	cout << ", " << PimageInFilt->dimZ() << endl; 
	cout << "Dim voxelX , Dim voxelY, Dim voxelZ = " << endl ;
	cout  << "AVANT " << endl << imageIn.sizeX() << ", "<< imageIn.sizeY();
	cout << ", " << imageIn.sizeZ() << endl << "APRES" << endl;
	cout << PimageInFilt->sizeX() << ", " << PimageInFilt->sizeY() ;
	cout << ", " << PimageInFilt->sizeZ(); 
	
	//création du sous volume/voxel
	AimsData<float>* dataRoi;
	dataRoi=  new AimsData<float> ( rx*ry*rz );
	dataRoi-> setSizeXYZT( size[0],size[1],size[2],PimageInFilt->sizeT() );
	
	int i, j, k, t, m, n, l;	    
	for (t= 0;  t < PimageInFilt->dimT()  ;++t)
	  for (k= 0;  k < dim[2]  ;++k)
	    for (j= 0;  j < dim[1]  ;++j)
	      for (i= 0;  i < dim[0]  ;++i)
		{		      
		  for (l= 0;  l < rz  ;++l)
		    for (n= 0;  n < ry  ;++n)
		      for (m= 0;  m < rx  ;++m)
			(*dataRoi)( l*rx*ry + n*rx + m )= imageIn(i*rx + m,    
								  j*ry + n,
								  k*rz + l,
								  t );    
		  //tri des valeurs
		  incSorting( *dataRoi );
		  (*PimageInFilt)( i,j,k,t )= (*dataRoi) ( rx*ry*rz / 2 );
		}
	
	//écriture du fichier
	int  ma= 0;  
	char aggreg[32];
	ma= rx*100 + ry*10 + rz;
	sprintf( aggreg, "ratACPaggreMedian_%d", ma );
	Writer<AimsData<float> > outWriter( aggreg ); 
 	outWriter << (*PimageInFilt);	    
	
	imageInFilt= *PimageInFilt;

	cout << endl << "Median aggregat done\n";
     
      }


    //____________________________________________________________________
    //  Pretraitement2 --> Application d'un filtre median
    // taille  du masque à determiner.
    // pas de réduction de  dimension -> affectation de la valeur médiane
    // ------------------------------------------------------------------------
    if ( fm == 1 )	
      {	   
	imageInFilt= 0.;
	MedianSmoothing< float > ms( rx, ry, rz );
	
	imageInFilt= ms.doit( imageIn );
	    
	//écriture du fichier
	int  ma= 0;
	char filtr[32];
	ma= rx*100 + ry*10 + rz;
	sprintf( filtr, "ratACPfiltreMedian_%d", ma );
	Writer<AimsData<float> > W( filtr );
	W <<  imageInFilt ;	  
  
	cout << "Median filtering done\n"; 

      }

      
    //______________________________________________________________________
    // Sélection des individus de l'Analyse en Composante Principale  
    // --> création d'un masque sur le volume initial : masqueInit 
    // =  1       : individus pris en compte pour la ACP
    // =  0       : indiv au decours = 0 au moins une fois 
    //              = voxels du fond + bizarreries dans le rat 
    //                + intérieur d'organe creux (estomac)
    // csqce ==> ++ suppression des voxels de bruit
    //           -- présence de 'trous' dans le rat
    // idée      : faire une fermeture ( par défaut, pas de modif des résult)
    // ------------------------------------------------------------------------
    AimsData<short> masque( imageInFilt.dimX(),imageInFilt.dimY(),imageInFilt.dimZ(),1,1 );    
         masque.setSizeXYZT( imageInFilt.sizeX(),imageInFilt.sizeY(),imageInFilt.sizeZ() );
    
    //détermination du masque 
    int                  nbFrame= imageInFilt.dimT();
    int                  nn= 0; 
    int                  x, y, z;
    ForEach3d(imageInFilt, x, y, z)
      {
	if ( imageInFilt(x,y,z,nbFrame/2) > 0 ) 
	  {	    
	    nn=0; 
	    //nb de valeurs != 0 pour chaque décours
	    for (int t=0; t < imageInFilt.dimT() ;t++ )  
	      if ( imageInFilt(x,y,z,t) !=0 )
		nn+=1;
	    if ( nn == imageInFilt.dimT() )
	      //décours non nul 
	      masque(x,y,z)= 1;
	  }
      }
    cout << "mask done"      << endl;

  
    ///// écriture du masque
    /////////////////////////////////////////////////////////////////////////
    Writer<AimsData<short> > mas("mask");
    mas <<  masque ;
    cout << "writing done \n";


   
    //'fermeture' du masque --> suppression des trous
    if ( clos == 1  )
      {
	AimsData<short> masqueCl(masque.dimX(),masque.dimY(),masque.dimZ(),1,1 );
	float           radius= 5;	
	masqueCl= AimsMorphoChamferClosing( masque, radius );	
     	
	ForEach3d( masqueCl,x,y,z )
	  masque(x,y,z)= masqueCl(x,y,z); 

	cout << "Closing done" << endl;

      }
  
    ///// écriture du masque
    /////////////////////////////////////////////////////////////////////////
    Writer<AimsData<short> > masAPRES("mask_clos");
    masAPRES <<  masque ;
    cout << "writing done \n";



    //application du masque sur imageInFilt --> imageInFiltM
    AimsData<float> imageInFiltM( imageInFilt.dimX(),imageInFilt.dimY(),imageInFilt.dimZ(),imageInFilt.dimT() );   
    imageInFiltM.setSizeXYZT( imageInFilt.sizeX(),imageInFilt.sizeY(),imageInFilt.sizeZ() );

    int nbVox= 0 ; 
    ForEach3d( masque, x, y, z )
	if ( masque(x,y,z) != 0 )
	  {
	    nbVox += 1;
	    for (int t= 0; t < imageInFilt.dimT() ;t++)
	      imageInFiltM(x,y,z,t)= imageInFilt(x,y,z,t);  
	  }
    cout << "mask application performed\n";	 
   
    //notations pour plus de clarte...
    int dX, dY, dZ;
    dX= imageInFiltM.dimX();
    dY= imageInFiltM.dimY();
    dZ= imageInFiltM.dimZ();
    float sX, sY, sZ;
    sX= imageInFiltM.sizeX() ;
    sY= imageInFiltM.sizeY() ;
    sZ= imageInFiltM.sizeZ() ;



/////////////// ATTENTION /////////////////////////////////////////////////////
//// imageIn      : série temporelle originale du rat
//// imageInFilt  : image résultant d'un filtrage  median (sans réduction 
////                de dim) 
////                OU d'une agrégation médiane (avec réduction de dim)
////                OU clone de imageIn si pas de pretraitement.
//// imageInFiltM : série temporelle du rat pour l'ACP obtenue après
////                application du masque; les décours dont l'analyse ne tient
////                pas compte sont les courbes qui s'annulent. 
////                pour un souci de presentation, on tient compte de certains
////                decours qui s'annulent (ceux correspondant a un voxel      
////                à l'intérieur du rat).
////
//// Par conséquent, le reste de l'analyse se fait avec imageInFiltM
/////////////////////////////////////////////////////////////////////////////

    
      //______________________________________________________________________
      // Représentation du tableau des données brutes 
      // --> 'série' d'individus pour ACP 
      // 1 individu = 1 localisation + 1 décours + 1 norme2 du decours
      // les décours peuvent etre normalisés avec norme2 ou normeMAX
      // ----------------------------------------------------------------------
      AimsData<Individu> serie(nbVox); 
      AimsData<float>    imageMax( dX, dY, dZ );
      imageMax.setSizeXYZT( sX, sY, sZ );
      float              maxD;
      int                ind= 0;
      
      ForEach3d( masque, x, y, z )
	{
	  
	  if ( masque(x,y,z) != 0 )
	    {
	      //déclaration d'un 'Individu'
	      Individu individu(nbFrame);
	     
	      //décours et normes 
	      maxD= 0;
	      for (int t= 0;  t < nbFrame  ;t++)
		{
		  individu.myDecours[t] = imageInFiltM(x,y,z,t);
		  if (  fabs( imageInFiltM(x,y,z,t) )  >  maxD   ) 
		    maxD= fabs(imageInFiltM(x,y,z,t));
		  individu.myNorm += square( imageInFiltM(x,y,z,t) );
		}
	      imageMax(x,y,z)= maxD;
	      individu.myNorm = sqrt( individu.myNorm );
	      
	      //normalisation temporelle du décours :norme euclidienne
	      if ( norm_tps == 2 ) 
		{
		  if ( individu.myNorm != 0 )
		    for (int t= 0; t < nbFrame  ;t++)	 
		      individu.myDecours[t] /= individu.myNorm;
		}
	      
	      //normalisation temporelle du décours : norme max
	      if ( norm_tps == 1 ) 
		{
		  if ( maxD != 0 )
		    for (int t= 0;  t < nbFrame  ;t++)         
		      individu.myDecours[t] /= maxD;
		} 
	      
	      //localisation de l'individu
	      individu.myLoc = Point3d(x, y, z);
	      
	      //insertion de l'individu dans 'serie'
	      serie(ind)= individu;
	      ind +=1;
	    };
	  
	};
      
      ////// écriture de l'image des max --> fin
      
      cout << "Serie of individuals (selection and building) performed\n";

      if ( norm_tps != 0 ) 
	cout << "with temporal normalisation " << endl;
      
      ///// écriture de la série 'normaliséé/temps' --> fin
	  
	  
	  //______________________________________________________________________
	  // Matrice des individus Centrée ( voire réduite )
	  //  --> calcul des moyennes de chacun des 38 volumes de la série dynamique
	  // (--> calcul des écart types de chacun ...)
	  // ----------------------------------------------------------------------
	  
	  AimsData<float> matriceIndiv( nbVox, nbFrame );
	  vector<double>  indivMoy(nbFrame);
	  vector<double>  indivVar(nbFrame);
	  
    for (int t= 0;  t < nbFrame  ;t++)
      {
	indivMoy[t]= 0.;
	indivVar[t]= 0.;
      }  

    ForEach2d( matriceIndiv, x, y )
      {
	matriceIndiv( x,y )= serie(x).myDecours[y];
	indivMoy[y] +=  matriceIndiv( x,y );
	indivVar[y] +=  square( matriceIndiv( x,y ) );
      }	
      
    for (int t= 0;  t < nbFrame  ;t++)
      {
	indivMoy[t] /= nbVox;
	indivVar[t] /= nbVox;
      }
        
    if ( redu == 1 )// si reduction
      { 
	for (int t= 0;  t < nbFrame  ;t++)
	  indivVar[t]= sqrt( indivVar[t] - square(indivMoy[t]) ) ;
      }

    for (int x= 0;  x < matriceIndiv.dimX()  ;x++)
      for (int y= 0;  y < matriceIndiv.dimY()  ;y++)
	{
	  matriceIndiv(x,y)= ( matriceIndiv(x,y) - indivMoy[y] ); 
	  // si reduction
	  if ( redu == 1 )
	    matriceIndiv(x,y) /= indivVar[y];
	}
    
    ///// écriture de la série 'centréé' --> fin
    
    cout << "Centering of matrix of individuals performed\n";
    

    //_____________________________________________________________________
    // Matrice des correlations
    // --> matrice symétrique, définie positive
    // --------------------------------------------------------------------
    
    AimsData< double >  matVarCov(nbFrame, nbFrame);
    
    ForEach2d( matVarCov, x, y )
      {
	for(int k= 0;  k < matriceIndiv.dimX()  ;++k)
	  matVarCov( x,y ) += matriceIndiv( k,x ) * matriceIndiv( k,y ); 
	if ( redu == 1 )// si réduction
	  matVarCov( x,y ) /= ( nbVox-1 );
      }
    
    cout << "Correlation  matrix of individuals performed\n";


    //______________________________________________________________________
    // Decomposition en Valeurs Singulières
    // --> svd 
    // renvoie les valeurs singulières de matVarCov
    // pour avoir les valeurs propres --> prendre la racine carre 
    // ------------------------------------------------------------------------
    
    AimsSVD< double >  svd;   
    AimsData< double > eigenVect = matVarCov.clone();
   
    //valeurs singulières dans un vecteur
    svd.setReturnType( AimsSVD< double >::VectorOfSingularValues );

    //extraction des valeurs singulières
    AimsData< double > eigenVal  = svd.doit( eigenVect );
   
    //tri decroissant des valeurs singulieres
    svd.sort(eigenVect, eigenVal);  

    //passage valeurs singulières --> valeurs propres 
    for (int i= 0;  i < eigenVal.dimX()  ;i++)
      eigenVal(i)= sqrt( eigenVal(i)  );
    
    ///// écriture et affichage des vecteurs propres --> fin
	
    cout << "svd performed\n" << endl;

      
    //_______________________________________________________________________
    // Correction de la série des 'individus' PCA  
    // --> centrage, reduction, normalisation temporelle...
    // ------------------------------------------------------------------------
    
    ForEach1d( serie, x )
      {
	//nouveau decours
	for (int t= 0;  t < nbFrame  ;t++)
	  serie(x).myDecours[t] = matriceIndiv(x,t);
	//nouvelle norme
	serie(x).myNorm = 0;
	for (int yy= 0;  yy < matriceIndiv.dimY()  ;yy++)
	  serie(x).myNorm += square( matriceIndiv(x,yy) );
	serie(x).myNorm = sqrt( serie(x).myNorm ) ;
      }
     
    cout << "correction  performed\n" << endl;


    //____________________________________________________________
    //creation d'un fichier contenant la norme de chaque individu 
    //--> pour visualiser avec Matlab la projection du nuage d'indiv 
    //_______________________________________________________________

    ofstream  N;
    int       comptN= 0;	
    char      fileNorm[10];
    sprintf(fileNorm, "norme" );
    N.open(fileNorm);
    ForEach1d( serie, x )
      { 	   	  		
	N  << serie(x).myNorm << endl;
	comptN ++;
      }	 
    N.close();
   

    //______________________________________________________________________
    // Calcul des images propres
    // ainsi que des images de qualité et de contribution
    // *les vecteurs propres sont définis au signe près 
    //  -->  l'orientation des axes est arbitraire
    // *il s'agit de la qualité et de la contribution des individus 
    // aux premiers axes factoriels 
    // ---------------------------------------------------------------------

   //declaration des objets pour les images qualite et distribution
    AimsData<float> repres(dX,dY,dZ);
    AimsData<float> contrib(dX,dY,dZ);       
    repres.setSizeXYZT(sX,sY,sZ);
    contrib.setSizeXYZT(sX,sY,sZ);
    
    //declaration d'un tableau de pointeurs d'objets AimsData
    vector< AimsData<float>* > totProj(nbFrame);

    //remplissage du tableau
    for (int fac=0;  fac < nbFrame  ;++fac)
      {

	// allocation du  tableau de pointeurs 
	// copie des l' 'images' propres factor dans le tableau
	totProj[fac]= new AimsData<float>(dX, dY, dZ);
	
 	(*(totProj[fac])).setSizeXYZT(sX,sY,sZ);
	
	*(totProj[fac])= 0.;

	ForEach1d( serie, x )
	  //valeurs des projections
	  for (int t= 0;  t < nbFrame  ;++t)
	   (*(totProj[fac]))( serie(x).myLoc ) += serie(x).myDecours[t] 
	      * (-eigenVect(t,fac) );    
	
	if ( fac < vpMax )
	  {
	    //initialisation
	    contrib= 0.;
	    repres=  0.;
	    
	    //ecriture des 'images' propres dans un fichier numerique
	    ofstream C ;
	    char     fileProj[10];
	    sprintf(fileProj, "proj_%d",fac+1 ); 
	    C.open(fileProj);
	    
	    ForEach1d( serie, x )
	      C  << (*(totProj[fac]))( serie(x).myLoc ) << endl;  
	    C.close(); 
	    
	    //ecriture de l''image propre'
	    char tmp_num1[32];
	    sprintf(tmp_num1, "imPropre_%d", fac+1);
	    Writer<AimsData< float> > w( tmp_num1 );
	    w  <<  *(totProj[fac]);
	  
	    //calul des 'images' des contributions et 
	    //des 'images' qualité représentation	       
	    ForEach1d( serie, x )
	      {
		//valeur absolue du cosinus (carré)
		repres( serie(x).myLoc )=  (  (*(totProj[fac]))(serie(x).myLoc) )  / serie(x).myNorm ;
	        // square()		
//contribution
		contrib( serie(x).myLoc ) = square(  (*(totProj[fac]))( serie(x).myLoc ) )  / eigenVal(fac);
	      }
	    
	    //ecriture de l''image qualite'
	    char tmp_num2[32];
	    sprintf(tmp_num2, "qualite_%d", fac+1);
	    Writer<AimsData< float> > w2( tmp_num2 );
	    w2 << repres;
	    
	    //ecriture de l''image contribution'
	    char tmp_num3[32];
	    sprintf(tmp_num3, "contrib_%d", fac+1);
	    Writer<AimsData< float> > w3( tmp_num3 );
	    w3 << contrib;	   
	  }
     	
      }

    cout << "images propres & contrib & repres  performed\n" << endl;
    cout << "totProj  performed\n" << endl;


   //______________________________________________________________________
   //Calcul de l'image Somme des projections
    //______________________________________________________________________
    AimsData<float> SoM(dX,dY,dZ);
    SoM.setSizeXYZT(sX,sY,sZ);
    AimsData<float> partSoM(dX,dY,dZ);
    partSoM.setSizeXYZT(sX,sY,sZ);

   ForEach3d( masque, x, y, z )	
     if ( masque(x,y,z) != 0  ){
       for (int t= 0;  t < nbFrame  ;t++){
	 if( t < vpMax )
	   partSoM(x,y,z) +=  fabs( ( *(totProj[t]) )(x,y,z)  );
	 SoM(x,y,z) +=  fabs( ( *(totProj[t]) )(x,y,z)  );
       }
       partSoM(x,y,z) /= SoM(x,y,z) ;
     }
   Writer<AimsData< float> > wpart("part" );
   wpart << partSoM ;       
   Writer<AimsData< float> > wsum("sum" );
   wsum << SoM ;       

   //______________________________________________________________________
   // Calcul voxel a voxel du rapport projection/somme
   //______________________________________________________________________
   AimsData<float> pourcProj(dX,dY,dZ);
   pourcProj.setSizeXYZT(sX,sY,sZ);

   for (int fac= 0;  fac < vpMax  ;++fac)
     {       
       ForEach3d(SoM, x, y, z )
	 if ( SoM(x,y,z) != 0  )	
	   pourcProj(x,y,z)= ( *(totProj[fac]) )(x,y,z) / SoM(x,y,z);
       
       //ecriture de l''image propre' pourcenT
       char nom[32];
       sprintf(nom, "projPourcent_%d",fac+1);
       Writer<AimsData< float> > wP(nom );
       wP  << pourcProj;       
     }

   cout << "writing projPourcent done " << endl;
    

    //__________________________________________________________________
    // Calcul du volume des normes de chaque decours en pourcentage
    //__________________________________________________________________
    // ATTENTION Check pas de div par zero
    AimsData<float> normProj(dX,dY,dZ);
    normProj.setSizeXYZT(sX,sY,sZ);

    //calcul de la norme max
    float maxNormes= 0;
    ForEach1d(serie, x)
      if ( serie(x).myNorm > maxNormes )
	maxNormes= serie(x).myNorm;

    //creation du volume des normes comprises entre 0 et 1
    ForEach1d (serie, x)
      normProj( serie(x).myLoc )= serie(x).myNorm / maxNormes;

    //ecriture de ce volume
    Writer<AimsData< float> > WsomNormes("normesP");
    WsomNormes << normProj;
    cout << "volume des normes ok" << endl;


    //___________________________________________________________________
    // Creation d'un fichier d'info sur les résultats de l'ACP
    // ------------------------------------------------------------------
    
    //calcul des sommes et sommes partielles des valeurs propres
    vector<float>  somP( eigenVal.dimX());
    float          tr= 0;
    for (int i=0;  i < eigenVal.dimX()  ;i++)
      { 
	tr +=  eigenVal(i);
	somP[i] = tr;
      }

    //informations generales
    int      nbTot=   imageIn.dimX()* imageIn.dimY()* imageIn.dimZ();
    ofstream fich;
    fich.open("infoPCA");
 
    fich << "PCA.....rat "                          << endl     << endl;
    fich << "nb total de voxels du volume du rat: " << nbTot    << endl ;
    fich << "nb de variables/frames de l'ACP :    " << nbFrame       << endl ;
    fich << "nb d'individus/décours de l'ACP  :   " << nbVox    << endl ;
    fich << endl << "centre de gravité: "                       << endl;
    for (int y=0; y< nbFrame  ;y++)
      {
	fich << "g_"            << y+1 << " = " << indivMoy[y] << "  ";
	fich << endl << "           v_" << y+1 << " : " << indivVar[y] << "  ";
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


    //___________________________________________________________
    // Creation des fichiers pour visualiser les cinetiques 
    // avec gnuplot
    // ---------------------------------------------------------------------

    ///// cinetique moyenne --> fin
  
    // vecteurs unitaires des premiers axes factoriels
    ofstream C, Cp;
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
	
	char filenameMod[10];
	sprintf(filenameMod, "vectMod%d",    k+1);
	C.open(filenameMod);

	char filename[10];
	sprintf(filename, "vect%d",    k+1);
	Cp.open(filename);
	
	if ( som1 > som2  )//cinetique decroissante ... au debut...
	  for (int n=0;  n < eigenVect.dimX() ;n++)
	    {
	      C  << -eigenVect(n,k)*eigenVal(k)  << endl;
	      Cp << -eigenVect(n,k)              << endl;
	    }
	
	else               //cinetique croissante ... au debut...
	  for (int n=0;  n < eigenVect.dimX() ;n++)
	    {
	      C   <<  eigenVect(n,k)*eigenVal(k)   << endl;
	      Cp  <<  eigenVect(n,k)               << endl;
	    } 
	
	C.close();
	Cp.close();
      }





    // PLUS /////////////////////////////////////////////////////////////////
    
    ///// écriture de l'image 'masquée'
    /////////////////////////////////////////////////////////////////////////
    // Writer<AimsData<float> > masqApp("maskApp");
    // masqApp <<  imageInFiltM ;
    // cout << "writing done \n";

    
    ///// écriture du masque
    /////////////////////////////////////////////////////////////////////////
    // Writer<AimsData<short> > mas("mask");
    //mas <<  masque ;
    //cout << "writing done \n";

    ///// écriture de l'image des max
    //Writer<AimsData< float> > wmax("TheMax" );
    //wmax << imageMax ;

    ///// écriture de la série 'normalisée/temps' ///////////////////////////
    /////////////////////////////////////////////////////////////////////////
    // Calcul de la série temporelle après normalisation  
    //     AimsData<float>  imNorme( (*imageInFiltM).dimX(),(*imageInFiltM).dimY(),(*imageInFiltM).dimZ(),(*imageInFiltM).dimT() );
    //     imNorme.setSizeXYZT( (*imageInFiltM).sizeX(), (*imageInFiltM).sizeY(),(*imageInFiltM).sizeZ() );
    //     ForEach1d( serie, x )
    //       {
    //      	for (int t=0;  t < nbFrame  ;t++)
    //     	  imNorme(serie(x).myLoc[0],
    // 		  serie(x).myLoc[1],
    // 		  serie(x).myLoc[2],t)=  serie(x).myDecours[t];
    //       } 
    //     Writer<AimsData<float> > I("imaNorm");
    //     I << imNorme;
    


    ///// écriture de la série 'centrée' //////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    // Calcul de la série temporelle après centrage   
    //     AimsData<float>  imCentre( imageIn.dimX(),imageIn.dimY(),imageIn.dimZ(), imageIn.dimT() );
    //     imCentre.setSizeXYZT( imageIn.sizeX(), imageIn.sizeY(),imageIn.sizeZ() );
    
    //     ForEach1d( serie, x )
    //       {
    // 	for (int t=0;  t < nbFrame  ;t++)
    // 	  imCentre(serie(x).myLoc[0],
    // 		   serie(x).myLoc[1],
    // 		   serie(x).myLoc[2],t)=  serie(x).myDecours[t];
    //       }
    
    //     Writer<AimsData<float> > I("imaCent");
    //     I << imCentre;
//  // visualisation d'un decours particulier
// 	ofstream Dec;
// 	Dec.open("decoursPart");
// 	for (int n=0;  n < nbFrame ;n++)
// 	  Dec << imCentre(61,67,74,n) << endl;
// 	Dec.close();
    

    ///// écriture et affichage des vecteurs propres //////////////////////////
    /////////////////////////////////////////////////////////////////////////
    //     Writer<AimsData< double> > w("eigenVect");
    //     w << eigenVect;
    
    //     ForEach2d(eigenVect, x, y)
    //       {
    //      	if ( !x ) cout << endl;
    //      	cout << eigenVect(x, y) << " ";
    //       }
    

    ///// cinetique moyenne ////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    //     ofstream M;
    //     M.open("Indmoyen");
    //     for (int n=0;  n < eigenVect.dimX() ;n++)
    //       M << indivMoy[n] << endl;
    //     M.close();
 

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


