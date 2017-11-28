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


#ifndef AIMS_MATH_MATH_G_H
#define AIMS_MATH_MATH_G_H


/**@name	math package
The math package provides standard mathematical algorithms.
The algorithms are mainly issued from the Numerical Recipee in C.
*/
//@{ 
   //@Include: math/bessel.h
   //@Include: math/mathelem.h
   //@Include: math/geometry.h
   /**@name Curvature*/
   //@{
	  //@Include: math/curv2Diso.h
	  //@Include: math/curv3Diso.h
   //@}
   /**@name Linear algebra*/
   //@{
	  //@Include: math/eigen.h
	  //@Include: math/householder.h
	  //@Include: math/tqli.h
	  //@Include: math/balanc.h
	  //@Include: math/balbak.h
	  //@Include: math/elmhes.h
	  //@Include: math/eltran.h
	  //@Include: math/hqr.h
	  //@Include: math/svd.h
          //@Include: math/gaussj.h
	  //@Include: math/gausslu.h
   //@}
   /**@name Gradient estimation*/
   //@{
	  //@Include: math/gradient.h
   //@}
   /**@name Interpolation*/
   //@{
	  //@Include: math/bspline3.h
   //@}
   /**@name Integration*/
   //@}
   /**@name Fit*/
   //@{
	  //@Include: math/multilinfit.h
   //@}
   /**@name Random generators*/
   //@{
	  //@Include: math/random.h
   //@}
   //@Include: math/distspheric.h
   /**@name Tensor*/
   //@{
          //@Include: trieder.h
          //@Include: tensor.h
          //@Include: dtitensor.h
          //@Include: tensorMatrix.h
   //@}
//@}


// misc
#include <aims/math/bessel.h>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <aims/math/geometry.h>
#include <aims/math/trieder.h>
#include <aims/math/tensor.h>
#include <aims/math/dtitensor.h>
#include <aims/math/tensorMatrix.h>

// curvature
#include <aims/math/curv2Diso.h>
#include <aims/math/curv3Diso.h>

// derivation
#include <aims/math/gradient.h>

// interpolation
//#include <aims/math/lagrange.h>
#include <aims/math/bspline3.h>

// integration
#include <aims/math/romberg.h>
#include <aims/math/integrator.h>
#include <aims/math/trapezeitg.h>
#include <aims/math/rombergitg.h>


// linear algebra
#include <aims/math/eigen.h>
#include <aims/math/householder.h>
#include <aims/math/tqli.h>
#include <aims/math/balanc.h>
#include <aims/math/balbak.h>
#include <aims/math/elmhes.h>
#include <aims/math/eltran.h>
#include <aims/math/hqr.h>
#include <aims/math/svd.h>
#include <aims/math/gaussj.h>
#include <aims/math/gausslu.h>

// fit
#include <aims/math/multilinfit.h>

// distributions
#include <aims/math/distspheric.h>


#endif


