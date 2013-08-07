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


#ifndef AIMS_MATH_GRADIENT_H
#define AIMS_MATH_GRADIENT_H


#include <aims/data/fastAllocationData.h>

/** Gradient methods.
   There are 3 possible numerical estimations of the gradient :
    \\ AIMS_GRADIENT_CENTRAL : central approximation
    \\ AIMS_GRADIENT_DMINUS  : left approximation
    \\ AIMS_GRADIENT_DPLUS   : right approximation
*/
enum AimsGradientMethod
{
  ///
  AIMS_GRADIENT_CENTRAL,
  ///
  AIMS_GRADIENT_DMINUS,
  ///
  AIMS_GRADIENT_DPLUS
};


/**@name The different gradients
*/
//@{
/** First order*/
enum AimsFirstOrderGradient
{
  ///
  AIMS_GRADIENT_X,
  ///
  AIMS_GRADIENT_Y,
  ///
  AIMS_GRADIENT_Z,
  ///
  AIMS_GRADIENT_T
};

/** Second order*/
enum AimsSecondOrderGradient
{
  ///
  AIMS_GRADIENT_XX,
  ///
  AIMS_GRADIENT_XY,
  ///
  AIMS_GRADIENT_XZ,
  ///
  AIMS_GRADIENT_XT,
  ///
  AIMS_GRADIENT_YX,
  ///
  AIMS_GRADIENT_YY,
  ///
  AIMS_GRADIENT_YZ,
  ///
  AIMS_GRADIENT_YT,
  ///
  AIMS_GRADIENT_ZX,
  ///
  AIMS_GRADIENT_ZY,
  ///
  AIMS_GRADIENT_ZZ,
  ///
  AIMS_GRADIENT_ZT,
  ///
  AIMS_GRADIENT_TX,
  ///
  AIMS_GRADIENT_TY,
  ///
  AIMS_GRADIENT_TZ,
  ///
  AIMS_GRADIENT_TT
};

/** Third order*/
enum AimsThirdOrderGradient
{
  ///
  AIMS_GRADIENT_XXX,
  ///
  AIMS_GRADIENT_XXY,
  ///
  AIMS_GRADIENT_XXZ,
  ///
  AIMS_GRADIENT_XXT,
  ///
  AIMS_GRADIENT_XYX,
  ///
  AIMS_GRADIENT_XYY,
  ///
  AIMS_GRADIENT_XYZ,
  ///
  AIMS_GRADIENT_XYT,
  ///
  AIMS_GRADIENT_XZX,
  ///
  AIMS_GRADIENT_XZY,
  ///
  AIMS_GRADIENT_XZZ,
  ///
  AIMS_GRADIENT_XZT,
  ///
  AIMS_GRADIENT_XTX,
  ///
  AIMS_GRADIENT_XTY,
  ///
  AIMS_GRADIENT_XTZ,
  ///
  AIMS_GRADIENT_XTT,
  ///
  AIMS_GRADIENT_YXX,
  ///
  AIMS_GRADIENT_YXY,
  ///
  AIMS_GRADIENT_YXZ,
  ///
  AIMS_GRADIENT_YXT,
  ///
  AIMS_GRADIENT_YYX,
  ///
  AIMS_GRADIENT_YYY,
  ///
  AIMS_GRADIENT_YYZ,
  ///
  AIMS_GRADIENT_YYT,
  ///
  AIMS_GRADIENT_YZX,
  ///
  AIMS_GRADIENT_YZY,
  ///
  AIMS_GRADIENT_YZZ,
  ///
  AIMS_GRADIENT_YZT,
  ///
  AIMS_GRADIENT_YTX,
  ///
  AIMS_GRADIENT_YTY,
  ///
  AIMS_GRADIENT_YTZ,
  ///
  AIMS_GRADIENT_YTT,
  ///
  AIMS_GRADIENT_ZXX,
  ///
  AIMS_GRADIENT_ZXY,
  ///
  AIMS_GRADIENT_ZXZ,
  ///
  AIMS_GRADIENT_ZXT,
  ///
  AIMS_GRADIENT_ZYX,
  ///
  AIMS_GRADIENT_ZYY,
  ///
  AIMS_GRADIENT_ZYZ,
  ///
  AIMS_GRADIENT_ZYT,
  ///
  AIMS_GRADIENT_ZZX,
  ///
  AIMS_GRADIENT_ZZY,
  ///
  AIMS_GRADIENT_ZZZ,
  ///
  AIMS_GRADIENT_ZZT,
  ///
  AIMS_GRADIENT_ZTX,
  ///
  AIMS_GRADIENT_ZTY,
  ///
  AIMS_GRADIENT_ZTZ,
  ///
  AIMS_GRADIENT_ZTT,
  ///
  AIMS_GRADIENT_TXX,
  ///
  AIMS_GRADIENT_TXY,
  ///
  AIMS_GRADIENT_TXZ,
  ///
  AIMS_GRADIENT_TXT,
  ///
  AIMS_GRADIENT_TYX,
  ///
  AIMS_GRADIENT_TYY,
  ///
  AIMS_GRADIENT_TYZ,
  ///
  AIMS_GRADIENT_TYT,
  ///
  AIMS_GRADIENT_TZX,
  ///
  AIMS_GRADIENT_TZY,
  ///
  AIMS_GRADIENT_TZZ,
  ///
  AIMS_GRADIENT_TZT,
  ///
  AIMS_GRADIENT_TTX,
  ///
  AIMS_GRADIENT_TTY,
  ///
  AIMS_GRADIENT_TTZ,
  ///
  AIMS_GRADIENT_TTT
};

