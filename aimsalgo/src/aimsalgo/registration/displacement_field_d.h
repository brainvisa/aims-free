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
#include <aims/registration/displacement_field.h>
#include <aims/pyramid/pyramid.h>
#include <aims/pyramid/medfunc.h>
#include <cartobase/config/verbose.h>
#include <cartobase/type/limits.h>
#include <iostream>
#include <cartobase/type/string_conversion.h>

#ifdef AIMS_USE_MPI
#include <mpi.h>

using namespace MPI;
#endif


// fonction calculant la moyenne et variance de l'imagette(i,j) en parcourant l'offset
template<class T> inline
static void  meanVar(const carto::rc_ptr<carto::Volume<T> > im,
                     const carto::rc_ptr<carto::Volume<int> > offset, int i, int j, int k,
                     double &m, double &s, const T &lth, const T &hth)
{
    carto::Volume<int>::const_iterator oit;
    carto::Volume<int>::const_iterator eoit = offset->end();  //important optimisation not performed by g++ -O2
    // pointeur first sur le niveau im(i,j) du premier point de l'imagette de coin sup. gauche (i,j)
    T *first = (T*)(&im->at( i, j, k ));
    T *pt;        //pointeur courant parcourant l'imagette
    // nombre total de points dans l'imagette
    int N = offset->getSizeX() * offset->getSizeY() * offset->getSizeZ();

    m = 0. ; s = 0. ;                            //initialisation de moyenne et variance a 0

    for(oit =offset->begin(); oit != eoit; oit++)
    {
        pt = first + *oit;                        // absolu + relatif

        // Ca bogue ici parfois (sortie de l'image)
        if( (*pt<lth) || (*pt>hth) )
        {
            s=0; m=0; break;
        }

        m += *pt;
        s += *pt * *pt;      
    }

    m /= N;

    s /= N; // E(X^2) ...
    s -= (m * m); // ... - (E(X))^2

    // On decide de renvoyer la variance qui est le carre de l'ecart type
    //  s = sqrt( s );    // passage de variance a ecart type en prenant la racine carree
}

template<class T>
void DisplacementField<T>::selectBlock(const carto::rc_ptr<carto::Volume<T> > image,
                                    double cV)
{
    double m = 0.,s = 0.;

    int i, j, k;
        
    _sel.clear();
    _moy.clear();
    std::vector<int> dim = image->getSize();


    for(k = 0; k < dim[2]; ++k)
        for(j = 0; j < dim[1]; ++j)
            for(i = 0; i < dim[0]; ++i)
            {
                if ((i < dim[0] - _nx) && (j < dim[1] - _ny)
                    && (k < dim[2] - _nz23d))
                {
                    meanVar(image, _offset, i, j, k, m, s, _lthr, _hthr);
                    //std::cout << "selectBlock - [" << i << ", " << j << ", " << k << " ], mean : " << m << ", var : " << s << std::endl << std::flush;
                    if(s > 0.)
                    {
                        _sel.insert(std::pair<float, Point3d>(s, Point3d(i,j, k))); // on remplit 2 map en classant par variance croissante
                        _moy.insert(std::pair<float, double>(s, m)); // l'une avec les coord, l'autre avec la moyenne correspondante
                    }
                }
            }

    _selThresh = _sel.size() * cV;
}


