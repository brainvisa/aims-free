/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_REGISTRATION_DISPLACEMENT_FIELD_D_H
#define AIMS_REGISTRATION_DISPLACEMENT_FIELD_D_H


/////////////////////////
#include <aims/data/data_g.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>



#include <cartobase/config/verbose.h>
#include <cartobase/type/limits.h>
#include <iostream>
#include <aims/registration/displacement_field.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <cartobase/type/string_conversion.h>

#ifdef AIMS_USE_MPI
#include <mpi.h>

using namespace MPI;
#endif


// fonction calculant la moyenne et variance de l'imagette(i,j) en parcourant l'offset
template<class T> inline
static void  meanVar(const AimsData<T>& im, const AimsData<int>& offset, int i, int j, int k, double &m, double &s, const T &lth, const T &hth)
{
  AimsData<int>::const_iterator oit;
  AimsData<int>::const_iterator eoit = offset.end();  //important optimisation not performed by g++ -O2
  // pointeur first sur le niveau im(i,j) du premier point de l'imagette de coin sup. gauche (i,j)
  T *first = (T*) (im.begin() + im.oFirstPoint() + j*im.oLine() + i + k*im.oSlice() );  
  T * pt ;		//pointeur courant parcourant l'imagette
  int N = offset.dimX() * offset.dimY() * offset.dimZ();		// nombre total de points dans l'imagette
  
  m = 0. ; s = 0. ;							//initialisation de moyenne et variance a 0
  
  for(oit =offset.begin(); oit != eoit; oit++ )
    {
      pt = first + *oit;						// absolu + relatif

      // Ca bogue ici parfois (sortie de l'image)
      if( (*pt<lth) || (*pt>hth) )      {s=0; m=0; break;}

      m += *pt;
      s += *pt * *pt;      
    }

  m /= N;
  
  s /= N; // E(X^2) ...
  s -= (m * m); // ... - (E(X))^2
  
  // On decide de renvoyer la variance qui est le carre de l'ecart type
  //  s = sqrt( s );	// passage de variance a ecart type en prenant la racine carree
  
  
}

template<class T>
void  DisplacementField<T>::selectBlock( const AimsData<T>& image, double cV)
{
  double m = 0.,s = 0.;
  
  int i, j, k;
     
  // Ecriture sur disque de pyrRef
  //aims::Writer< AimsData<T> > ecr_T("pyrRef" + carto::toString(_level) );
  //ecr_T.write(image);

  _sel.clear();
  _moy.clear();


  ForEach3d(image, i, j, k)
    {
      //std::cout << "selectBlock - [" << i << ", " << j << ", " << k << " ], [" << (image.dimX()- _nx) << ", " << (image.dimY() - _ny) << ", " << (image.dimZ() - _nz23d) << "]"<< std::endl << std::flush;
      if ( ( i < image.dimX() - _nx) && (j < image.dimY() - _ny) && (k < image.dimZ() - _nz23d) )
      {
        meanVar( image, _offset, i, j, k, m, s, _lthr, _hthr);
              //std::cout << "selectBlock - [" << i << ", " << j << ", " << k << " ], mean : " << m << ", var : " << s << std::endl << std::flush;
        if( s > 0. )
          {
            _sel.insert(std::pair<float, Point3d>(s, Point3d(i,j, k) ) ); // on remplit 2 map en classant par variance croissante
            _moy.insert(std::pair<float, double>(s, m ) ); // l'une avec les coord, l'autre avec la moyenne correspondante
          }
      }
    }
  
  _selThresh = _sel.size() * cV;

  
}