//@}


/** The template class for estimating gradients.
    The template argument gives the type of the basic item.
*/
template <class C>
class AimsGradient
{ protected :
  /**@name Data*/
  //@{
  /// Type of the gradient
    int _type;
  //@}

  public :
  /**@name Constructor and destructor*/
  //@{
  /** One argument can be give to the constructor :
      @param type method used for the estimation [default=AIMS_GRADIENT_CENTRAL]
  */
  AimsGradient(int type=AIMS_GRADIENT_CENTRAL) { _type = type;}
  /// Destructor does nothing
  virtual ~ AimsGradient() {}
  //@} 

  /**@name First order*/
  //@{
  /// d(data)/dx = AimsGradientX(data)
  AimsData<C> X(const AimsData<C> &data);
  /// d(data)/dy = AimsGradientY(data)
  AimsData<C> Y(const AimsData<C> &data);
  /// d(data)/dz = AimsGradientZ(data)
  AimsData<C> Z(const AimsData<C> &data);
  /// d(data)/dt = AimsGradientT(data)
  AimsData<C> T(const AimsData<C> &data);
  //@}

  /**@name Second order*/
  //@{
  /// d2(data)/dx2 = XX(data)
  AimsData<C> XX(const AimsData<C> &data);
  /// d2(data)/dxdy = XY(data)
  AimsData<C> XY(const AimsData<C> &data);
  /// d2(data)/dxdz = XZ(data)
  AimsData<C> XZ(const AimsData<C> &data);
  /// d2(data)/dxdt = XT(data)
  AimsData<C> XT(const AimsData<C> &data);
  /// d2(data)/dydx = YX(data)
  AimsData<C> YX(const AimsData<C> &data);
  /// d2(data)/dy2 = YY(data)
  AimsData<C> YY(const AimsData<C> &data);
  /// d2(data)/dydz = YZ(data)
  AimsData<C> YZ(const AimsData<C> &data);
  /// d2(data)/dydt = YT(data)
  AimsData<C> YT(const AimsData<C> &data);
  /// d2(data)/dzdx = ZX(data)
  AimsData<C> ZX(const AimsData<C> &data);
  /// d2(data)/dzdy = ZY(data)
  AimsData<C> ZY(const AimsData<C> &data);
  /// d2(data)/dz2 = ZZ(data)
  AimsData<C> ZZ(const AimsData<C> &data);
  /// d2(data)/dzdt = ZT(data)
  AimsData<C> ZT(const AimsData<C> &data);
  /// d2(data)/dtdx = TX(data)
  AimsData<C> TX(const AimsData<C> &data);
  /// d2(data)/dtdy = TY(data)
  AimsData<C> TY(const AimsData<C> &data);
  /// d2(data)/dtdz = TZ(data)
  AimsData<C> TZ(const AimsData<C> &data);
  /// d2(data)/dt2 = TT(data)
  AimsData<C> TT(const AimsData<C> &data);
  //@}
    