template<class T>
void DisplacementField<T>::init(carto::rc_ptr<carto::Volume<T> > ref,
                                ScaleControl& scaleControl, T* seuils)
{
    // Initialisation des caracteristiques de ref
    _dimRef = Point3d(ref->getSizeX(), ref->getSizeY(), ref->getSizeZ());
    _sizeRef = Point3df(ref->getVoxelSize());

    // Initialisation de l'echelle
    _level = scaleControl.getScale();

    // taille des blocs
    _nx = (scaleControl.getTailleBloc()[0]);
    _ny = (scaleControl.getTailleBloc()[1]);
    _nz = (scaleControl.getTailleBloc()[2]);
    _nz23d = (_nz == 1 ? 0 : _nz);


    // On fixe le seuil sur le coeff de simi
    _seuilCorrel = scaleControl.getSeuilCorrel();

    // On rentre les seuils sur les niveaux de gris
    _lthr = seuils[0];
    _hthr = seuils[1];
    _ltht = seuils[2];
    _htht = seuils[3];


    // Construction de la pyramide pour l'image de reference
    Pyramid<T>     *Pref = NULL ;
    PyramidFunc<T> *pyramidfunc = NULL; 

    pyramidfunc = new MedianPyramidFunc<T>();
    Pref = new Pyramid<T>(*pyramidfunc);
    Pref->setRef(ref);
    Pref->setLevel(_level);

    _pyrRef = carto::VolumeRef<T>(Pref->item(_level)).deepcopy();        // _pyrRef est l'image au niveau _level

    // menage...
    delete Pref ;
    delete pyramidfunc ;


    // ATTENTION PROBLEME PYRAMIDE, SOLUTION TEMPORAIRE!!!!!!!!!!!!!!!!!!!
    //  _pyrRef = ref;


    // Creation des offsets comme pre-calcul des coordonnees
    //  std::cerr<<"offsets"<<std::endl;
    carto::VolumeRef<int> tmp(_nx,_ny,_nz);
    _offset = tmp;
    int i, j, k;
    std::vector<int> dim = _offset->getSize();

    for(k=0; k < dim[2]; ++k)
        for(j=0; j < dim[1]; ++j)
            for(i=0; i < dim[0]; ++i)
            {
                _offset(i, j, k) = &_pyrRef->at(i, j, k) - &_pyrRef->at(0);
            }

    // Selection des blocks
    //std::cout << "cur var : " << scaleControl.getcutVar() << std::endl << std::flush;
    selectBlock(_pyrRef, scaleControl.getcutVar());
}