template<class T>
void DisplacementField<T>::init(AimsData<T>& ref, ScaleControl& scaleControl, T* seuils)
{
  // Initialisation des caracteristiques de ref
  _dimRef = Point3d(ref.dimX(), ref.dimY(), ref.dimZ());
  _sizeRef = Point3df(ref.sizeX(), ref.sizeY(), ref.sizeZ());

  // Initialisation de l'echelle
  _level = scaleControl.getScale();
    
  // taille des blocs
  _nx=(scaleControl.getTailleBloc()[0]);
  _ny=(scaleControl.getTailleBloc()[1]);
  _nz=(scaleControl.getTailleBloc()[2]);
  _nz23d=( _nz == 1 ? 0 : _nz );


  // On fixe le seuil sur le coeff de simi
  _seuilCorrel = scaleControl.getSeuilCorrel();

  // On rentre les seuils sur les niveaux de gris
  _lthr = seuils[0];
  _hthr = seuils[1];
  _ltht = seuils[2];
  _htht = seuils[3];

  
  // Construction de la pyramide pour l'image de reference
  Pyramid<T>      *Pref = NULL ;
  PyramidFunc<T> *pyramidfunc = NULL; 
  
  pyramidfunc = new MedianPyramidFunc<T>();
  Pref = new Pyramid<T>(*pyramidfunc);
  Pref->setRef( ref );
  Pref->setLevel( _level );
  
  _pyrRef = Pref->item( _level ).clone();		// _pyrRef est l'image au niveau _level
//   aims::Writer<AimsData<T> > w("pyref_debug.ima");
//   w.write(_pyrRef);
  
  // menage...
  delete Pref ;
  delete pyramidfunc ;
  
  
  // ATTENTION PROBLEME PYRAMIDE, SOLUTION TEMPORAIRE!!!!!!!!!!!!!!!!!!!
  //  _pyrRef = ref;
  
  
  // Creation des offsets comme pre-calcul des coordonnees
//  std::cerr<<"offsets"<<std::endl;
  AimsData<int> tmp(_nx,_ny,_nz);
  _offset = tmp;
  int i, j, k;
  ForEach3d(_offset, i, j, k)
    {
      _offset(i, j, k) = i + j * _pyrRef.oLine() + k * _pyrRef.oSlice() ;
    }
  
  // Selection des blocks
  //std::cout << "cur var : " << scaleControl.getcutVar() << std::endl << std::flush;
  selectBlock( _pyrRef, scaleControl.getcutVar());
}



