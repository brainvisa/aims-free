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


#include <cartodata/volume/volume.h>

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
  carto::VolumeRef<C> X(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d(data)/dy = AimsGradientY(data)
  carto::VolumeRef<C> Y(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d(data)/dz = AimsGradientZ(data)
  carto::VolumeRef<C> Z(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d(data)/dt = AimsGradientT(data)
  carto::VolumeRef<C> T(const carto::rc_ptr<carto::Volume<C> > &data);
  //@}

  /**@name Second order*/
  //@{
  /// d2(data)/dx2 = XX(data)
  carto::VolumeRef<C> XX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dxdy = XY(data)
  carto::VolumeRef<C> XY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dxdz = XZ(data)
  carto::VolumeRef<C> XZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dxdt = XT(data)
  carto::VolumeRef<C> XT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dydx = YX(data)
  carto::VolumeRef<C> YX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dy2 = YY(data)
  carto::VolumeRef<C> YY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dydz = YZ(data)
  carto::VolumeRef<C> YZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dydt = YT(data)
  carto::VolumeRef<C> YT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dzdx = ZX(data)
  carto::VolumeRef<C> ZX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dzdy = ZY(data)
  carto::VolumeRef<C> ZY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dz2 = ZZ(data)
  carto::VolumeRef<C> ZZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dzdt = ZT(data)
  carto::VolumeRef<C> ZT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dtdx = TX(data)
  carto::VolumeRef<C> TX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dtdy = TY(data)
  carto::VolumeRef<C> TY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dtdz = TZ(data)
  carto::VolumeRef<C> TZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d2(data)/dt2 = TT(data)
  carto::VolumeRef<C> TT(const carto::rc_ptr<carto::Volume<C> > &data);
  //@}
    
  /**@name Third order*/
  //@{
  /// d3(data)/dx3 = XXX(data)
  carto::VolumeRef<C> XXX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dx2dy = XXY(data)
  carto::VolumeRef<C> XXY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dx2dz = XXZ(data)
  carto::VolumeRef<C> XXZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dx2dt = XXT(data)
  carto::VolumeRef<C> XXT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdydx = XYX(data)
  carto::VolumeRef<C> XYX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdy2 = XYY(data)
  carto::VolumeRef<C> XYY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdydz = XYZ(data)
  carto::VolumeRef<C> XYZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdydt = XYT(data)
  carto::VolumeRef<C> XYT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdzdx = XZX(data)
  carto::VolumeRef<C> XZX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdzdy = XZY(data)
  carto::VolumeRef<C> XZY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdz2 = XZZ(data)
  carto::VolumeRef<C> XZZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdzdt = XZT(data)
  carto::VolumeRef<C> XZT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdtdx = XTX(data)
  carto::VolumeRef<C> XTX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdtdy = XTY(data)
  carto::VolumeRef<C> XTY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdtdz = XTZ(data)
  carto::VolumeRef<C> XTZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dxdt2 = XTT(data)
  carto::VolumeRef<C> XTT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydx2 = YXX(data)
  carto::VolumeRef<C> YXX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydxdy = YXY(data)
  carto::VolumeRef<C> YXY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydxdz = YXZ(data)
  carto::VolumeRef<C> YXZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydxdt = YXT(data)
  carto::VolumeRef<C> YXT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dy2dx = YYX(data)
  carto::VolumeRef<C> YYX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dy3 = YYY(data)
  carto::VolumeRef<C> YYY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dy2dz = YYZ(data)
  carto::VolumeRef<C> YYZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dy2dt = YYT(data)
  carto::VolumeRef<C> YYT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydzdx = YZX(data)
  carto::VolumeRef<C> YZX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydzdy = YZY(data)
  carto::VolumeRef<C> YZY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydz2 = YZZ(data)
  carto::VolumeRef<C> YZZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydzdt = YZT(data)
  carto::VolumeRef<C> YZT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydtdx = YTX(data)
  carto::VolumeRef<C> YTX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydtdy = YTY(data)
  carto::VolumeRef<C> YTY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydtdz = YTZ(data)
  carto::VolumeRef<C> YTZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dydt2 = YTT(data)
  carto::VolumeRef<C> YTT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdx2 = ZXX(data)
  carto::VolumeRef<C> ZXX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdxdy = ZXY(data)
  carto::VolumeRef<C> ZXY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdxdz = ZXZ(data)
  carto::VolumeRef<C> ZXZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdxdt = ZXT(data)
  carto::VolumeRef<C> ZXT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdydx = ZYX(data)
  carto::VolumeRef<C> ZYX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdy2 = ZYY(data)
  carto::VolumeRef<C> ZYY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdydz = ZYZ(data)
  carto::VolumeRef<C> ZYZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdydt = ZYT(data)
  carto::VolumeRef<C> ZYT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dz2dx = ZZX(data)
  carto::VolumeRef<C> ZZX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dz2dy = ZZY(data)
  carto::VolumeRef<C> ZZY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dz3 = ZZZ(data)
  carto::VolumeRef<C> ZZZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dz2dt = ZZT(data)
  carto::VolumeRef<C> ZZT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdtdx = ZTX(data)
  carto::VolumeRef<C> ZTX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdtdy = ZTY(data)
  carto::VolumeRef<C> ZTY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdtdz = ZTZ(data)
  carto::VolumeRef<C> ZTZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dzdt2 = ZTT(data)
  carto::VolumeRef<C> ZTT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdx2 = TXX(data)
  carto::VolumeRef<C> TXX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdxdy = TXY(data)
  carto::VolumeRef<C> TXY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdxdz = TXZ(data)
  carto::VolumeRef<C> TXZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdxdt = TXT(data)
  carto::VolumeRef<C> TXT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdydx = TYX(data)
  carto::VolumeRef<C> TYX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdy2 = TYY(data)
  carto::VolumeRef<C> TYY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdydz = TYZ(data)
  carto::VolumeRef<C> TYZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdydt = TYT(data)
  carto::VolumeRef<C> TYT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdzdx = TZX(data)
  carto::VolumeRef<C> TZX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdzdy = TZY(data)
  carto::VolumeRef<C> TZY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdz2 = TZZ(data)
  carto::VolumeRef<C> TZZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dtdzdt = TZT(data)
  carto::VolumeRef<C> TZT(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dt2dx = TTX(data)
  carto::VolumeRef<C> TTX(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dt2dy = TTY(data)
  carto::VolumeRef<C> TTY(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dt2dz = TTZ(data)
  carto::VolumeRef<C> TTZ(const carto::rc_ptr<carto::Volume<C> > &data);
  /// d3(data)/dt3 = TTT(data)
  carto::VolumeRef<C> TTT(const carto::rc_ptr<carto::Volume<C> > &data);
  //@}
};




/* gradient 1st order */

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::X(
  const carto::rc_ptr<carto::Volume<C> > &data)
{
  int    beginx=0,endx=0,offx1=0,offx2=0;
  float  divx=0;
  std::vector<int> dims = data->getSize();
  std::vector<float> vs = data->getVoxelSize();

  carto::VolumeRef<C> grad( dims, data->getBorders() );
  grad = (C)0;
  grad.fillBorder(0);
  grad.copyHeaderFrom( data->header() );

  switch (_type)
  {
  case AIMS_GRADIENT_CENTRAL :
    beginx = 1;
    endx = dims[0]-1;
    offx1 = 1;
    offx2 = 1;
    divx = 2 * vs[0];
    break;
  case AIMS_GRADIENT_DMINUS  :
    beginx = 1;
    endx = dims[0];
    offx1 = 1;
    offx2 = 0;
    divx = vs[0];
    break;
  case AIMS_GRADIENT_DPLUS   :
    beginx = 0;
    endx = dims[0]-1;
    offx1 = 0;
    offx2 = 1;
    divx = vs[0];
    break;
  }

  for (int t=0;t<dims[3];t++)
    for (int z=0;z<dims[2];z++)
      for (int y=0;y<dims[1];y++)
        for (int x=beginx;x<endx;x++)
          grad(x,y,z,t) = (C)( ((float)data->at(x+offx2,y,z,t) -
                                (float)data->at(x-offx1,y,z,t)) / divx);
  return grad;
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::Y(
  const carto::rc_ptr<carto::Volume<C> > &data)
{
  int    beginy=0,endy=0,offy1=0,offy2=0;
  float  divy=0;
  std::vector<int> dims = data->getSize();
  std::vector<float> vs = data->getVoxelSize();

  carto::VolumeRef<C> grad( dims, data->getBorders() );
  grad = (C)0;
  grad.fillBorder(0);
  grad.copyHeaderFrom( data->header() );

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : beginy = 1;endy = dims[1]-1;
                                 offy1=1;offy2=1;divy=2 * vs[1];break;
    case AIMS_GRADIENT_DMINUS  : beginy = 1;endy = dims[1]  ;
                                 offy1=1;offy2=0;divy=vs[1];break;
    case AIMS_GRADIENT_DPLUS   : beginy = 0;endy = dims[1]-1;
                                 offy1=0;offy2=1;divy=vs[1];break;
  }

  for (int t=0;t<dims[3];t++)
    for (int z=0;z<dims[2];z++)
      for (int y=beginy;y<endy;y++)
        for (int x=0;x<dims[0];x++)
          grad(x,y,z,t) = (C)(((float)data->at(x,y+offy2,z,t) -
                               (float)data->at(x,y-offy1,z,t)) / divy);
  return grad;
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::Z(
  const carto::rc_ptr<carto::Volume<C> > &data)
{
  int    beginz=0,endz=0,offz1=0,offz2=0;
  float  divz=0;
  std::vector<int> dims = data->getSize();
  std::vector<float> vs = data->getVoxelSize();

  carto::VolumeRef<C> grad( dims, data->getBorders() );
  grad = (C)0;
  grad.fillBorder(0);
  grad.copyHeaderFrom( data->header() );

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : beginz = 1;endz = dims[2]-1;
                                 offz1=1;offz2=1;divz=2 * vs[2];break;
    case AIMS_GRADIENT_DMINUS  : beginz = 1;endz = dims[2]  ;
                                 offz1=1;offz2=0;divz=vs[2];break;
    case AIMS_GRADIENT_DPLUS   : beginz = 0;endz = dims[2]-1;
                                 offz1=0;offz2=1;divz=vs[2];break;
  }

  for (int t=0;t<dims[3];t++)
    for (int z=beginz;z<endz;z++)
      for (int y=0;y<dims[1];y++)
        for (int x=0;x<dims[0];x++)
          grad(x,y,z,t) = (C)(((float)data->at(x,y,z+offz2,t) -
                               (float)data->at(x,y,z-offz1,t)) / divz);
  return grad;
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::T(
  const carto::rc_ptr<carto::Volume<C> > &data)
{
  int    begint=0,endt=0,offt1=0,offt2=0;
  float  divt=0;
  std::vector<int> dims = data->getSize();
  std::vector<float> vs = data->getVoxelSize();

  carto::VolumeRef<C> grad( dims, data->getBorders() );
  grad = (C)0;
  grad.fillBorder(0);
  grad.copyHeaderFrom( data->header() );

  switch (_type)
  { case AIMS_GRADIENT_CENTRAL : begint = 1;endt = dims[3]-1;
                                 offt1=1;offt2=1;divt=2 * vs[3];break;
    case AIMS_GRADIENT_DMINUS  : begint = 1;endt = dims[3]  ;
                                 offt1=1;offt2=0;divt=vs[3];break;
    case AIMS_GRADIENT_DPLUS   : begint = 0;endt = dims[3]-1;
                                 offt1=0;offt2=1;divt=vs[3];break;
  }

  for (int t=begint;t<endt;t++)
    for (int z=0;z<dims[2];z++)
      for (int y=0;y<dims[1];y++)
        for (int x=0;x<dims[0];x++)
          grad(x,y,z,t) = (C)(((float)data->at(x,y,z,t+offt2) -
                               (float)data->at(x,y,z,t-offt1)) / divt);
  return grad;
}


/* gradient 2nd order */

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(X(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(Y(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(Z(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(T(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(X(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(Y(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(Z(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(T(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(X(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(Y(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(Z(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(T(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(X(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(Y(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(Z(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(T(data)));
}


/* gradient 3rd order */

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XXX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(XX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XXY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(XY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XXZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(XZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XXT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(XT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XYX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(YX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XYY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(YY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XYZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(YZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XYT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(YT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XZX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(ZX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XZY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(ZY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XZZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(ZZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XZT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(ZT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XTX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(TX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XTY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(TY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XTZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(TZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::XTT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(X(TT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YXX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(XX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YXY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(XY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YXZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(XZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YXT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(XT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YYX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(YX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YYY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(YY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YYZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(YZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YYT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(YT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YZX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(ZX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YZY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(ZY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YZZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(ZZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YZT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(ZT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YTX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(TX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YTY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(TY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YTZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(TZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::YTT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Y(TT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZXX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(XX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZXY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(XY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZXZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(XZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZXT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(XT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZYX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(YX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZYY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(YY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZYZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(YZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZYT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(YT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZZX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(ZX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZZY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(ZY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZZZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(ZZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZZT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(ZT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZTX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(TX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZTY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(TY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZTZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(TZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::ZTT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(Z(TT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TXX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(XX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TXY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(XY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TXZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(XZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TXT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(XT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TYX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(YX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TYY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(YY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TYZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(YZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TYT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(YT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TZX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(ZX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TZY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(ZY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TZZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(ZZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TZT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(ZT(data)));
}


template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TTX(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(TX(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TTY(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(TY(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TTZ(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(TZ(data)));
}

template <class C> inline
carto::VolumeRef<C> AimsGradient<C>::TTT(const carto::rc_ptr<carto::Volume<C> > &data)
{ return(T(TT(data)));
}

#endif
