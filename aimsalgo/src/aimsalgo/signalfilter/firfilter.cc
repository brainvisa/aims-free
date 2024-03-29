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


#include <aims/signalfilter/firfilter.h>
#include <cartodata/volume/volume.h>
#include <aims/math/mathelem.h>
#include <aims/math/bessel.h>

using namespace carto;


AimsFIRFilter::AimsFIRFilter(float fs,
                             float fc,
                             float deltaf,
                             float deltap,
                             float deltas,
                             int type)
{ ASSERT(fs>0); 
  _fSampling = fs;

  ASSERT(fc>0); 
  ASSERT(fc<fs); 
  _fCut = fc;

  ASSERT(deltaf>0); 
  _deltaF = deltaf;

  ASSERT(deltap>0); 
  _deltaP = deltap;

  ASSERT(deltas>0); 
  _deltaS = deltas;

  ASSERT(type==AIMS_FIR_LOWPASS || type==AIMS_FIR_HIGHPASS); 
  _type = type;

  initialize();
}


AimsFIRFilter::AimsFIRFilter(float fs,
                             float fl,
                             float fh,
                             float deltaf,
                             float deltap,
                             float deltas,
                             int type)
{ ASSERT(fs>0); 
  _fSampling = fs;

  ASSERT(fl<=0); 
  ASSERT(fl<fs); 
  _fLow = fl;

  ASSERT(fh>0); 
  ASSERT(fh<fs); 
  _fHigh = fh;

  ASSERT(fh>fl); 

  ASSERT(deltaf>0); 
  _deltaF = deltaf;

  ASSERT(deltap>0); 
  _deltaP = deltap;

  ASSERT(deltas>0); 
  _deltaS = deltas;

  ASSERT(type==AIMS_FIR_BANDPASS || type==AIMS_FIR_BANDSTOP); 
  _type = type;

  initialize();
}


AimsFIRFilter::~AimsFIRFilter()
{ delete(_impulseResponse);
}


void AimsFIRFilter::numberOfFIRFilterCoef()
{ float dp,ds,delta,A;
  
  dp    = (float)pow(10.0,((double)_deltaP/20.0)) - 1.0;
  ds    = (float)pow(10.0,(-(double)_deltaS/20.0));
  delta = (ds < dp)? ds : dp;

  A     = -20.0 * (float)log10((double)delta);
  if (A<=21)        _beta = 0.0;
  else if (A>=50)   _beta = (0.1102 * (A-8.7));
  else              _beta = (0.5842 * 
                            (float)pow((A-21.0),0.4) + 0.07886 * (A-21.0));

#if defined(__GNUC__) && defined (__sun__)
  _nCoef     = (int) ::ceil(((A-7.95)/(14.36*_deltaF/_fSampling)));
#else
  _nCoef     = (int) ceil(((A-7.95)/(14.36*_deltaF/_fSampling)));
#endif
  if (_nCoef % 2 == 0) _nCoef++;
}



VolumeRef<float> AimsFIRFilter::kaiserWindow()
{ float             num,NN,xx;
  VolumeRef<float> w( (_nCoef+1)/2, 1, 1, 1, AllocatorContext::fast() );

  NN = (float)_nCoef;  
  for (int x=0;x<(_nCoef+1)/2;x++)
  { xx   = (float)x;
    num  = _beta * sqrt( 1.0 - square((2.0*xx) / (NN-1.0)) );
    w(x) = (float)(AimsBessel0((double)num)/
                   AimsBessel0((double)_beta));
  }
  return w;
}


VolumeRef<float> AimsFIRFilter::idealImpulseResponse()
{ VolumeRef<float> hD( (_nCoef+1)/2, 1, 1, 1, AllocatorContext::fast() );
  float             f,f1,f2,W,W1,W2,xx;
  int		    x;

  switch (_type)
  { case AIMS_FIR_LOWPASS  : 
      f     = ( (_fCut + _deltaF / 2) / _fSampling);
      W     = 2 * M_PI * f;
      hD(0) = 2.0 * f;
      for (x=1;x<(_nCoef+1)/2;x++)
      { xx    = x;
        hD(x) = 2.0 * f * sin(xx*W) / (xx*W);
      }
      break;
    case AIMS_FIR_HIGHPASS : 
      f     = ( (_fCut - _deltaF / 2) / _fSampling);
      W     = 2 * M_PI * f;
      hD(0) = 1.0 - 2.0 * f;
      for (x=1;x<(_nCoef+1)/2;x++)
      { xx    = x;
        hD(x) = - 2.0 * f * sin(xx*W) / (xx*W);
      }
      break;
    case AIMS_FIR_BANDPASS : 
      f1    = ( (_fLow  - _deltaF / 2) / _fSampling);
      f2    = ( (_fHigh + _deltaF / 2) / _fSampling);
      W1    = 2 * M_PI * f1;
      W2    = 2 * M_PI * f2;
      hD(0) = 2.0 * (f2 - f1);
      for (x=1;x<(_nCoef+1)/2;x++)
      { xx    = x;
        hD(x) = 2.0 * f2 * sin(xx*W2) / (xx*W2) -
                2.0 * f1 * sin(xx*W1) / (xx*W1);
      }
      break;
    case AIMS_FIR_BANDSTOP :
      f1    = ( (_fLow  + _deltaF / 2) / _fSampling);
      f2    = ( (_fHigh - _deltaF / 2) / _fSampling);
      W1    = 2 * M_PI * f1;
      W2    = 2 * M_PI * f2;
      hD(0) = 1.0 - 2.0 * (f2 - f1);
      for (x=1;x<(_nCoef+1)/2;x++)
      { xx    = x;
        hD(x) = 2.0 * f1 * sin(xx*W1) / (xx*W1) -
                2.0 * f2 * sin(xx*W2) / (xx*W2);
      }
      break;
  }
  return hD;
}