  /**@name Third order*/
  //@{
  /// d3(data)/dx3 = XXX(data)
  AimsData<C> XXX(const AimsData<C> &data);
  /// d3(data)/dx2dy = XXY(data)
  AimsData<C> XXY(const AimsData<C> &data);
  /// d3(data)/dx2dz = XXZ(data)
  AimsData<C> XXZ(const AimsData<C> &data);
  /// d3(data)/dx2dt = XXT(data)
  AimsData<C> XXT(const AimsData<C> &data);
  /// d3(data)/dxdydx = XYX(data)
  AimsData<C> XYX(const AimsData<C> &data);
  /// d3(data)/dxdy2 = XYY(data)
  AimsData<C> XYY(const AimsData<C> &data);
  /// d3(data)/dxdydz = XYZ(data)
  AimsData<C> XYZ(const AimsData<C> &data);
  /// d3(data)/dxdydt = XYT(data)
  AimsData<C> XYT(const AimsData<C> &data);
  /// d3(data)/dxdzdx = XZX(data)
  AimsData<C> XZX(const AimsData<C> &data);
  /// d3(data)/dxdzdy = XZY(data)
  AimsData<C> XZY(const AimsData<C> &data);
  /// d3(data)/dxdz2 = XZZ(data)
  AimsData<C> XZZ(const AimsData<C> &data);
  /// d3(data)/dxdzdt = XZT(data)
  AimsData<C> XZT(const AimsData<C> &data);
  /// d3(data)/dxdtdx = XTX(data)
  AimsData<C> XTX(const AimsData<C> &data);
  /// d3(data)/dxdtdy = XTY(data)
  AimsData<C> XTY(const AimsData<C> &data);
  /// d3(data)/dxdtdz = XTZ(data)
  AimsData<C> XTZ(const AimsData<C> &data);
  /// d3(data)/dxdt2 = XTT(data)
  AimsData<C> XTT(const AimsData<C> &data);
  /// d3(data)/dydx2 = YXX(data)
  AimsData<C> YXX(const AimsData<C> &data);
  /// d3(data)/dydxdy = YXY(data)
  AimsData<C> YXY(const AimsData<C> &data);
  /// d3(data)/dydxdz = YXZ(data)
  AimsData<C> YXZ(const AimsData<C> &data);
  /// d3(data)/dydxdt = YXT(data)
  AimsData<C> YXT(const AimsData<C> &data);
  /// d3(data)/dy2dx = YYX(data)
  AimsData<C> YYX(const AimsData<C> &data);
  /// d3(data)/dy3 = YYY(data)
  AimsData<C> YYY(const AimsData<C> &data);
  /// d3(data)/dy2dz = YYZ(data)
  AimsData<C> YYZ(const AimsData<C> &data);
  /// d3(data)/dy2dt = YYT(data)
  AimsData<C> YYT(const AimsData<C> &data);
  /// d3(data)/dydzdx = YZX(data)
  AimsData<C> YZX(const AimsData<C> &data);
  /// d3(data)/dydzdy = YZY(data)
  AimsData<C> YZY(const AimsData<C> &data);
  /// d3(data)/dydz2 = YZZ(data)
  AimsData<C> YZZ(const AimsData<C> &data);
  /// d3(data)/dydzdt = YZT(data)
  AimsData<C> YZT(const AimsData<C> &data);
  /// d3(data)/dydtdx = YTX(data)
  AimsData<C> YTX(const AimsData<C> &data);
  /// d3(data)/dydtdy = YTY(data)
  AimsData<C> YTY(const AimsData<C> &data);
  /// d3(data)/dydtdz = YTZ(data)
  AimsData<C> YTZ(const AimsData<C> &data);
  /// d3(data)/dydt2 = YTT(data)
  AimsData<C> YTT(const AimsData<C> &data);
  /// d3(data)/dzdx2 = ZXX(data)
  AimsData<C> ZXX(const AimsData<C> &data);
  /// d3(data)/dzdxdy = ZXY(data)
  AimsData<C> ZXY(const AimsData<C> &data);
  /// d3(data)/dzdxdz = ZXZ(data)
  AimsData<C> ZXZ(const AimsData<C> &data);
  /// d3(data)/dzdxdt = ZXT(data)
  AimsData<C> ZXT(const AimsData<C> &data);
  /// d3(data)/dzdydx = ZYX(data)
  AimsData<C> ZYX(const AimsData<C> &data);
  /// d3(data)/dzdy2 = ZYY(data)
  AimsData<C> ZYY(const AimsData<C> &data);
  /// d3(data)/dzdydz = ZYZ(data)
  AimsData<C> ZYZ(const AimsData<C> &data);
  /// d3(data)/dzdydt = ZYT(data)
  AimsData<C> ZYT(const AimsData<C> &data);
  /// d3(data)/dz2dx = ZZX(data)
  AimsData<C> ZZX(const AimsData<C> &data);
  /// d3(data)/dz2dy = ZZY(data)
  AimsData<C> ZZY(const AimsData<C> &data);
  /// d3(data)/dz3 = ZZZ(data)
  AimsData<C> ZZZ(const AimsData<C> &data);
  /// d3(data)/dz2dt = ZZT(data)
  AimsData<C> ZZT(const AimsData<C> &data);
  /// d3(data)/dzdtdx = ZTX(data)
  AimsData<C> ZTX(const AimsData<C> &data);
  /// d3(data)/dzdtdy = ZTY(data)
  AimsData<C> ZTY(const AimsData<C> &data);
  /// d3(data)/dzdtdz = ZTZ(data)
  AimsData<C> ZTZ(const AimsData<C> &data);
  /// d3(data)/dzdt2 = ZTT(data)
  AimsData<C> ZTT(const AimsData<C> &data);
  /// d3(data)/dtdx2 = TXX(data)
  AimsData<C> TXX(const AimsData<C> &data);
  /// d3(data)/dtdxdy = TXY(data)
  AimsData<C> TXY(const AimsData<C> &data);
  /// d3(data)/dtdxdz = TXZ(data)
  AimsData<C> TXZ(const AimsData<C> &data);
  /// d3(data)/dtdxdt = TXT(data)
  AimsData<C> TXT(const AimsData<C> &data);
  /// d3(data)/dtdydx = TYX(data)
  AimsData<C> TYX(const AimsData<C> &data);
  /// d3(data)/dtdy2 = TYY(data)
  AimsData<C> TYY(const AimsData<C> &data);
  /// d3(data)/dtdydz = TYZ(data)
  AimsData<C> TYZ(const AimsData<C> &data);
  /// d3(data)/dtdydt = TYT(data)
  AimsData<C> TYT(const AimsData<C> &data);
  /// d3(data)/dtdzdx = TZX(data)
  AimsData<C> TZX(const AimsData<C> &data);
  /// d3(data)/dtdzdy = TZY(data)
  AimsData<C> TZY(const AimsData<C> &data);
  /// d3(data)/dtdz2 = TZZ(data)
  AimsData<C> TZZ(const AimsData<C> &data);
  /// d3(data)/dtdzdt = TZT(data)
  AimsData<C> TZT(const AimsData<C> &data);
  /// d3(data)/dt2dx = TTX(data)
  AimsData<C> TTX(const AimsData<C> &data);
  /// d3(data)/dt2dy = TTY(data)
  AimsData<C> TTY(const AimsData<C> &data);
  /// d3(data)/dt2dz = TTZ(data)
  AimsData<C> TTZ(const AimsData<C> &data);
  /// d3(data)/dt3 = TTT(data)
  AimsData<C> TTT(const AimsData<C> &data);
  //@}
};




