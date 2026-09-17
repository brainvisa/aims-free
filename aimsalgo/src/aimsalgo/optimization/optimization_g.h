
#ifndef AIMS_OPTIMIZATION_OPTIMIZATION_G_H
#define AIMS_OPTIMIZATION_OPTIMIZATION_G_H

/**@name optimization package
The optimization package provides optimization algorithm such as Powell, 
Gradient, Simulated Annealing, ICM.
*/
//@{ 
    //@Include: optimization/objective.h
    //@Include: optimization/optimizer.h
    //@Include: optimization/singleoptimizer.h
    //@Include: optimization/powell.h
    //@Include: optimization/deterministic.h
    /**@name Levenberg-Marquardt*/
    //@{
          //@Include: optimization/covsrt.h
          //@Include: optimization/lmfunc.h
          //@Include: optimization/lmgamma.h
          //@Include: optimization/lmgauss.h
          //@Include: optimization/lm2gauss.h
          //@Include: optimization/levmrq.h
    //@}
//@}


#include <aims/optimization/objective.h>
#include <aims/optimization/optimizer.h>
#include <aims/optimization/singleoptimizer.h>
#include <aims/optimization/powell.h>
#include <aims/optimization/deterministic.h>
#include <aims/optimization/covsrt.h>
#include <aims/optimization/lmfunc.h>
#include <aims/optimization/lmgamma.h>
#include <aims/optimization/lmgauss.h>
#include <aims/optimization/lm2gauss.h>
#include <aims/optimization/levmrq.h>

#endif