template <class T>
carto::VolumeRef< Point3d > DisplacementField<T>::getField(
  carto::rc_ptr<carto::Volume<T> > test)
{
    // Initialisation des barycentres
    _baryref[0] = _baryref[1] = _baryref[2] = 0.;
    _barytest[0] = _barytest[1] =_barytest[2] = 0.;

    // Initialisation de moyenne et variance courante pour calcul correlation
    double mLocalTest = 0, sLocalTest = 0;


    ////////////////////ATTENTION, PLACER LA PYRAMIDE SEULEMENT DANS LA GRANDE BOUCLE DU MAIN///////////////

    // Declaration pyramide pour l'image testtrans (transformee courante)
    Pyramid<T>     *Ptest = NULL ;
    PyramidFunc<T> *pyramidfunc = NULL; 

    pyramidfunc = new MedianPyramidFunc<T>();
    Ptest = new Pyramid<T>(*pyramidfunc);
    Ptest->setRef(test);
    Ptest->setLevel( _level );

    _pyrTest = carto::VolumeRef<T>(Ptest->item(_level)).deepcopy();        // _pyrTest est l'image testtrans degrade niveau _level

    // menage...
    delete Ptest ;
    delete pyramidfunc ;


    // Creation des offsets comme pre-calcul des coordonnees pour Test si dim differentes
    carto::VolumeRef<int> tmp(_nx,_ny,_nz);
    _offset2 = tmp;
    int i, j, k;
    std::vector<int> dim = _offset2->getSize();

    for(k=0; k < dim[2]; ++k)
        for(j=0; j < dim[1]; ++j)
            for(i=0; i < dim[0]; ++i)
            {
                _offset2(i, j, k) = &_pyrTest->at(i, j, k) - &_pyrTest->at(0);
            }

    ////////////////////ATTENTION, PLACER LA PYRAMIDE SEULEMENT DANS LA GRANDE BOUCLE DU MAIN///////////////


    // Declaration de l'image du champ niveau _level, qui sera renvoyee en sortie
    carto::VolumeRef<Point3d> field(_pyrRef->getSize());
    field.setVoxelSize(_pyrRef->getVoxelSize());
    field = Point3d(0, 0, 0) ;


    std::multimap<float, Point3d>::reverse_iterator it = _sel.rbegin();
    std::multimap<float, double>::reverse_iterator   itm = _moy.rbegin();
    carto::Volume<int>::iterator oit, oit2;
    std::vector<Point3df> pointstest, pointsref;    /* PAR: useless temporary vectors? */

    // Calcul de n puissance 4, pour prendre ro carre, pour positivite entre 0 et 1
    double norm_fctr = _offset->getSizeX() * _offset->getSizeY() 
                     * _offset->getSizeZ() * _offset->getSizeX() 
                     * _offset->getSizeY() * _offset->getSizeZ();
    //  norm_fctr *= norm_fctr;    // mise au carre

    float puis_level = float(pow((double) 2, (double) _level));
    //  std::cout << "_level = " << _level << "\tpuiss_level = " << puis_level 
    //       << "\t _nx = " << _nx << "\t _ny = " << _ny <<std::endl ;
    int realPointNumber = 0 ;
    int entLimit = int(_selThresh + .5);  //after this number of blocks, all significant (in termes of enthropy) blocks have been dealt with
    // Declaration d'une image des blocs selectionnes par variance uniquement sur Ref

    /*carto::VolumeRef<double> selected_blocks(_pyrRef->getSizeX(),
                                                _pyrRef->getSizeY(),
                                                _pyrRef->getSizeZ() );
    selected_blocks.setSizeXYZT(_pyrRef.sizeX(),
                                            _pyrRef.sizeY(),
                                            _pyrRef.sizeZ(), 1.0);
    selected_blocks=0;


    carto::VolumeRef<double> test_match(_pyrTest->getSizeX(),
                                            _pyrTest->getSizeY(),
                                            _pyrTest->getSizeZ() );
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
        for(j = 0; j < rank * (nbBlock + 1); j++) 
        {
            //can't iterate non-sequentially on reverse_iterators, so should change _sel to descending multimap and use a normal iterator..
            it++;
            itm++;
        }
        entLimit = nbBlock + 1;
    }
    else {
        for(j = 0;j < nbBlockLeft + rank * nbBlock;j++) 
        {
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
    carto::Volume<int>::const_iterator oitend = _offset.end();

    // boucle sur tous les blocs selectionnes dans pyrRef
    for(int n = 0; n < entLimit; ++n, ++it, ++itm)
    {
        Point3d p = it->second;            // p contient les coordonnees des points de ref selectionnes
        double mo = itm->second;            // moyenne du bloc de coin sup. gauche p
        double sd = itm->first;            // variance du meme bloc      
        int k, l, m;                // parcours voisinage bloc...
        double n4_by_sd = norm_fctr * sd;        // pre-calcul facteur normalisation
        bool badPoint = true;            // booleen pour savoir si le correl est correct
        bool one_max = true;            // booleen pour assurer unicite correl max 
        double correlMax = 0;            // on a normalement 0 <= correl_au_carre <= 1


        // boucle recherche bloc correspondant dans voisinage
        for(k = -3; k < 4; ++k)
        {
            for(l = -3; l < 4;++l)
            {
                for(m = -3; m < 4; ++m)
                {
                    if((p[0]+k >= 0) && 
                       (p[1]+l >= 0) && 
                       (p[2]+m >= 0) && 
                       (p[0]+k+_nx < _pyrTest->getSizeX() ) &&
                       (p[1]+l+_ny < _pyrTest->getSizeY() ) &&
                       (p[2]+m+ _nz23d < _pyrTest->getSizeZ()))
                    {
                        T *pRef  = &_pyrRef->at(p);
                        T *pTest = &_pyrTest->at(p[0]+k, p[1]+l, p[2]+m);

                        meanVar(_pyrTest, _offset2, p[0]+k, p[1]+l, p[2]+m, mLocalTest, sLocalTest, _ltht, _htht);
                        //std::cout << "mLocalTest : " << mLocalTest << ", sLocalTest : " << sLocalTest << std::endl << std::flush;

                        //test_match(p[0]+k + _nx/2, p[1]+l + _ny/2, p[2]+m + _nz/2) = mLocalTest;    


                        // calcul coefficient de correlation entre les deux blocs courants
                        double correl = 0. ;          
                        for(oit = _offset.begin(), oit2 = _offset2.begin(); oit != oitend; oit++, oit2++)
                            correl += (*(pRef + *oit) - mo ) * ( *(pTest + *oit2 ) - mLocalTest);

                        correl *= correl; // mise au carre

                        if(sLocalTest)     
                            correl /= (n4_by_sd * sLocalTest); // normalisation
                        else 
                            correl = 0;

                        if(correl)
                            badPoint = false;

                        // selection du correlMax
                        if ((correl == correlMax ) && (!badPoint))
                        {
                            one_max = false;
                            field(p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = Point3d(0,0,0);
                        }
                        else if ((correl > correlMax ) && (!badPoint))
                        {
                            one_max = true;
                            correlMax = correl;
                            field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = Point3d(k*(int16_t)puis_level, 
                                                                                          l*(int16_t)puis_level, 
                                                                                          m*(int16_t)puis_level);
                        }
                    }
                }
            }
        }

        // Calcul des coordonnes REELLES des CENTRES des blocs
        Point3df  curRef, curTest;

        curRef[0] = ((float)p[0] + float(_nx)/2.0) * puis_level;
        curRef[1] = ((float)p[1] + float(_ny)/2.0) * puis_level;
        curRef[2] = ((float)p[2] + float(_nz23d)/2.0) * puis_level;

        curTest[0] = curRef[0] + float(field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2)[0]);
        curTest[1] = curRef[1] + float(field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2)[1]);
        curTest[2] = curRef[2] + float(field( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2)[2]);

    /*  curTest[0] = curRef[0] + float( (field(p))[0] ) * puis_level;
        curTest[1] = curRef[1] + float( (field(p))[1] ) * puis_level;
        curTest[2] = curRef[2] + float( (field(p))[2] ) * puis_level;*/


        // Ecriture de l'image des blocs selectionnes uniquement par variance sur Ref
        //selected_blocks( unsigned( (curRef[0])/10.0 + .5 ), unsigned( (curRef[1])/10.0 + .5) )= correlMax;

        //selected_blocks( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = correlMax;

        if ((!badPoint)  && (correlMax > _seuilCorrel) && (one_max))
        {      
            // On ajoute le centre REEL des 2 blocs apparies dans 2 listes
            pointsref.push_back(curRef);
            pointstest.push_back(curTest);

            // Calcul du barycentre REEL des 2 listes de points
            _baryref[0] += curRef[0]; _baryref[1] += curRef[1]; _baryref[2] += curRef[2];
            _barytest[0] += curTest[0]; _barytest[1] += curTest[1]; _barytest[2] += curTest[2];
            ++realPointNumber;
        }
        else        
        {
            //selected_blocks( p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2 ) = 255;
            field(p[0] + _nx/2, p[1] + _ny/2, p[2] + _nz23d/2) = Point3d(0,0,0);
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
    for(i = 0; i < realPointNumber; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            pointsRecv[dspls[rank] + i*6 + j] = (_pointsref[i])[j];
            pointsRecv[dspls[rank] + i*6 + 3 + j] = (_pointstest[i])[j];
        }
    }
    _pointsref.clear();_pointstest.clear();
    COMM_WORLD.Allgatherv(IN_PLACE, 0, INT, pointsRecv, recvCounts, dspls, FLOAT);
    COMM_WORLD.Barrier();
    for(int i = 0; i < totalNumFloats; i += 6)
    {
        _pointsref.push_back(Point3df(pointsRecv[i], pointsRecv[i+1], pointsRecv[i+2]));
        _pointstest.push_back(Point3df(pointsRecv[i+3], pointsRecv[i+4], pointsRecv[i+5]));
    }
    delete[] pointsRecv;

    /*..and centers of mass */
    realPointNumber = totalNumFloats/6;
    COMM_WORLD.Allreduce(IN_PLACE, _baryref, 3, DOUBLE, SUM);
    COMM_WORLD.Allreduce(IN_PLACE, _barytest, 3, DOUBLE, SUM);
#endif

    _baryref[0]  /= realPointNumber;
    _baryref[1]  /= realPointNumber;
    _baryref[2]  /= realPointNumber;
    _barytest[0] /= realPointNumber;
    _barytest[1] /= realPointNumber;
    _barytest[2] /= realPointNumber;

    return field;
}