/* gradient 1st order */

template <class C> inline
AimsData<C> AimsGradient<C>::X(const AimsData<C> &data)
{ int    beginx=0,endx=0,offx1=0,offx2=0,divx=0;

  aims::AimsFastAllocationData<C> grad(data.dimX(),data.dimY(),data.dimZ(),data.dimT(),
                   data.borderWidth());
  grad = (C)0;
  grad.fillBorder(0);
  grad.setHeader(data.header()->cloneHeader());

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : beginx = 1;endx = data.dimX()-1;
                                 offx1=1;offx2=1;divx=2;break;
    case AIMS_GRADIENT_DMINUS  : beginx = 1;endx = data.dimX()  ;
                                 offx1=1;offx2=0;divx=1;break;
    case AIMS_GRADIENT_DPLUS   : beginx = 0;endx = data.dimX()-1;
                                 offx1=0;offx2=1;divx=1;break;
  }

  for (int t=0;t<data.dimT();t++)
    for (int z=0;z<data.dimZ();z++)
      for (int y=0;y<data.dimY();y++)
        for (int x=beginx;x<endx;x++)
          grad(x,y,z,t) = (C)( ((float)data(x+offx2,y,z,t) - 
                                (float)data(x-offx1,y,z,t)) / (float)divx);
  return(grad);
}