template <class T>
AimsData< Point3d > DisplacementField<T>::getField(AimsData<T>& test)
{
  
  // Initialisation des barycentres
  _baryref[0] = _baryref[1] = _baryref[2] = 0.;
  _barytest[0] = _barytest[1] =_barytest[2] = 0.;
  
  // Initialisation de moyenne et variance courante pour calcul correlation
  double mLocalTest = 0, sLocalTest = 0;
  
  
  ////////////////////ATTENTION, PLACER LA PYRAMIDE SEULEMENT DANS LA GRANDE BOUCLE DU MAIN///////////////
  
  // Declaration pyramide pour l'image testtrans (transformee courante)
  Pyramid<T>      *Ptest = NULL ;
  PyramidFunc<T> *pyramidfunc = NULL; 
  
  pyramidfunc = new MedianPyramidFunc<T>();
  Ptest = new Pyramid<T>(*pyramidfunc);
  Ptest->setRef(test);
  Ptest->setLevel( _level );
  
  _pyrTest = Ptest->item( _level ).clone();		// _pyrTest est l'image testtrans degrade niveau _level
  
  // menage...
  delete Ptest ;
  delete pyramidfunc ;
  
 
  // Creation des offsets comme pre-calcul des coordonnees pour Test si dim differentes
  AimsData<int> tmp(_nx,_ny,_nz);
  _offset2 = tmp;
  int i, j, k;
  ForEach3d(_offset2, i, j, k)
  {
    _offset2(i, j, k) = i + j * _pyrTest.oLine() + k * _pyrTest.oSlice() ;
  }
 


 
  ////////////////////ATTENTION, PLACER LA PYRAMIDE SEULEMENT DANS LA GRANDE BOUCLE DU MAIN///////////////
  
  
  // Declaration de l'image du champ niveau _level, qui sera renvoyee en sortie
  AimsData<Point3d> field( _pyrRef.dimX(), _pyrRef.dimY(), _pyrRef.dimZ());
  field.setSizeXYZT( _pyrRef.sizeX(), _pyrRef.sizeY(), _pyrRef.sizeZ(), _pyrRef.sizeT() );
  field = Point3d(0, 0, 0) ;
  
  
  std::multimap<float, Point3d>::reverse_iterator it = _sel.rbegin();
  std::multimap<float, double>::reverse_iterator   itm = _moy.rbegin();
  AimsData<int>::iterator oit, oit2;
  std::vector<Point3df> pointstest, pointsref;    /* PAR: useless temporary vectors? */
  
  // Calcul de n puissance 4, pour prendre ro carre, pour positivite entre 0 et 1
  double norm_fctr = _offset.dimX() * _offset.dimY() * _offset.dimZ() * _offset.dimX() * _offset.dimY() * _offset.dimZ();
//  norm_fctr *= norm_fctr;	// mise au carre
  
  float puis_level = float(pow((double) 2, (double) _level));
//  std::cout << "_level = " << _level << "\tpuiss_level = " << puis_level 
//       << "\t _nx = " << _nx << "\t _ny = " << _ny <<std::endl ;
  int realPointNumber = 0 ;
  int entLimit = int(_selThresh + .5);  //after this number of blocks, all significant (in termes of enthropy) blocks have been dealt with
  // Declaration d'une image des blocs selectionnes par variance uniquement sur Ref

  /*AimsData<double> selected_blocks(_pyrRef.dimX(),					
											  _pyrRef.dimY(),
											  _pyrRef.dimZ() );
  selected_blocks.setSizeXYZT(_pyrRef.sizeX(),
										_pyrRef.sizeY(),
										_pyrRef.sizeZ(), 1.0);
  selected_blocks=0;


  AimsData<double> test_match(_pyrTest.dimX(),
										_pyrTest.dimY(),
										_pyrTest.dimZ() );
  test_match.setSizeXYZT(_pyrTest.sizeX(),
										_pyrTest.sizeY(),
										_pyrTest.sizeZ(), 1.0);
  test_match=0;*/

#ifdef AIMS_USE_MPI
  //Calculate index info. Homogenous mapping of blocks on procs, "first" procs recieve one more block than last.
  int rank = COMM_WORLD.Get_rank();  //we recalculate rank and size for each getField (~9-15 times), so as to not modify the prototype
  int size = COMM_WORLD.Get_size();
  int nbBlock = entLimit/size;       //entLimit may change so it's ok.
  int nbBlockLeft = entLimit%size;
  if(rank < nbBlockLeft) {
    for(j = 0;j < rank * (nbBlock + 1);j++) {   //can't iterate non-sequentially on reverse_iterators, so should change _sel to descending multimap and use a normal iterator..
      it++;
      itm++;
    }
    entLimit = nbBlock + 1;
  }
  else {
    for(j = 0;j < nbBlockLeft + rank * nbBlock;j++) {
      it++;
      itm++;
    }
    entLimit = nbBlock;
  }
  //Buffer declarations
  float* pointsRecv;          //for recieving pointrefs and pointtests
  int recvCounts[size];             //as in MPI
  int dspls[size];                  //as in MPI
#endif
  /* PAR: Important optimisation that g++ does not perform */
  AimsData<int>::const_iterator oitend = _offset.end();

  // boucle sur tous les blocs selectionnes dans pyrRef
  for(int n = 0; n < entLimit; ++n, ++it, ++itm)
  {
    Point3d p = it->second;			// p contient les coordonnees des points de ref selectionnes
    double mo = itm->second;			// moyenne du bloc de coin sup. gauche p
    double sd = itm->first;			// variance du meme bloc      
    int k, l, m;				// parcours voisinage bloc...
    double n4_by_sd = norm_fctr * sd;		// pre-calcul facteur normalisation
    bool badPoint = true;			// booleen pour savoir si le correl est correct
    bool one_max = true;			// booleen pour assurer unicite correl max 
    double correlMax = 0;			// on a normalement 0 <= correl_au_carre <= 1


    // boucle recherche bloc correspondant dans voisinage
    for(k=-3;k<4;++k)
    {
      for(l=-3;l<4;++l)
      {
        for(m=-3;m<4;++m)
        {
          if( (p[0]+k >= 0) && 
              (p[1]+l >= 0) && 
              (p[2]+m >= 0) && 
              (p[0]+k+_nx < _pyrTest.dimX() ) && 
              (p[1]+l+_ny < _pyrTest.dimY() ) &&
              (p[2]+m+ _nz23d < _pyrTest.dimZ() )  )
          {
            T *pRef       = _pyrRef.begin() + _pyrRef.oFirstPoint() + p[0] + p[1]*_pyrRef.oLine() + p[2]*_pyrRef.oSlice() ;
            T *pTest = _pyrTest.begin() + _pyrTest.oFirstPoint() + (p[0]+k) + (p[1]+l)*_pyrTest.oLine() + (p[2]+m)*_pyrTest.oSlice() ;

            meanVar( _pyrTest, _offset2, p[0]+k, p[1]+l, p[2]+m, mLocalTest, sLocalTest, _ltht, _htht);
            //std::cout << "mLocalTest : " << mLocalTest << ", sLocalTest : " << sLocalTest << std::endl << std::flush;

            //test_match(p[0]+k + _nx/2, p[1]+l + _ny/2, p[2]+m + _nz/2) = mLocalTest;	


            // calcul coefficient de correlation entre les deux blocs courants
            double correl = 0. ;		  
            for(oit = _offset.begin(), oit2 = _offset2.begin(); oit != oitend; oit++, oit2++ )
            {
              correl += ( *(pRef + *oit) - mo ) * ( *(pTest + *oit2 ) - mLocalTest);
            }
            correl *= correl; // mise au carre

            if(sLocalTest) 	
            {
              correl /= (n4_by_sd * sLocalTest); // normalisation
            }
            else correl = 0;

            if(correl) badPoint = false;

            // selection du correlMax
            if ( (correl == correlMax ) && (!badPoint) )
            {
              one_max = false;
              field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = Point3d(0,0,0);
            }
            else if ( (correl > correlMax ) && (!badPoint) )
            {
              one_max = true;
              correlMax = correl;
              field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = Point3d(k*(int16_t)puis_level, l*(int16_t)puis_level, m*(int16_t)puis_level);
            }
          }
        }
      }
    }

    // Calcul des coordonnes REELLES des CENTRES des blocs
    Point3df  curRef, curTest;

    curRef[0] = ( (float)p[0] + float(_nx)/2.0 ) * puis_level;
    curRef[1] = ( (float)p[1] + float(_ny)/2.0 ) * puis_level;
    curRef[2] = ( (float)p[2] + float(_nz23d)/2.0 ) * puis_level;

    curTest[0] = curRef[0] + float( field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2)[0] );
    curTest[1] = curRef[1] + float( field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2)[1] );
    curTest[2] = curRef[2] + float( field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2)[2] );

