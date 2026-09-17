
#ifndef AIMS_OPTIMIZATION_LMFUNC_H
#define AIMS_OPTIMIZATION_LMFUNC_H

#include <vector>


template< class T >
class LMFunction
{
public:

  LMFunction()  { }
  LMFunction( const LMFunction< T >& );
  virtual ~LMFunction();

  std::vector< T >& param() { return par; }
  std::vector< T >& derivative() { return der; }

  virtual T apply( T ) { return (T)0; };
  virtual T eval( T ) { return (T)0; };

protected:

  std::vector< T > par;
  std::vector< T > der;
};


template< class T > inline
LMFunction< T >::LMFunction( const LMFunction< T > &func )
  : par( func.par ), der( func.der )
{
}


template< class T > inline
LMFunction< T >::~LMFunction()
{
  par.clear();
  der.clear();
}

#endif