template <class C> inline
AimsData<C> AimsGradient<C>::Y(const AimsData<C> &data)
{ int    beginy=0,endy=0,offy1=0,offy2=0,divy=0;

  aims::AimsFastAllocationData<C> grad(data.dimX(),data.dimY(),data.dimZ(),data.dimT(),
                     data.borderWidth());
  grad = (C)0;
  grad.fillBorder(0);
  grad.setHeader(data.header()->cloneHeader());

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : beginy = 1;endy = data.dimY()-1;
                                 offy1=1;offy2=1;divy=2;break;
    case AIMS_GRADIENT_DMINUS  : beginy = 1;endy = data.dimY()  ;
                                 offy1=1;offy2=0;divy=1;break;
    case AIMS_GRADIENT_DPLUS   : beginy = 0;endy = data.dimY()-1;
                                 offy1=0;offy2=1;divy=1;break;
  }

  for (int t=0;t<data.dimT();t++)
    for (int z=0;z<data.dimZ();z++)
      for (int y=beginy;y<endy;y++)
        for (int x=0;x<data.dimX();x++)
          grad(x,y,z,t) = (C)(((float)data(x,y+offy2,z,t) - 
                               (float)data(x,y-offy1,z,t)) / (float)divy);
  return(grad);
}


template <class C> inline
AimsData<C> AimsGradient<C>::Z(const AimsData<C> &data)
{ int    beginz=0,endz=0,offz1=0,offz2=0,divz=0;

  aims::AimsFastAllocationData<C> grad(data.dimX(),data.dimY(),data.dimZ(),data.dimT(),
                     data.borderWidth());
  grad = (C)0;
  grad.fillBorder(0);
  grad.setHeader(data.header()->cloneHeader());

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : beginz = 1;endz = data.dimZ()-1;
                                 offz1=1;offz2=1;divz=2;break;
    case AIMS_GRADIENT_DMINUS  : beginz = 1;endz = data.dimZ()  ;
                                 offz1=1;offz2=0;divz=1;break;
    case AIMS_GRADIENT_DPLUS   : beginz = 0;endz = data.dimZ()-1;
                                 offz1=0;offz2=1;divz=1;break;
  }

  for (int t=0;t<data.dimT();t++)
    for (int z=beginz;z<endz;z++)
      for (int y=0;y<data.dimY();y++)
        for (int x=0;x<data.dimX();x++)
          grad(x,y,z,t) = (C)(((float)data(x,y,z+offz2,t) - 
                               (float)data(x,y,z-offz1,t)) / (float)divz);
  return(grad);
}


