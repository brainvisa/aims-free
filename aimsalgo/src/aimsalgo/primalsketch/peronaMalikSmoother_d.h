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


#ifndef AIMS_PRIMALSKETCH_PERONAMALIKSMOOTHER_D_H
#define AIMS_PRIMALSKETCH_PERONAMALIKSMOOTHER_D_H

#include <cstdlib>
#include <aims/primalsketch/peronaMalikSmoother.h>
#include <aims/signalfilter/ggradient.h>
#include <aims/signalfilter/gjacobian.h>

namespace aims
{

  template<class T> carto::VolumeRef<T>
  PeronaMalikSmoother<T>::doSmoothing( const carto::VolumeRef<T> & ima,
                                       int maxiter, bool /*verbose*/ )
  {
    if (maxiter >= 0)
    {
      carto::VolumeRef<float> kernel( 3, 3, 3, 1,
                                      carto::AllocatorContext::fast() );
      carto::VolumeRef<float> *tmp1, *tmp2, *swap, grad;
      int i;
      int x,y,z, count=0;
      int sx=ima->getSizeX(), sy=ima->getSizeY(), sz=ima->getSizeZ();
      std::map<float, int> valueSet;
      std::map<float,int>::iterator itVal;
      float nbThresh;
      float gxm, gxp, gym, gyp, gzm, gzp, cxm, cxp, cym, cyp, czm, czp;

      GaussianGradient<float>  gaussGrad(_sigma, _sigma, _sigma);
      GaussianJacobian<float> gaussJac(_sigma, _sigma, _sigma);
      AimsVector< carto::VolumeRef< float >, 3 > jaco;

      carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> >
        conv;
      carto::VolumeRef< float > imaF( ima->getSize() ),
                                ima2( ima->getSize() );
      conv.convert( ima, imaF );


      std::cout << "Evaluating gradient bound : " << std::endl;

      grad=gaussGrad.doit(imaF);
//       Writer<carto::VolumeRef<float> > writerG( "gradient.ima" );
//       writerG.write( grad );

      // Evaluation of the gradient parameter from K

      std::cout << "Ordering values" << std::endl;

      nbThresh=sx*sz*sy*_K;

      for (z=0; z<sz; z++)
        for (y=0; y<sy; y++)
          for (x=0; x<sx; x++)
          {
            if (valueSet.find(grad(x,y,z)) != valueSet.end())
              valueSet[grad(x,y,z)]++;
            else
              valueSet.insert(std::pair<float,int>(grad(x,y,z), 1));
          }
      itVal=valueSet.begin();

      std::cout << "Looking for bound with K= " << _K << " at value number " << nbThresh << std::endl;
      for (; itVal!=valueSet.end(); ++itVal)
      {
        count+=(*itVal).second;
        if ((count>=nbThresh) && ((count-1)<nbThresh))
          _gradK=(*itVal).first;
      }
      std::cout << "Gradient bound has been set to " << _gradK << std::endl;

      tmp1=&imaF; tmp2=&ima2;
      std::cout << "Starting " << maxiter << " iterations of diffusion process" << std::endl;

      for (i=0; i<maxiter; i++)
      {
        std::cout << "+" << std::flush;
        grad=gaussGrad.doit((*tmp1));

        for (z=0; z<sz; z++)
          for (y=0; y<sy; y++)
            for (x=0; x<sx; x++)
            {
              if (x==0)
              {
                gxm=0; gxp=(*tmp1)(x+1,y,z) - (*tmp1)(x,y,z);
                cxm=0; cxp=conductance( (grad(x+1,y,z)+grad(x,y,z))/2.0);
              }
              else if (x==sx-1)
              {
                gxm=(*tmp1)(x-1,y,z) - (*tmp1)(x,y,z); gxp=0;
                cxm=conductance( (grad(x-1,y,z)+grad(x,y,z))/2.0); cxp=0;
              }
              else
              {
                gxm=(*tmp1)(x-1,y,z) - (*tmp1)(x,y,z); gxp=(*tmp1)(x+1,y,z) - (*tmp1)(x,y,z);
                cxm=conductance( (grad(x-1,y,z)+grad(x,y,z))/2.0); cxp=conductance( (grad(x+1,y,z)+grad(x,y,z))/2.0);
              }

              if (y==0)
              {
                gym=0; gyp=(*tmp1)(x,y+1,z) - (*tmp1)(x,y,z);
                cym=0; cyp=conductance( (grad(x,y+1,z)+grad(x,y,z))/2.0);
              }
              else if (y==sy-1)
              {
                gym=(*tmp1)(x,y-1,z) - (*tmp1)(x,y,z); gyp=0;
                cym=conductance( (grad(x,y-1,z)+grad(x,y,z))/2.0); cyp=0;
              }
              else
              {
                gym=(*tmp1)(x,y-1,z) - (*tmp1)(x,y,z); gyp=(*tmp1)(x,y+1,z) - (*tmp1)(x,y,z);
                cym=conductance( (grad(x,y-1,z)+grad(x,y,z))/2.0); cyp=conductance( (grad(x,y+1,z)+grad(x,y,z))/2.0);
              }

              if (z==0)
              {
                gzm=0; gzp=(*tmp1)(x,y,z+1) - (*tmp1)(x,y,z);
                czm=0; czp=conductance( (grad(x,y,z+1)+grad(x,y,z))/2.0);
              }
              else if (z==sz-1)
              {
                gzm=(*tmp1)(x,y,z-1) - (*tmp1)(x,y,z); gzp=0;
                czm=conductance( (grad(x,y,z-1)+grad(x,y,z))/2.0); czp=0;
              }
              else
              {
                gzm=(*tmp1)(x,y,z-1) - (*tmp1)(x,y,z); gzp=(*tmp1)(x,y,z+1) - (*tmp1)(x,y,z);
                czm=conductance( (grad(x,y,z-1)+grad(x,y,z))/2.0); czp=conductance( (grad(x,y,z+1)+grad(x,y,z))/2.0);
              }

              (*tmp2)(x,y,z)=(*tmp1)(x,y,z) + _dt *
                                      ( cxm*gxm + cxp*gxp
                                      +cym*gym + cyp*gyp
                                      +czm*gzm + czp*gzp );
            }
        swap=tmp1;
          tmp1=tmp2;
          tmp2=swap;
      }
      std::cout << "Finished" << std::endl;
      carto::Converter< carto::VolumeRef<float>, carto::VolumeRef<T> >
        conv2;
      carto::VolumeRef<T>  ima3( ima->getSize() );
      conv2.convert( (*tmp1), ima3);

      return ima3;
    }
    else
    {
      std::cerr << "PeronaMalik smoother  must have tIn < tOut" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

}

#endif
