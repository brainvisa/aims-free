

#ifndef AIMS_AIMST2BASEDCORRECTIONEPI_EPIDISTORSION_H
#define AIMS_AIMST2BASEDCORRECTIONEPI_EPIDISTORSION_H


#include <aims/vector/vector.h>
#include <aims/optimization/objective.h>


class Smoother;

//

class EpiDistorsion : public ObjectiveFunc< float, 3 >
{

  public:

    EpiDistorsion( short* reference, short* distorded, short* output,
                   int size, float resolution,
                   float** p12, float* p1, float* p2, int levels,
                   Smoother* smoother );
    ~EpiDistorsion();

    float eval( const AimsVector< float, 3 >& ) const;

  protected:

    short* _reference;
    short* _distorded;
    short* _output;
    int _size;
    float _resolution;
    float** _p12;
    float* _p1;
    float* _p2;
    int _levels;
    Smoother* _smoother;

};



#endif