template <class C> inline
AimsData<C> AimsGradient<C>::T(const AimsData<C> &data)
{ int    begint=0,endt=0,offt1=0,offt2=0,divt=0;

  aims::AimsFastAllocationData<C> grad(data.dimX(),data.dimY(),data.dimZ(),data.dimT(),
                     data.borderWidth());
  grad = (C)0;
  grad.fillBorder(0);
  grad.setHeader(data.header()->cloneHeader());

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : begint = 1;endt = data.dimY()-1;
                                 offt1=1;offt2=1;divt=2;break;
    case AIMS_GRADIENT_DMINUS  : begint = 1;endt = data.dimY()  ;
                                 offt1=1;offt2=0;divt=1;break;
    case AIMS_GRADIENT_DPLUS   : begint = 0;endt = data.dimY()-1;
                                 offt1=0;offt2=1;divt=1;break;
  }

  for (int t=begint;t<endt;t++)
    for (int z=0;z<data.dimZ();z++)
      for (int y=0;y<data.dimY();y++)
        for (int x=0;x<data.dimX();x++)
          grad(x,y,z,t) = (C)(((float)data(x,y,z,t+offt2) - 
                               (float)data(x,y,z,t-offt1)) / (float)divt);
  return(grad);
}


/* gradient 2nd order */

