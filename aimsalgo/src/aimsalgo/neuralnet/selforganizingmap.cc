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


#include <aims/neuralnet/selforganizingmap.h>
#include <aims/data/data.h>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <aims/vector/vector.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace std;


AimsSelfOrganizingMap::AimsSelfOrganizingMap(int size,int ninputs,
                                             float learningrate)
{ _size         = size;
  _nInputs      = ninputs;
  _learningRate = learningrate;
  _weight       = new AimsData<float>(size,size,ninputs);
  int x,y,z;
  ForEach3d( (*_weight) , x,y,z)
    (*_weight)(x,y,z) = UniformRandom(-0.1,0.1);
  _counter      = 0;
}


AimsSelfOrganizingMap::AimsSelfOrganizingMap(const AimsData<float> &weight,
                                             float learningrate)
{ ASSERT(weight.dimX() == weight.dimY());
  _size         = weight.dimX();
  _nInputs      = weight.dimZ();
  _learningRate = learningrate;
  _weight       = new AimsData<float>(weight);
  _counter      = 0;
}


AimsSelfOrganizingMap::~AimsSelfOrganizingMap()
{ delete(_weight);
}


int AimsSelfOrganizingMap::size() const
{ return(_size);
}


int AimsSelfOrganizingMap::nInputs() const
{ return(_nInputs);
}


int AimsSelfOrganizingMap::counter() const
{ return(_counter);
}


float AimsSelfOrganizingMap::learningRate() const
{ return(_learningRate);
}


void AimsSelfOrganizingMap::setLearningRate(float learningrate)
{ ASSERT(learningrate>(float)0);
  _learningRate = learningrate;
}


void AimsSelfOrganizingMap::learn(const AimsData<float> &base,
                                  int counter)
{ float x,learn;
  int q,delta;
  AimsData<float> input(_nInputs);

  _counter += counter;
  for (int i=0;i<counter;i++)
  { x = (float)i/(float)counter;
#if defined(__GNUC__) && defined (__sun__)
    delta = max((int)::ceil((double)_size*(double)(1.0-x)),1);
#else
    delta = max((int)ceil((double)_size*(double)(1.0-x)),1);
#endif
    learn = (1-x) * _learningRate;
    q     = (int)UniformRandom((double)0,(double)base.dimY()-1);
    for (int k=0;k<_nInputs;k++)
      input(k) = base(k,q);
    
    update(input,learn,delta);
  }
}


AimsVector<short,2> AimsSelfOrganizingMap::winner(const AimsData<float> &input)
{ float smallest=1e38,error;
  short XWinner=0,YWinner=0;

  for (int y=0;y<_weight->dimY();y++)
    for (int x=0;x<_weight->dimX();x++)
    { error = 0;
      for (int k=0;k<_nInputs;k++)
        error += square( input(k) - (*_weight)(x,y,k) );
      if (error<smallest)
      { XWinner = (short)x;
        YWinner = (short)y;
        smallest = error;
      }
    }
  return(AimsVector<short,2>(XWinner,YWinner));
}


void AimsSelfOrganizingMap::update(const AimsData<float> &input,
                                   float learn, int delta)
{ int               xmin,xmax,ymin,ymax;
  AimsVector<short,2> win(winner(input));

  xmin = (delta>=win.item(0))         ? 0 : win.item(0)-delta;
  xmax = (delta>=_size-1-win.item(0)) ? 0 : win.item(0)+delta;

  ymin = (delta>=win.item(1))         ? 0 : win.item(1)-delta;
  ymax = (delta>=_size-1-win.item(1)) ? 0 : win.item(1)+delta;

  for (int y=ymin;y<=ymax;y++)
    for (int x=xmin;x<=xmax;x++)
    { for (int k=0;k<_nInputs;k++)
        (*_weight)(x,y,k) += learn * ( input(k) - (*_weight)(x,y,k) );
    }
}


AimsData< AimsVector<short,2> >
AimsSelfOrganizingMap::test(const AimsData<float> &base)
{ AimsVector<short,2> win;

  ASSERT(base.dimX() == _nInputs);
  AimsData< AimsVector<short,2> > result(base.dimY());
  AimsData<float> input(base.dimX());

  for (int i=0;i<base.dimY();i++)
  { for (int k=0;k<_nInputs;k++)
      input(k) = base(k,i);
    result(i) = winner(input);
  }  
  return(result);
}


void AimsSelfOrganizingMap::save(const string &filename)
{
  Writer<AimsData<float> > writer(filename);
  writer << *_weight;
}
