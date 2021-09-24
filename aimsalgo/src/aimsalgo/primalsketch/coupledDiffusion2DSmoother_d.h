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


#ifndef AIMS_PRIMALSKETCH_COUPLEDDIFFUSION2DSMOOTHER_D_H
#define AIMS_PRIMALSKETCH_COUPLEDDIFFUSION2DSMOOTHER_D_H

#include <cstdlib>
#include <aims/primalsketch/coupledDiffusion2DSmoother.h>
#include <aims/signalfilter/convol.h>
#include <cmath>
#include <cfloat>

//
// WARNING : so far, this class implements a very particular type
// of coupled diffusion with soft constraints, i.e. :
// du/dt=Lapl(u) + div( (grad(u).grad(v)).grad(v) ) - (u-Cu)
// dv/dt=Lapl(v) + div( (grad(u).grad(v)).grad(u) ) - (v-Cv);
//
// which is a heat equation plus a term that keeps
// isocontours of both images as orthogonal as possible
//
// this is diffusion with constraints, that is with constant
// heat sources. So, not much use for anybody but me...
//

namespace aims
{

template<class T> std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> >
CoupledDiffusion2DSmoother<T>::doSmoothing(
  const std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> > & ima,
  const std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> > & constraints,
  int maxiter, bool /*verbose*/ )
{
    if ( maxiter >= 0)
    {
        carto::VolumeRef<T> ima1, ima2;
        ima1=ima.first;
        ima2=ima.second;

        int PAS=20;   //10
        float epsilon=0.01;

        if ((ima.first.getSizeZ()>1) || (ima.second.getSizeZ()>1))
        {
            std::cerr << "coupledDiffusion2DSmoother: only for 2D images !!"
              << std::endl;
            exit(1);
        }
        if ( (!carto::isSameVolumeSize( ima.first, ima.second ))
             || (!carto::isSameVolumeSize( ima.first, constraints.first ))
             || (!carto::isSameVolumeSize( ima.first, constraints.second )) )
        {
            std::cerr << "coupledDiffusion2DSmoother: images do not all have the same size..." << std::endl;
            exit(1);
        }

        int sx=ima.first.getSizeX(), sy=ima.first.getSizeT(), x, y;
        carto::VolumeRef<float> *tmp1_1, *tmp1_2, *swap1;
        carto::VolumeRef<float> *tmp2_1, *tmp2_2, *swap2;
        int i;
        float lapl1,  lapl2, div1, div2, lapx1, lapy1, lapx2, lapy2;

        std::vector<std::pair<int, int> > vcont1, vcont2;
        std::vector<std::pair<int, int> >::iterator pt1, pt2;

        carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
        carto::VolumeRef< float > imaF1( sx, sy ),imaF2( sx, sy ),
                          imaB1( sx, sy ),imaB2( sx, sy ),
                          grad1_x( sx, sy ), grad1_y( sx, sy ),
                          grad2_x( sx, sy ), grad2_y( sx, sy );
        conv.convert( ima.first, imaF1 );
        conv.convert( ima.second, imaF2 );
        float cdiff1, cdiff2, diff1, diff2, diffMax1, diffMax2;
        tmp1_1=&imaF1; tmp1_2=&imaB1;
        tmp2_1=&imaF2; tmp2_2=&imaB2;

        std::cout << "Initalizing images  with constraints" << std::endl;

        for (y=0; y<sy; y++)
            for (x=0; x<sx; x++)
            {
                if ((fabs(constraints.first(x,y)-1) < epsilon) || (fabs(constraints.first(x,y)-80)<epsilon))
                {
                    imaF1(x,y)=(float) constraints.first(x,y);
                    vcont1.push_back(std::pair<int,int>(x,y));
                }
                if ((fabs(constraints.second(x,y)-1) < epsilon) || (fabs(constraints.second(x,y)-80)<epsilon))
                {
                    imaF2(x,y)=(float) constraints.second(x,y);
                    vcont2.push_back(std::pair<int,int>(x,y));
                }
            }

//         cout << "Contraintes 1 :" << endl;
//         for (pt1=vcont1.begin(); pt1!=vcont1.end(); ++pt1)
//             cout << "(" << (*pt1).first << "," << (*pt1).second << ") "; fflush(stdout);
//         cout << endl;
//         cout << "Contraintes 2 :" << endl;
//         for (pt2=vcont2.begin(); pt2!=vcont2.end(); ++pt2)
//             cout << "(" << (*pt2).first << "," << (*pt2).second << ") "; fflush(stdout);
//         cout << endl;
        std::cout << "Starting " << maxiter
          << " iterations of diffusion process" << std::endl;

        int sz=(maxiter/PAS) + 1;
        carto::VolumeRef<T> debug1(sx, sy, sz), debug2(sx, sy, sz);
        carto::VolumeRef<T> grad1x(sx, sy, sz), grad1y(sx, sy, sz), grad2x(sx, sy, sz), grad2y(sx, sy, sz);
        carto::VolumeRef<T> gugv(sx, sy), scal(sx, sy, sz), gu(sx, sy, sz), gv(sx, sy, sz);

        for (y=0; y<sy; y++)
            for (x=0; x<sx; x++)
            {
                debug1(x, y, 0)=(*tmp1_1)(x, y);
                debug2(x, y, 0)=(*tmp2_1)(x, y);
            }
        int z=1;
        diffMax1=diffMax2=0.0;

        for (i=0; i<maxiter; i++)
         {
            if ((i%1)==0) //100
            {
                std::cout << "(t=" << i*_dt << ") -> diff=(" << diffMax1 << ","
                  << diffMax2 << ") - "<< std::endl;;
            }
//             std::cout << "(G"; fflush(stdout);
            diffMax1=diffMax2=0.0;
            // Calcul des gradients de chaque  image, et du produit scalaire
            for (y=0; y<sy; y++)
                for (x=0; x<sx; x++)
                {
                    if (x==0)
                    {
                        grad1_x(x,y)=((*tmp1_1)(x+1,y) - (*tmp1_1)(x,y))/2.0;
                        grad2_x(x,y)=((*tmp2_1)(x+1,y) - (*tmp2_1)(x,y))/2.0;
                    }
                    else if (x==sx-1)
                    {
                        grad1_x(x,y)=((*tmp1_1)(x,y) - (*tmp1_1)(x-1,y))/2.0;
                        grad2_x(x,y)=((*tmp2_1)(x,y) - (*tmp2_1)(x-1,y))/2.0;
                    }
                    else
                    {
                        grad1_x(x,y)=((*tmp1_1)(x+1,y) - (*tmp1_1)(x-1,y))/2.0;
                        grad2_x(x,y)=((*tmp2_1)(x+1,y) - (*tmp2_1)(x-1,y))/2.0;
                    }
                    if (y==0)
                    {
                        grad1_y(x,y)=((*tmp1_1)(x,y+1) - (*tmp1_1)(x,y))/2.0;
                        grad2_y(x,y)=((*tmp2_1)(x,y+1) - (*tmp2_1)(x,y))/2.0;
                    }
                    else if (y==sy-1)
                    {
                        grad1_y(x,y)=((*tmp1_1)(x,y) - (*tmp1_1)(x,y-1))/2.0;
                        grad2_y(x,y)=((*tmp2_1)(x,y) - (*tmp2_1)(x,y-1))/2.0;
                    }
                    else
                    {
                        grad1_y(x,y)=((*tmp1_1)(x,y+1) - (*tmp1_1)(x,y-1))/2.0;
                        grad2_y(x,y)=((*tmp2_1)(x,y+1) - (*tmp2_1)(x,y-1))/2.0;
                    }

                    gugv(x,y)=grad1_x(x,y)*grad2_x(x,y) + grad1_y(x,y)*grad2_y(x,y);

                }

            // lissage de gugv pour stabilit� du sch�ma num�rique...

            Gaussian2DSmoothing<float> g2d(2.0,2.0);
            carto::VolumeRef<float> g1x(sx,sy), g2x(sx,sy), g1y(sx,sy), g2y(sx,sy);
            g1x=g2d.doit(grad1_x);
            g2x=g2d.doit(grad2_x);
            g1y=g2d.doit(grad1_y);
            g2y=g2d.doit(grad2_y);

            for (y=0; y<sy; y++)
                 for (x=0; x<sx; x++)
                 {
                      double prod=g1x(x,y)*g2x(x,y) + g1y(x,y)*g2y(x,y);
                      gugv(x,y)=float(2*prod*exp(-prod*prod));   // Fonction de conductance !!!!!!!
                 }

            // passe de calcul du terme total

            float div1x, div1y, div2x, div2y;
            for (y=0; y<(sy); y++)
                for (x=0; x<(sx); x++)
                {
                    if (x==0)
                    {
                        lapx1=(*tmp1_1)(x+1,y) + (*tmp1_1)(x,y) -2*(*tmp1_1)(x,y);
                        lapx2=(*tmp2_1)(x+1,y) + (*tmp2_1)(x,y) -2*(*tmp2_1)(x,y);
                        div1x=( (g1x(x+1,y)*g2x(x+1,y) + g1y(x+1,y)*g2x(x+1,y) )*g2x(x+1,y)
                              - (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g2x(x,y) )/2.0;
                        div2x=( (g1x(x+1,y)*g2x(x+1,y) + g1y(x+1,y)*g2x(x+1,y) )*g1x(x+1,y)
                              - (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g1x(x,y) )/2.0;
                    }
                    else if (x==(sx-1))
                    {
                        lapx1=(*tmp1_1)(x,y) + (*tmp1_1)(x-1,y) -2*(*tmp1_1)(x,y);
                        lapx2=(*tmp2_1)(x,y) + (*tmp2_1)(x-1,y) -2*(*tmp2_1)(x,y);
                        div1x=( (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g2x(x,y)
                              - (g1x(x-1,y)*g2x(x-1,y) + g1y(x-1,y)*g2x(x-1,y) )*g2x(x-1,y) )/2.0;
                        div2x=( (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g1x(x,y)
                              - (g1x(x-1,y)*g2x(x-1,y) + g1y(x-1,y)*g2x(x-1,y) )*g1x(x-1,y) )/2.0;
                    }
                    else
                    {
                        lapx1=(*tmp1_1)(x+1,y) + (*tmp1_1)(x-1,y) -2*(*tmp1_1)(x,y);
                        lapx2=(*tmp2_1)(x+1,y) + (*tmp2_1)(x-1,y) -2*(*tmp2_1)(x,y);
                        div1x=( (g1x(x+1,y)*g2x(x+1,y) + g1y(x+1,y)*g2x(x+1,y) )*g2x(x+1,y)
                              - (g1x(x-1,y)*g2x(x-1,y) + g1y(x-1,y)*g2x(x-1,y) )*g2x(x-1,y) )/2.0;
                        div2x=( (g1x(x+1,y)*g2x(x+1,y) + g1y(x+1,y)*g2x(x+1,y) )*g1x(x+1,y)
                              - (g1x(x-1,y)*g2x(x-1,y) + g1y(x-1,y)*g2x(x-1,y) )*g1x(x-1,y) )/2.0;
                    }
                    if (y==0)
                    {
                        lapy1=(*tmp1_1)(x,y+1) + (*tmp1_1)(x,y) -2*(*tmp1_1)(x,y);
                        lapy2=(*tmp2_1)(x,y+1) + (*tmp2_1)(x,y) -2*(*tmp2_1)(x,y);
                        div1y=( (g1x(x,y+1)*g2x(x,y+1) + g1y(x,y+1)*g2x(x,y+1) )*g2y(x,y+1)
                              - (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g2y(x,y) )/2.0;
                        div2y=( (g1x(x,y+1)*g2x(x,y+1) + g1y(x,y+1)*g2x(x,y+1) )*g1y(x,y+1)
                              - (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g1y(x,y) )/2.0;
                    }
                    else if (y==(sy-1))
                    {
                        lapy1=(*tmp1_1)(x,y) + (*tmp1_1)(x,y-1) -2*(*tmp1_1)(x,y);
                        lapy2=(*tmp2_1)(x,y) + (*tmp2_1)(x,y-1) -2*(*tmp2_1)(x,y);
                        div1y=( (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g2y(x,y)
                              - (g1x(x,y-1)*g2x(x,y-1) + g1y(x,y-1)*g2x(x,y-1) )*g2y(x,y-1) )/2.0;
                        div2y=( (g1x(x,y)*g2x(x,y) + g1y(x,y)*g2x(x,y) )*g1y(x,y)
                              - (g1x(x,y-1)*g2x(x,y-1) + g1y(x,y-1)*g2x(x,y-1) )*g1y(x,y-1) )/2.0;
                    }
                    else
                    {
                        lapy1=(*tmp1_1)(x,y+1) + (*tmp1_1)(x,y-1) -2*(*tmp1_1)(x,y);
                        lapy2=(*tmp2_1)(x,y+1) + (*tmp2_1)(x,y-1) -2*(*tmp2_1)(x,y);
                        div1y=( (g1x(x,y+1)*g2x(x,y+1) + g1y(x,y+1)*g2x(x,y+1) )*g2y(x,y+1)
                              - (g1x(x,y-1)*g2x(x,y-1) + g1y(x,y-1)*g2x(x,y-1) )*g2y(x,y-1) )/2.0;
                        div2y=( (g1x(x,y+1)*g2x(x,y+1) + g1y(x,y+1)*g2x(x,y+1) )*g1y(x,y+1)
                              - (g1x(x,y-1)*g2x(x,y-1) + g1y(x,y-1)*g2x(x,y-1) )*g1y(x,y-1) )/2.0;
                    }

                    lapl1=lapx1+lapy1;
                    lapl2=lapx2+lapy2;
                    div1 = div1x+div1y;
                    div2 = div2x+div2y;

                    if ((fabs(constraints.first(x,y))>epsilon) && (fabs(constraints.first(x,y)-0) > epsilon) && (fabs(constraints.first(x,y)-80) > epsilon))
                         cdiff1=(*tmp1_1)(x,y) - constraints.first(x,y);
                    else
                         cdiff1=0.0;
                    if ((fabs(constraints.second(x,y)) > epsilon) && (fabs(constraints.second(x,y)-0) > epsilon) && (fabs(constraints.second(x,y)-80)> epsilon))
                         cdiff2=(*tmp2_1)(x,y) - constraints.second(x,y);
                    else
                         cdiff2=0.0;
                    (*tmp1_2)(x,y) = (*tmp1_1)(x,y) + _dt*(_alpha*lapl1 + _beta*div1 - _gamma*cdiff1);
                    (*tmp2_2)(x,y) = (*tmp2_1)(x,y) + _dt*(_alpha*lapl2 + _beta*div2 - _gamma*cdiff2);
                    diff1 = fabs((*tmp1_2)(x,y) - (*tmp1_1)(x,y));
                    diff2 = fabs((*tmp2_2)(x,y) - (*tmp2_1)(x,y));

                    diffMax1 += diff1; diffMax2 += diff2;
                }
//             std::cout << " OK) "; fflush(stdout);
            diffMax1=diffMax1/float(sx*sy);
            diffMax2=diffMax2/float(sx*sy);
            // on fixe les contraintes � leur valeur initiale
            for (pt1=vcont1.begin(); pt1!=vcont1.end(); ++pt1)
                (*tmp1_2)((*pt1).first, (*pt1).second)= (float) constraints.first((*pt1).first, (*pt1).second);
            for (pt2=vcont2.begin(); pt2!=vcont2.end(); ++pt2)
                (*tmp2_2)((*pt2).first, (*pt2).second)= (float) constraints.second((*pt2).first, (*pt2).second);
            swap1=tmp1_1;
         tmp1_1=tmp1_2;
         tmp1_2=swap1;
            swap2=tmp2_1;
         tmp2_1=tmp2_2;
         tmp2_2=swap2;

            if ((i%PAS)==0)
            {
                for (y=0; y<sy; y++)
                    for (x=0; x<sx; x++)
                    {
                        debug1(x, y, z)=(*tmp1_1)(x, y);
                        debug2(x, y, z)=(*tmp2_1)(x, y);
                        grad1x(x, y, z)=grad1_x(x,y);
                        grad1y(x, y, z)=grad1_y(x,y);
                        grad2x(x, y, z)=grad2_x(x,y);
                        grad2y(x, y, z)=grad2_y(x,y);
                        scal(x, y, z)=gugv(x,y);
                    }
                z++;
            }
        }

        std::cout << "Finished" << std::endl;
        carto::Converter< carto::VolumeRef<float>, carto::VolumeRef<T> > conv2;
        carto::VolumeRef<T>  out1( sx, sy), out2(sx,sy);
        conv2.convert( (*tmp1_1),out1);
        conv2.convert( (*tmp2_1),out2);

        //Pour debug : evolution des iso-contours

        std::cout << "Computing and writing iso-contours" << std::endl;
        carto::VolumeRef<uint8_t>  iso( sx, sy, sz);
        for (z=0; z<sz; z++)
             for (y=0; y<sy-1; y++)
                  for (x=0; x<sx-1; x++)
                  {
                       iso(x,y,z)=0;
                  }
        for (z=0; z<sz; z++)
             for (y=0; y<sy-1; y++)
                  for (x=0; x<sx-1; x++)
                  {
                       int value;
                       for (value=0; value < 80; value=value+5)
                       {
//                             if ((constraints.first(x,y)>0) || (constraints.second(x,y)>0))
//                                  iso(x,y,z)=2;
                            if ((((debug1(x,y,z)-float(value))*(debug1(x,y+1,z)-float(value)))<=0)
                              || (((debug1(x,y,z)-float(value))*(debug1(x+1,y,z)-float(value)))<=0))
                                 iso(x,y,z)=1;
                            if ((((debug2(x,y,z)-float(value))*(debug2(x,y+1,z)-float(value)))<=0)
                              || (((debug2(x,y,z)-float(value))*(debug2(x+1,y,z)-float(value)))<=0))
                                 iso(x,y,z)=1;
                       }
                  }

        /*
        Writer<carto::VolumeRef<uint8_t> > writerIso( "grille.ima" );
        writerIso.write(iso);

        Writer<carto::VolumeRef<float> > writerD1( "evolution1.ima" );
        Writer<carto::VolumeRef<float> > writerD2( "evolution2.ima" );
        writerD1.write(debug1);
        writerD2.write(debug2);
        Writer<carto::VolumeRef<float> > writerG1x( "grad1x.ima" );
        Writer<carto::VolumeRef<float> > writerG2x( "grad2x.ima" );
        writerG1x.write(grad1x);
        writerG2x.write(grad2x);
        Writer<carto::VolumeRef<float> > writerG1y( "grad1y.ima" );
        Writer<carto::VolumeRef<float> > writerG2y( "grad2y.ima" );
        writerG1y.write(grad1y);
        writerG2y.write(grad2y);
        Writer<carto::VolumeRef<float> > writerGuGv( "gugv.ima" );
        writerGuGv.write(scal);
        */

        return std::make_pair( out1, out2 );
    }
    else
    {
        std::cerr << "coupledDiffusion2DSmoother: must have tIn < tOut"
          << std::endl;
        exit(1);
    }

}

}

#endif