/*  curTest[0] = curRef[0] + float( (field(p))[0] ) * puis_level;
    curTest[1] = curRef[1] + float( (field(p))[1] ) * puis_level;
    curTest[2] = curRef[2] + float( (field(p))[2] ) * puis_level;*/


    // Ecriture de l'image des blocs selectionnes uniquement par variance sur Ref
    //selected_blocks( unsigned( (curRef[0])/10.0 + .5 ), unsigned( (curRef[1])/10.0 + .5) )= correlMax;

    //selected_blocks( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = correlMax;

    if (   (!badPoint)  && ( correlMax > _seuilCorrel ) && (one_max)	)
    {	  
      // On ajoute le centre REEL des 2 blocs apparies dans 2 listes
      pointsref.push_back(  curRef );
      pointstest.push_back( curTest );

      // Calcul du barycentre REEL des 2 listes de points
      _baryref[0] += curRef[0]; _baryref[1] += curRef[1]; _baryref[2] += curRef[2];
      _barytest[0] += curTest[0]; _barytest[1] += curTest[1]; _barytest[2] += curTest[2];
      ++realPointNumber;
    }
    else		
    {
      //selected_blocks( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2 ) = 255;
      field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = Point3d(0,0,0);
    }
   } //fin boucle sur chaque bloc

  _pointstest = pointstest;
  _pointsref  = pointsref;
