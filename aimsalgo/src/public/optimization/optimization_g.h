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