VolumeRef<float> AimsFIRFilter::impulseResponse()
{ VolumeRef<float> res((_nCoef+1)/2),
                  kw = kaiserWindow(),
                  iir = idealImpulseResponse();
  for (int x=0;x<res.getSizeX();x++)
    res(x) = kw(x) * iir(x);
  return res;
}


void AimsFIRFilter::initialize()
{ numberOfFIRFilterCoef();
  _impulseResponse = new VolumeRef<float>(impulseResponse());
}


float AimsFIRFilter::fSampling() const
{ return _fSampling;
}


float AimsFIRFilter::fCut() const
{ return _fCut;
}


float AimsFIRFilter::fLow() const
{ return _fLow;
}


float AimsFIRFilter::fHigh() const
{ return _fHigh;
}


float AimsFIRFilter::deltaF() const
{ return _deltaF;
}


float AimsFIRFilter::deltaP() const
{ return _deltaP;
}


float AimsFIRFilter::deltaS() const
{ return _deltaS;
}


int AimsFIRFilter::type() const
{ return _type;
}


VolumeRef<float>
AimsFIRFilter::process( const rc_ptr<Volume<float> > &sqv,
                        AimsDirectionAxis dir )
{ int hsize;
  float tmp;
  VolumeRef<float> res(sqv->getSize(), sqv->getBorders());
  res->copyHeaderFrom( sqv->header() );
  res = 0.0;

  int	x, y, z, t, k;

  hsize = _impulseResponse->getSizeX();
  switch (dir)
  { case AIMS_X_DIRECTION : 
      for (t=0;t<res.getSizeT();t++)
        for (z=0;z<res.getSizeZ();z++)
          for (y=0;y<res.getSizeY();y++)
            for (x=(hsize-1)/2 ; x<sqv->getSizeX()+(hsize-1)/2 ; x++)
            { tmp = 0.0;
              for (k=0;k<hsize;k++)
                if (((x-k)>=0) && ((x-k)<sqv->getSizeX()))
                  tmp += (*_impulseResponse)(k) * sqv->at(x-k,y,z,t);
              res(x-(hsize-1)/2,y,z,t) = tmp;
            }
      break;
    case AIMS_Y_DIRECTION : 
      for (t=0;t<res.getSizeT();t++)
        for (z=0;z<res.getSizeZ();z++)
          for (x=0;x<res.getSizeX();x++)
            for (y=(hsize-1)/2 ; y<sqv->getSizeY()+(hsize-1)/2 ; y++)
            { tmp = 0.0;
              for (k=0;k<hsize;k++)
                if (((y-k)>=0) && ((y-k)<sqv->getSizeY()))
                  tmp += (*_impulseResponse)(k) * sqv->at(x,y-k,z,t);
              res(x,y-(hsize-1)/2,z,t) = tmp;
            }
      break;
    case AIMS_Z_DIRECTION : 
      for (t=0;t<res.getSizeT();t++)
        for (y=0;y<res.getSizeY();y++)
          for (x=0;x<res.getSizeX();x++)
            for (z=(hsize-1)/2 ; z<sqv->getSizeZ()+(hsize-1)/2 ; z++)
            { tmp = 0.0;
              for (k=0;k<hsize;k++)
                if (((z-k)>=0) && ((z-k)<sqv->getSizeZ()))
                  tmp += (*_impulseResponse)(k) * sqv->at(x,y,z-k,t);
              res(x,y,z-(hsize-1)/2,t) = tmp;
            }
      break;
    case AIMS_T_DIRECTION : 
      // ### et ce cas ?
      break;
  }
  return res;
}