#ifdef AIMS_USE_MPI
  /* Mutualize calculated fieldpoints.. (order of points is important, see yx in AffineLeastSquareEstimation::computeMotion()) */
    //Tech:Can't place directly in pointRecv instead of pushing back cause don't know where. So copy points into oversized buffer, than to pointRecv, or
    //get from _pointref, almost as fast..
  recvCounts[rank] = 6*realPointNumber;
  COMM_WORLD.Allgather(IN_PLACE, 0, INT, recvCounts, 1, INT);
  COMM_WORLD.Barrier();

  dspls[0] = 0;
  for(i = 1; i < size; i++) {                 //faster than MPI allgather
    dspls[i] = dspls[i-1] + recvCounts[i-1];
  }

  int totalNumFloats = dspls[size-1] + recvCounts[size-1];  //total number of points across all processes * 6
  pointsRecv = new float[totalNumFloats];
  for(i = 0; i < realPointNumber; i++) {
    for(int j = 0; j < 3; j++) {
      pointsRecv[dspls[rank] + i*6 + j] = (_pointsref[i])[j];
      pointsRecv[dspls[rank] + i*6 + 3 + j] = (_pointstest[i])[j];
    }
  }
  _pointsref.clear();_pointstest.clear();
  COMM_WORLD.Allgatherv(IN_PLACE, 0, INT, pointsRecv, recvCounts, dspls, FLOAT);
  COMM_WORLD.Barrier();
  for(int i = 0; i < totalNumFloats; i += 6) {
    _pointsref.push_back(Point3df(pointsRecv[i],pointsRecv[i+1],pointsRecv[i+2]));
    _pointstest.push_back(Point3df(pointsRecv[i+3],pointsRecv[i+4],pointsRecv[i+5]));
  }
  delete[] pointsRecv;

  /*..and centers of mass */
  realPointNumber = totalNumFloats/6;
  COMM_WORLD.Allreduce(IN_PLACE, _baryref, 3, DOUBLE, SUM);
  COMM_WORLD.Allreduce(IN_PLACE, _barytest, 3, DOUBLE, SUM);
#endif

//  Writer< AimsData<T> > testWriter("pyrTest_" + toString(_level) );  
//  testWriter.write(_pyrTest);


  // Ecriture sur disque de selected_bloks
//  aims::Writer< AimsData<double> > ecr_T("selected_blocks" + carto::toString(_level) );
//  ecr_T.write(selected_blocks);

//  aims::Writer< AimsData<double> > ecr_T2("test_match" + carto::toString(_level) );
//  ecr_T2.write(test_match);

  _baryref[0]  /= realPointNumber;
  _baryref[1]  /= realPointNumber;
  _baryref[2]  /= realPointNumber;
  _barytest[0] /= realPointNumber;
  _barytest[1] /= realPointNumber;
  _barytest[2] /= realPointNumber;

  return ( field );
}


