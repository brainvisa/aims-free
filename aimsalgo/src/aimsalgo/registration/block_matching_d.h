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

#include <aims/registration/block_matching.h>
#include <aims/resampling/resampling_g.h>
#include <aims/math/math_g.h>
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
Motion BlockMatching<T>::doit(
  carto::rc_ptr<carto::Volume<T> >& ref,
  const carto::rc_ptr<carto::Volume<T> >& test_orig )
{


  carto::VolumeRef<T> test = carto::VolumeRef<T>( test_orig ).copy();

  // Mise de l'image test a la resolution de l'image de reference si besoin
  if( ref->getVoxelSize() != test->getVoxelSize() )
  {
    Motion identity;
    identity.setToIdentity();
      aims::LinearResampler<T> reech;
      reech.setRef( test );

      test = reech.doit( identity, 
                          unsigned (test->getSizeX()*test->getVoxelSize()[0]/(1.0*ref->getVoxelSize()[0]) + .5),
                          unsigned (test->getSizeY()*test->getVoxelSize()[1]/(1.0*ref->getVoxelSize()[1]) + .5),
                          unsigned (test->getSizeZ()*test->getVoxelSize()[2]/(1.0*ref->getVoxelSize()[2]) + .5),
                          Point3df(ref->getVoxelSize()[0],ref->getVoxelSize()[1],ref->getVoxelSize()[2]));
  }

  
  // Instanciation des differentes classes externes
  ScaleControl      scaleControl;			// Gere la pyramide
  DisplacementField<T> displacementField;		// Calcule le champ d'appariemment
  Transformation transformation;				// Gere le critere de changement d'echelle delta
  Minimisation minimisation(_Pkept, _transfo);					// Calcule la transformation a partir du champ
    
  
  //declaration de l'image transformee a chaque etape
  carto::VolumeRef<T> testtrans = carto::VolumeRef<T>( test ).copy();
  
  
  
  // Declaration des differents motions necessaires
  // p transfo totale precedente, q transfo ponctuelle calculee, r nouvelle transfo totale composee  
  Motion p, q, r ;
  p=_initialisation;
  q.setToIdentity();
  r.setToIdentity();

  {
  aims::LinearResampler<T> reech;
  reech.setRef( test );

  testtrans = reech.doit( p,
                          testtrans->getSizeX(),
                          testtrans->getSizeY(),
                          testtrans->getSizeZ(),
                          Point3df( testtrans->getVoxelSize() ) );
  }


  // Quelques initialisations en millimetres
  transformation.setX(int(test->getSizeX()*test->getVoxelSize()[0]));
  transformation.setY(int(test->getSizeY()*test->getVoxelSize()[1]));
  transformation.setZ(int(test->getSizeZ()*test->getVoxelSize()[2]));
  transformation.setiterMax(_itermax); //on fixe le nombre maxi d'iter par niveau
  transformation.setcx(0);
  transformation.setcy(0);
  transformation.setcz(0);
  if(ref->getSizeZ()==1) _tailleBloc[2] = 1;
  scaleControl.init<T>( ref, _level_start, _level_stop, _cutVar, _stopVar, _seuilCorrel, _tailleBloc);
  int count = 0 ;
  // Initialisation de delta au carre de 4 fois un majorant de 2 fois la diagonale en mm...
  float delta_init = 8*pow(test->getSizeX()*test->getVoxelSize()[0] + test->getSizeY()*test->getVoxelSize()[1] + test->getSizeZ()*test->getVoxelSize()[2], 2);


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
  p.matrix()(0, 3) /= test->getVoxelSize()[0];
  p.matrix()(1, 3) /= test->getVoxelSize()[1];
  p.matrix()(2, 3) /= test->getVoxelSize()[2];

  
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
      carto::VolumeRef< Point3d > df = displacementField.getField( testtrans);
  
    // Attention, le champ va de Ref a Test et non l'inverse !!!!!!!!!!!!!!!!!!!!!!!!
  
  
        // Calcul de la transformation d'etape a partir du champ estime, rappel : q est un motion
      q = minimisation.quaternion(displacementField);		//transfo de test vers ref !!!!
  
  
      // Rappel : r est la transformation totale actualisee, q la calculee courante et p l'ancienne totale
      // On actualise effectivement r en composant q avec r
      r = q * p ;
  
      // MISE DES TRANSLATIONS EN MILLIMETRES !
      r.matrix()(0, 3) *= test->getVoxelSize()[0];
      r.matrix()(1, 3) *= test->getVoxelSize()[1];
      r.matrix()(2, 3) *= test->getVoxelSize()[2];

  
      // Resampling de test en testtrans par r la transfo totale la plus recente
      aims::LinearResampler<T> resampler;
      resampler.setRef( test );
      testtrans = resampler.doit( r, test->getSizeX(), test->getSizeY(), test->getSizeZ(),
                                  Point3df(test->getVoxelSize()) );
  
  
      // Compteur du nombre d'iterations a cette echelle	  
      ++count ;
  
    } while ( count < (_itermax + 1) );		// On fait itermax iterations exactement a chaque niveau de pyramide

      
    // On a fini d'iterer a cette echelle, on monte un niveau de pyramide
    scaleControl.nextScale();
     
      
  } while (scaleControl.goOn());		// test fin pyramide


  _result = carto::VolumeRef<T>( test_orig ).copy();
  aims::LinearResampler<T> linres;
  linres.setRef( test_orig );
  _result = linres.doit( r, 
                         test_orig->getSizeX(),
                         test_orig->getSizeY(),
                         test_orig->getSizeZ(),
                         Point3df( test_orig->getVoxelSize() ) );

  _done=true;

//  std::cout<<"C'est fini!"<<std::endl<<"Le motion trouve est : "<<std::endl;
//  std::cout << r << std::endl;
  return(r);
}

#endif

