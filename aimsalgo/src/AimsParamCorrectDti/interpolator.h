
#ifndef _AIMSPARAMCORRECTDTI_INTERPOLATION_H_
#define _AIMSPARAMCORRECTDTI_INTERPOLATION_H_


template <class T> class AimsData;
template <class T,int D> class AimsVector;


template <int D>
class Interpolator
{
  public:

    Interpolator() { }
    virtual ~Interpolator() { }

    virtual void doit( const AimsData<short>&, AimsData<short>&,
                       const AimsVector<float,D>& ) const { }
};




class SplineInterpolator : public Interpolator<3>
{
  public:

    SplineInterpolator() : Interpolator<3>() { }
    ~SplineInterpolator() { }

    void doit( const AimsData<short>& orig, AimsData<short>& dest,
               const AimsVector<float,3>& param ) const;
};


#endif
