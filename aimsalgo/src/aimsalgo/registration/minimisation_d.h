/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_REGISTRATION_MINIMISATION_D_H
#define AIMS_REGISTRATION_MINIMISATION_D_H


#include <cartobase/type/string_conversion.h>
#include <cartobase/config/verbose.h>
#include <iostream>

#include <aims/io/motionW.h>
#include <aims/transform/least_square_estimation.h>
#include <aims/registration/minimisation.h>


//methode principale:

template <class T>
Motion Minimisation::quaternion(DisplacementField<T>& displacementField)
{
      
  //on charge l'ensemble des points a apparier
  std::vector<Point3df> ptstest = displacementField.getpointstest();
  std::vector<Point3df> ptsref = displacementField.getpointsref(); 
  int size = ptstest.size();
  unsigned sizekept = unsigned ( double(ptstest.size())*_Pkept );
  
//   Point3dd sumtest(0,0,0);
//   std::vector<Point3df>::iterator it, ite = ptstest.end();
//   for(it = ptstest.begin(); it!=ite; ++it) {
//       sumtest[0] += (*it)[0];
//       sumtest[1] += (*it)[1];
//       sumtest[2] += (*it)[2];
//   }
//   std::cout << "!! Test points sum: " << carto::toString(sumtest) << std::endl;
//   
//   Point3dd sumref(0,0,0);
//   ite = ptsref.end();
//   for(it = ptsref.begin(); it!=ite; ++it) {
//       sumref[0] += (*it)[0];
//       sumref[1] += (*it)[1];
//       sumref[2] += (*it)[2];
//   }
//   std::cout << "!! Ref points sum: " << carto::toString(sumref) << std::endl;
  
  Motion m;

  if ((sizekept > 0) && (ptsref.size() > 0))
  {
    // Calcul de la transformation LS choisie
    switch(_transfo)
    {
      case 1:
      {
        // std::cout<<"Calcul transfo rigide..."<<std::endl;
        aims::RigidLeastSquareEstimation leastSquareEstimation(ptstest, ptsref);
        m = *(leastSquareEstimation.motion());
        break;
      }

      case 2:
      {
        // std::cout<<"Calcul transfo similitude..."<<std::endl;
        aims::SimiLeastSquareEstimation leastSquareEstimation(ptstest, ptsref);
        m = *(leastSquareEstimation.motion());
        break;
      }

      case 3:
      {
        // std::cout<<"Calcul transfo affine..."<<std::endl;
        aims::AffineLeastSquareEstimation leastSquareEstimation(ptstest, ptsref);
        m = *(leastSquareEstimation.motion());
        break;
      }

      default:
      {
        m.setToIdentity();
        throw std::runtime_error("The transformation chosen is not valid !") ;
        break;
      }
    }

    //ROBUSTESSE DEBUT
    
    // Le tri des residus pour ref et test  
    std::multimap<float, Point3df> mt;
    std::multimap<float, Point3df> mr;
    
    std::multimap<float, Point3df>::iterator itt;
    std::multimap<float, Point3df>::iterator itr;
    
    // Diverses variables utiles
    Point3df r; // point residu courant
    float r2; // norme carree de r
    unsigned indice; // indice de boucle multiple (variable locale boucle impossible)
    Motion rigide_kept;
    Motion similitude_kept;

    // La liste des points ref et test ayant les plus petits residus
    std::vector<Point3df> ptstestkept;
    std::vector<Point3df> ptsrefkept;
    
    
    for(int boucle=0; boucle < 3; boucle++)
    {

//       std::cout << "Minimisation - s : " << size << ", m : " << m << std::endl << std::flush;

      mt.clear(); mr.clear();
      ptstestkept.clear(); ptsrefkept.clear();

      for(int l = 0; l < size; l++)  
      {
        Point3df x = ptstest[l];
        Point3df y = ptsref[l];

        r = y - m.transform(x); //calcul du residu pour chaque point
        r2 = r.norm(); // sa norme carree
        mt.insert(std::pair<float, Point3df>(r2, x) ); //insertion du residu et du point test dans map	
        mr.insert(std::pair<float, Point3df>(r2, y) ); //insertion du residu et du point test dans map
      }

      // On remplit les listes avec une partie seulement des meilleurs residus      
      for (itt = mt.begin(),itr = mr.begin(), indice = 0 ;indice<sizekept;++itt,++itr,++indice)
      {
        ptstestkept.push_back( (*itt).second );
        ptsrefkept.push_back( (*itr).second );
      }

      // Calcul de la transformation LS choisie
      switch(_transfo)
      {
        case 1:
        {
          // cout<<"Calcul transfo rigide..."<<cout;
          aims::RigidLeastSquareEstimation leastSquareEstimation_kept(ptstestkept, ptsrefkept);
          m = *(leastSquareEstimation_kept.motion());
          break;
        }

        case 2:
        {
          // cout<<"Calcul transfo similitude..."<<cout;
          aims::SimiLeastSquareEstimation leastSquareEstimation_kept(ptstestkept, ptsrefkept);
          m = *(leastSquareEstimation_kept.motion());
          break;
        }

        case 3:
        {
          // cout<<"Calcul transfo affine..."<<cout;
          aims::AffineLeastSquareEstimation leastSquareEstimation_kept(ptstestkept, ptsrefkept);
          m = *(leastSquareEstimation_kept.motion());
          break;
        }

        default:
        {
          m.setToIdentity();
          throw std::runtime_error("The transformation chosen is not valid !") ;
          break;
        }
      }
    } // fin boucle robustesse		
  }
  else
  {
    cartoDbgMsg( 1, "Not enough information in reference or test image to estimate transformation" );
  }
  
  return (m);
}


#endif