template <class C> inline
AimsData<C> AimsGradient<C>::XX(const AimsData<C> &data)
{ return(X(X(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XY(const AimsData<C> &data)
{ return(X(Y(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XZ(const AimsData<C> &data)
{ return(X(Z(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XT(const AimsData<C> &data)
{ return(X(T(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::YX(const AimsData<C> &data)
{ return(Y(X(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YY(const AimsData<C> &data)
{ return(Y(Y(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YZ(const AimsData<C> &data)
{ return(Y(Z(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YT(const AimsData<C> &data)
{ return(Y(T(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::ZX(const AimsData<C> &data)
{ return(Z(X(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZY(const AimsData<C> &data)
{ return(Z(Y(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZZ(const AimsData<C> &data)
{ return(Z(Z(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZT(const AimsData<C> &data)
{ return(Z(T(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::TX(const AimsData<C> &data)
{ return(T(X(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TY(const AimsData<C> &data)
{ return(T(Y(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TZ(const AimsData<C> &data)
{ return(T(Z(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TT(const AimsData<C> &data)
{ return(T(T(data)));
}


/* gradient 3rd order */

template <class C> inline
AimsData<C> AimsGradient<C>::XXX(const AimsData<C> &data)
{ return(X(XX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XXY(const AimsData<C> &data)
{ return(X(XY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XXZ(const AimsData<C> &data)
{ return(X(XZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XXT(const AimsData<C> &data)
{ return(X(XT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::XYX(const AimsData<C> &data)
{ return(X(YX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XYY(const AimsData<C> &data)
{ return(X(YY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XYZ(const AimsData<C> &data)
{ return(X(YZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XYT(const AimsData<C> &data)
{ return(X(YT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::XZX(const AimsData<C> &data)
{ return(X(ZX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XZY(const AimsData<C> &data)
{ return(X(ZY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XZZ(const AimsData<C> &data)
{ return(X(ZZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XZT(const AimsData<C> &data)
{ return(X(ZT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::XTX(const AimsData<C> &data)
{ return(X(TX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XTY(const AimsData<C> &data)
{ return(X(TY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XTZ(const AimsData<C> &data)
{ return(X(TZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::XTT(const AimsData<C> &data)
{ return(X(TT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::YXX(const AimsData<C> &data)
{ return(Y(XX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YXY(const AimsData<C> &data)
{ return(Y(XY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YXZ(const AimsData<C> &data)
{ return(Y(XZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YXT(const AimsData<C> &data)
{ return(Y(XT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::YYX(const AimsData<C> &data)
{ return(Y(YX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YYY(const AimsData<C> &data)
{ return(Y(YY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YYZ(const AimsData<C> &data)
{ return(Y(YZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YYT(const AimsData<C> &data)
{ return(Y(YT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::YZX(const AimsData<C> &data)
{ return(Y(ZX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YZY(const AimsData<C> &data)
{ return(Y(ZY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YZZ(const AimsData<C> &data)
{ return(Y(ZZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YZT(const AimsData<C> &data)
{ return(Y(ZT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::YTX(const AimsData<C> &data)
{ return(Y(TX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YTY(const AimsData<C> &data)
{ return(Y(TY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YTZ(const AimsData<C> &data)
{ return(Y(TZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::YTT(const AimsData<C> &data)
{ return(Y(TT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::ZXX(const AimsData<C> &data)
{ return(Z(XX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZXY(const AimsData<C> &data)
{ return(Z(XY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZXZ(const AimsData<C> &data)
{ return(Z(XZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZXT(const AimsData<C> &data)
{ return(Z(XT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::ZYX(const AimsData<C> &data)
{ return(Z(YX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZYY(const AimsData<C> &data)
{ return(Z(YY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZYZ(const AimsData<C> &data)
{ return(Z(YZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZYT(const AimsData<C> &data)
{ return(Z(YT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::ZZX(const AimsData<C> &data)
{ return(Z(ZX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZZY(const AimsData<C> &data)
{ return(Z(ZY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZZZ(const AimsData<C> &data)
{ return(Z(ZZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZZT(const AimsData<C> &data)
{ return(Z(ZT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::ZTX(const AimsData<C> &data)
{ return(Z(TX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZTY(const AimsData<C> &data)
{ return(Z(TY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZTZ(const AimsData<C> &data)
{ return(Z(TZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::ZTT(const AimsData<C> &data)
{ return(Z(TT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::TXX(const AimsData<C> &data)
{ return(T(XX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TXY(const AimsData<C> &data)
{ return(T(XY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TXZ(const AimsData<C> &data)
{ return(T(XZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TXT(const AimsData<C> &data)
{ return(T(XT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::TYX(const AimsData<C> &data)
{ return(T(YX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TYY(const AimsData<C> &data)
{ return(T(YY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TYZ(const AimsData<C> &data)
{ return(T(YZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TYT(const AimsData<C> &data)
{ return(T(YT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::TZX(const AimsData<C> &data)
{ return(T(ZX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TZY(const AimsData<C> &data)
{ return(T(ZY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TZZ(const AimsData<C> &data)
{ return(T(ZZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TZT(const AimsData<C> &data)
{ return(T(ZT(data)));
}


template <class C> inline
AimsData<C> AimsGradient<C>::TTX(const AimsData<C> &data)
{ return(T(TX(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TTY(const AimsData<C> &data)
{ return(T(TY(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TTZ(const AimsData<C> &data)
{ return(T(TZ(data)));
}

template <class C> inline
AimsData<C> AimsGradient<C>::TTT(const AimsData<C> &data)
{ return(T(TT(data)));
}

#endif
