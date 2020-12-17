/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_REGISTRATION_BLOCK_MATCHING_D_H
#define AIMS_REGISTRATION_BLOCK_MATCHING_D_H

#include <aims/data/data_g.h>
#include <aims/resampling/resampling_g.h> 
#include <aims/math/math_g.h>
#include <aims/registration/block_matching.h>
#include <aims/registration/displacement_field.h>
#include <aims/registration/scale_control.h>
#include <aims/registration/transformation.h>
#include <aims/registration/minimisation.h>
#include <cartobase/type/limits.h>
#include <vector>


template<typename T>
BlockMatching<T>::BlockMatching()
{
  _done=false;
  _initialisation=Motion();
  _level_start=-1;
  _level_stop=-1;
  _transfo=1;
  _tailleBloc=Point3d(4,4,4);
  _cutVar=-1;
  _stopVar=-1;
  _Pkept=0.5;
  _seuilCorrel=-1;
  _seuils[0]=std::numeric_limits<T>::min();
  _seuils[1]=std::numeric_limits<T>::max();
  _seuils[2]=_seuils[0];
  _seuils[3]=_seuils[1];
  _itermax=3;
  _info=false;
}


template<typename T>
Motion BlockMatching<T>::doit( AimsData<T>& ref, const AimsData<T>& test_orig )
{


  AimsData<T> test=test_orig.clone();

  // Mise de l'image test a la resolution de l'image de reference si besoin
  if( (ref.sizeX()!=test.sizeX()) || (ref.sizeY()!=test.sizeY()) || (ref.sizeZ()!=test.sizeZ()) )
  {
    Motion identity;
    identity.setToIdentity();
      aims::LinearResampler<T> reech;
      reech.setRef( test.volume() );

      test = reech.doit( identity, 
                          unsigned (test.dimX()*test.sizeX()/(1.0*ref.sizeX()) + .5),
                          unsigned (test.dimY()*test.sizeY()/(1.0*ref.sizeY()) + .5),
                          unsigned (test.dimZ()*test.sizeZ()/(1.0*ref.sizeZ()) + .5),
                          Point3df(ref.sizeX(),ref.sizeY(),ref.sizeZ()));
  }

  
  // Instanciation des differentes classes externes
  ScaleControl      scaleControl;			// Gere la pyramide
  DisplacementField<T> displacementField;		// Calcule le champ d'appariemment
  Transformation transformation;				// Gere le critere de changement d'echelle delta
  Minimisation minimisation(_Pkept, _transfo);					// Calcule la transformation a partir du champ
    
  
  //declaration de l'image transformee a chaque etape
  AimsData<T> testtrans = test.clone();
  
  
  
  // Declaration des differents motions necessaires
  // p transfo totale precedente, q transfo ponctuelle calculee, r nouvelle transfo totale composee  
  Motion p, q, r ;
  p=_initialisation;
  q.setToIdentity();
  r.setToIdentity();

  {
  aims::LinearResampler<T> reech;
  reech.setRef( test.volume() );

  testtrans = reech.doit( p,
                          testtrans.dimX(),
                          testtrans.dimY(),
                          testtrans.dimZ(),
                          Point3df(testtrans.sizeX(),testtrans.sizeY(),testtrans.sizeZ()));
  }


  
  // Quelques initialisations en millimetres
  transformation.setX(int(test.dimX()*test.sizeX()));
  transformation.setY(int(test.dimY()*test.sizeY())); 
  transformation.setZ(int(test.dimZ()*test.sizeZ())); 
  transformation.setiterMax(_itermax); //on fixe le nombre maxi d'iter par niveau
  transformation.setcx(0);
  transformation.setcy(0);
  transformation.setcz(0);
  if(ref.dimZ()==1) _tailleBloc[2] = 1;
  scaleControl.init( ref, _level_start, _level_stop, _cutVar, _stopVar, _seuilCorrel, _tailleBloc);
  int count = 0 ;
  // Initialisation de delta au carre de 4 fois un majorant de 2 fois la diagonale en mm...
  float delta_init = 8*pow(test.dimX()*test.sizeX() + test.dimY()*test.sizeY() + test.dimZ()*test.sizeZ(), 2);


  // Affichage de tous les parametres avant debut :
  if(_info)
  {
    std::cout<<std::endl<<"PARAMETRES INITIAUX :"<<std::endl;
    std::cout<<"Transfo initiale : "<<std::flush;
    std::cout << p;

    std::cout<<"Transformation recherchee : ";
    switch(_transfo){
    case 1:	std::cout<<"RIGIDE"<<std::endl; break;
    case 2:	std::cout<<"SIMILITUDE"<<std::endl; break;
    case 3:	std::cout<<"AFFINE"<<std::endl; break;}

    std::cout<<"Pyramide debut : "<<scaleControl.getScale()<<"  fin : "<<scaleControl.getlevel_stop()<<std::endl;
    std::cout<<"Taille des blocs : x = "<<_tailleBloc[0]<<"  y = "<<_tailleBloc[1]<<"  z = "<<_tailleBloc[2]<<std::endl;
    std::cout<<"Pourcentage de blocs conserves par variance : %initial = "<<scaleControl.getcutVar()<<"  %final ="<<scaleControl.getstopVar()<<std::endl;
    std::cout<<"Seuil sur la mesure de similarite = "<<_seuilCorrel<<std::endl;
    std::cout<<"Seuils sur les niveaux de gris : "<<std::endl;
    std::cout<<"			seuilBasRef = "<<_seuils[0]<<"  seuilHautRef = "<<_seuils[1]<<std::endl;
    std::cout<<"			seuilBasTest = "<<_seuils[2]<<"  seuilHautTest = "<<_seuils[3]<<std::endl;
    std::cout<<"Nombre d'iterations par niveau de pyramide : "<<_itermax<<std::endl;
  }

  // MISE DE p EN VOXELS !
  p.matrix()(0, 3) /= test.sizeX();
  p.matrix()(1, 3) /= test.sizeY();
  p.matrix()(2, 3) /= test.sizeZ();

  
  do
  {    
    // boucle echelle
    std::cout << std::endl << std::endl << "PYRAMIDE NIVEAU : " << scaleControl.getScale() << std::endl;

    transformation.setdeltaprev(delta_init);
    displacementField.init( ref,  scaleControl, _seuils);

    count = 1; // count compte le nbre d'iterations sans changer d'echelle


    do 
    {  
      // boucle delta
      std::cout<<"		ITERATION No "<<count<<std::endl;
  
      // mise a jour transfo ancienne a ancienne nouvelle...	  
      p = q * p ;
  
      // Calcul effectif du champ
      AimsData< Point3d > df = displacementField.getField( testtrans);
  
    // Attention, le champ va de Ref a Test et non l'inverse !!!!!!!!!!!!!!!!!!!!!!!!
  
  
        // Calcul de la transformation d'etape a partir du champ estime, rappel : q est un motion
      q = minimisation.quaternion(displacementField);		//transfo de test vers ref !!!!
  
  
      // Rappel : r est la transformation totale actualisee, q la calculee courante et p l'ancienne totale
      // On actualise effectivement r en composant q avec r
      r = q * p ;
  
      // MISE DES TRANSLATIONS EN MILLIMETRES !
      r.matrix()(0, 3) *= test.sizeX();
      r.matrix()(1, 3) *= test.sizeY();
      r.matrix()(2, 3) *= test.sizeZ();

  
      // Resampling de test en testtrans par r la transfo totale la plus recente
      aims::LinearResampler<T> resampler;
      resampler.setRef( test.volume() );
      testtrans = resampler.doit( r, test.dimX(), test.dimY(), test.dimZ(),
                          Point3df(test.sizeX(),test.sizeY(),test.sizeZ()));
  
  
      // Compteur du nombre d'iterations a cette echelle	  
      ++count ;
  
    } while ( count < (_itermax + 1) );		// On fait itermax iterations exactement a chaque niveau de pyramide

      
    // On a fini d'iterer a cette echelle, on monte un niveau de pyramide
    scaleControl.nextScale();
     
      
  } while (scaleControl.goOn());		// test fin pyramide


  _result = test_orig.clone();
  aims::LinearResampler<T> linres;
  linres.setRef( test_orig.volume() );
  _result = linres.doit( r, 
                         test_orig.dimX(), 
                         test_orig.dimY(), 
                         test_orig.dimZ(),
                         Point3df(test_orig.sizeX(),test_orig.sizeY(),test_orig.sizeZ()));

  _done=true;

//  std::cout<<"C'est fini!"<<std::endl<<"Le motion trouve est : "<<std::endl;
//  std::cout << r << std::endl;
  return(r);
}

#endif