template <class T>
carto::VolumeRef<T> DisplacementField<T>::getQuality(carto::rc_ptr<carto::Volume<T> > testtrans,
                                                     carto::rc_ptr<carto::Volume<T> > ref,
                                                     int level, T thresh)
{
    // Declaration pyramide pour l'image testtrans (transformee finale au niveau _level)
    Pyramid<T>      *Ptesttrans = NULL ;
    PyramidFunc<T> *pyramidfunc = NULL; 
    
    pyramidfunc = new MedianPyramidFunc<T>();
    Ptesttrans = new Pyramid<T>(*pyramidfunc);
    Ptesttrans->setRef(testtrans);
    Ptesttrans->setLevel( level );
    
    carto::VolumeRef<T> pyrTrans
        = carto::VolumeRef<T>(Ptesttrans->item(level)).deepcopy();    // pyrTrans est l'image testtrans degradee niveau level
    
    // menage...
    delete Ptesttrans;
    delete pyramidfunc;

    // Declaration pyramide pour l'image ref (transformee en niveau _level)
    Pyramid<T>      *Pref = NULL ;
    PyramidFunc<T> *pyramidfunc2 = NULL; 
    
    pyramidfunc2 = new MedianPyramidFunc<T>();
    Pref = new Pyramid<T>(*pyramidfunc2);
    Pref->setRef(ref);
    Pref->setLevel( level );
    
    carto::VolumeRef<T> pyrRef
        = carto::VolumeRef<T>(Pref->item(level)).deepcopy();    // pyrRef est l'image ref degradee niveau level
    
    // menage...
    delete Pref;
    delete pyramidfunc2;



    // Creation des offsets comme pre-calcul des coordonnees
    carto::VolumeRef<int> tmp(_nx, _ny, _nz);
    _offset = tmp.deepcopy();
    _offset2 = tmp.deepcopy();
    int i, j, k;
    std::vector<int> dim = _offset2->getSize();

    for(k = 0; k < dim[2]; ++k)
        for(j = 0; j < dim[1]; ++j)
            for(i = 0; i < dim[0]; ++i)
            {
                _offset(i, j, k) = &pyrRef->at(i, j, k) - &pyrRef->at(0);
                _offset2(i, j, k) = &pyrTrans->at(i, j, k) - &pyrTrans->at(0);
            }

    // Initialisation de moyenne et variance courante pour calcul correlation
    double mRef = 0, sRef = 0, mTest = 0, sTest = 0;

    // Creation d'une image affichant la correlation en chaque point e un niveau de pyramide
    carto::VolumeRef<T> pyrQuality = pyrTrans.deepcopy();

    ////////////////////////
    pyrQuality = 0;
    
    carto::Volume<int>::iterator oit, oit2;
    carto::Volume<int>::const_iterator eoit = _offset->end();  //important optimisation that g++ does not perform
    
    // Calcul de n puissance 4, pour prendre ro carre, pour positivite entre 0 et 1
    double norm = _offset->getSizeX() * _offset->getSizeY() * _offset->getSizeZ() 
                * _offset->getSizeX() * _offset->getSizeY() * _offset->getSizeZ();

    /////////////////////////////////////////


    dim = pyrTrans->getSize();

    for(k = 0; k < dim[2]; ++k)
        for(j = 0; j < dim[1]; ++j)
            for(i = 0; i < dim[0]; ++i)
            {
        /*
                std::cout<<"i, j, k : "<<i<<", "<<j<<", "<<k<<std::endl;
                std::cout<<"i + _nx, j + _ny, k + _nz : "<<i + _nx<<", "<<j + _ny<<", "<<k + _nz23d*_nz<<std::endl;
                std::cout<<"pyrTrans->getSizeX(), pyrTrans->getSizeY(), pyrTrans->getSizeZ() : "<<pyrTrans->getSizeX()<<", "<<pyrTrans->getSizeY()<<", "<<pyrTrans->getSizeZ()<<std::endl;
        */

                if ((i + _nx < dim[0]) && (j + _ny < dim[1]) && (k + _nz23d*_nz < dim[2]))
                {
                    /////////////////////////////////
                    //  std::cout<<"boucle 1"<<std::endl;
                    if (pyrTrans(i+_nx/2, j+_ny/2, k+_nz23d*_nz/2) < thresh)
                    {
                        T *pRef  = &pyrRef->at(i, j, k);
                        T *pTest = &pyrTrans->at(i, j, k);

                        meanVar( pyrTrans, _offset2, i, j , k, mTest, sTest, _ltht, _htht);
                        meanVar( pyrRef, _offset, i, j , k, mRef,  sRef, _lthr, _hthr);
                        double normS = norm * (sRef*sTest);
                        /////////////////////////////////
        //              std::cout<<"boucle 2"<<std::endl;

                        if (normS != 0)
                        {
                            // calcul coefficient de correlation entre les deux blocs courants
                            double correl = 0. ;          
                            for(oit = _offset.begin(), oit2 = _offset2.begin(); oit != eoit; oit++, oit2++)
                            {
                                correl += (*(pRef + *oit) - mRef ) * (*(pTest + *oit2 ) - mTest);
                            }
                            
                            correl *= correl;       // mise au carre
                            correl /= normS;        // normalisation
                            correl = sqrt(correl);
                            
                            //////////////////////////////////////////
        //                  std::cout<<"correl : "<<correl<<std::endl;
        //                  std::cout<<"correl rescale : "<<T((std::numeric_limits<T>::max() -1)*correl + 0.5)<<std::endl;

                            pyrQuality(i+_nx/2, j+_ny/2, k+_nz23d*_nz/2) = T((std::numeric_limits<T>::max() -1)*correl + 0.5);
                        }

                        //////////////////////////////////////////////////////
        //              else pyrQuality(i + _nx/2,j + _ny/2, k + _nz23d*_nz/2) = 1000;
                    }
                    else pyrQuality(i+_nx/2, j+_ny/2, k+_nz23d*_nz/2) = 0;
                }
            }


  return(pyrQuality);
}


#endif

