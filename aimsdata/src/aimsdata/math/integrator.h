/*
 *  Numerical integration.
 */
#ifndef AIMS_MATH_INTEGRATOR_H
#define AIMS_MATH_INTEGRATOR_H


namespace aims {

template <class REAL>
class IntegrableOf
{
 public:
  
  inline IntegrableOf() { }
  inline virtual ~IntegrableOf() { }
  
  virtual REAL valueAt( REAL ) const = 0;
};


template <class REAL>
class IntegratorOf
{
 public:
  
  inline IntegratorOf() {}
  inline virtual ~IntegratorOf() { }
  
  virtual REAL eval( const IntegrableOf<REAL> &, REAL, REAL ) const;
};


typedef IntegrableOf<float> Integrable;
typedef IntegratorOf<float> Integrator;

} // namespace aims

#endif