template <class T>
AimsData<T> DisplacementField<T>::getQuality(AimsData<T>& testtrans, AimsData<T>& ref, int level, T thresh)
{
  // Declaration pyramide pour l'image testtrans (transformee finale au niveau _level)
  Pyramid<T>      *Ptesttrans = NULL ;
  PyramidFunc<T> *pyramidfunc = NULL; 
  
  pyramidfunc = new MedianPyramidFunc<T>();
  Ptesttrans = new Pyramid<T>(*pyramidfunc);
  Ptesttrans->setRef(testtrans);
  Ptesttrans->setLevel( level );
  
  AimsData<T> pyrTrans = Ptesttrans->item( level ).clone();		// pyrTrans est l'image testtrans degradee niveau level
  
  // menage...
  delete Ptesttrans ;
  delete pyramidfunc ;

  // Declaration pyramide pour l'image ref (transformee en niveau _level)
  Pyramid<T>      *Pref = NULL ;
  PyramidFunc<T> *pyramidfunc2 = NULL; 
  
  pyramidfunc2 = new MedianPyramidFunc<T>();
  Pref = new Pyramid<T>(*pyramidfunc2);
  Pref->setRef(ref);
  Pref->setLevel( level );
  
  AimsData<T> pyrRef = Pref->item( level ).clone();		// pyrRef est l'image ref degradee niveau level
  
  // menage...
  delete Pref ;
  delete pyramidfunc2 ;



  // Creation des offsets comme pre-calcul des coordonnees
  AimsData<int> tmp(_nx,_ny,_nz);
  _offset = tmp.clone();
  _offset2 = tmp.clone();
  int i, j, k;
  ForEach3d(_offset2, i, j, k)
    {
      _offset(i, j, k) = i + j * pyrRef.oLine() + k * pyrRef.oSlice() ;
      _offset2(i, j, k) = i + j * pyrTrans.oLine() + k * pyrTrans.oSlice() ;
    }





  // Initialisation de moyenne et variance courante pour calcul correlation
  double mRef = 0, sRef = 0, mTest = 0, sTest = 0;

  // Creation d'une image affichant la correlation en chaque point e un niveau de pyramide
  AimsData<T> pyrQuality = pyrTrans.clone();

  ////////////////////////
  pyrQuality = 0;
  
  AimsData<int>::iterator oit, oit2;
  AimsData<int>::const_iterator eoit = _offset.end();  //important optimisation that g++ does not perform
  
  // Calcul de n puissance 4, pour prendre ro carre, pour positivite entre 0 et 1
  double norm = _offset.dimX() * _offset.dimY() * _offset.dimZ() * _offset.dimX() * _offset.dimY() * _offset.dimZ();

  /////////////////////////////////////////
//  std::cout<<"thresh : "<<thresh<<std::endl;
//  std::cout<<"_nz23d : "<<_nz23d<<std::endl;

//  aims::Writer< AimsData<T> > pyrTransWriter("pyrTrans");
//  pyrTransWriter.write(pyrTrans);



  ForEach3d(pyrTrans, i, j, k)
  {
/*
		std::cout<<"i, j, k : "<<i<<", "<<j<<", "<<k<<std::endl;
		std::cout<<"i + _nx, j + _ny, k + _nz : "<<i + _nx<<", "<<j + _ny<<", "<<k + _nz23d*_nz<<std::endl;
		std::cout<<"pyrTrans.dimX(), pyrTrans.dimY(), pyrTrans.dimZ() : "<<pyrTrans.dimX()<<", "<<pyrTrans.dimY()<<", "<<pyrTrans.dimZ()<<std::endl;
*/


	   if(  (i + _nx < pyrTrans.dimX() ) && (j + _ny < pyrTrans.dimY() )  && (k + _nz23d*_nz < pyrTrans.dimZ() ) )
	   {
			/////////////////////////////////
		//	std::cout<<"boucle 1"<<std::endl;
			if( (pyrTrans(i + _nx/2,j + _ny/2, k + _nz23d*_nz/2) < thresh) /*&& (pyrRef(i,j) < thresh) */)
			{
		  		T *pRef    = pyrRef.begin()  + pyrRef.oFirstPoint()  + i + j*pyrRef.oLine() + k*pyrRef.oSlice();
		  		T *pTest 	= pyrTrans.begin() + pyrTrans.oFirstPoint() + i + j*pyrTrans.oLine() + k*pyrRef.oSlice();  


				meanVar( pyrTrans, _offset2, i, j , k, mTest, sTest, _ltht, _htht);
				meanVar( pyrRef, _offset, i, j , k, mRef,  sRef, _lthr, _hthr);
				double normS = norm * (sRef*sTest);
				/////////////////////////////////
//				std::cout<<"boucle 2"<<std::endl;


				if(normS!=0)
				{
						// calcul coefficient de correlation entre les deux blocs courants
		  				double correl = 0. ;		  
		  				for(oit = _offset.begin(), oit2 = _offset2.begin(); oit != eoit; oit++, oit2++ )
		    			{
		      			correl += ( *(pRef + *oit) - mRef ) * ( *(pTest + *oit2 ) - mTest);
		    			}
		  				correl *= correl;								// mise au carre
						correl /= normS;		// normalisation
						correl = sqrt(correl);
						
						//////////////////////////////////////////
//						std::cout<<"correl : "<<correl<<std::endl;
//						std::cout<<"correl rescale : "<<T((std::numeric_limits<T>::max() -1)*correl + 0.5)<<std::endl;

						pyrQuality(i + _nx/2,j + _ny/2, k + _nz23d*_nz/2) = T((std::numeric_limits<T>::max() -1)*correl + 0.5);
				}

				//////////////////////////////////////////////////////
//				else pyrQuality(i + _nx/2,j + _ny/2, k + _nz23d*_nz/2) = 1000;
			}
			else pyrQuality(i + _nx/2,j + _ny/2, k + _nz23d*_nz/2) = 0;
		}
	}


  return(pyrQuality);
}


#endif

