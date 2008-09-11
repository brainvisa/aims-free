/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_MATH_GAUSSJ_H
#define AIMS_MATH_GAUSSJ_H

#include <aims/def/general.h>

template <class T> class AimsData;


/** @name Gauss-Jordan elimination */
template < class T >
class GaussJordan
{
public:

  /** Constructor and destructor */
  //@{
  /// constructor
  GaussJordan()  { }
  /// destructor
  virtual ~GaussJordan() { }
  //@}

  /** Gauss-Jordan elimination procedure. \\
      This function is adapted from the Numerical Recipes in C. \\
      It returns a matrix corresponding to the set of solution vectors of a 
      linear equation solution by Gauss-Jordan elimination of the input 
      matrix. \\
      The first parameter is the input matrix. On ouput, it is replaced by
      its matrix inverse. \\
      The second input is a matrix containing the right-hand side
      vectors. On output, this matrix is replaced by the 
      corresponding set of solution vectors (those that are also returned
      by the function). \\
      @param mUtil allows to perform the elimination only on a subset of
      the input matrix where mUtils represents the number of columns of the
      input matrix used for the computation.
  */
  bool doit( AimsData< T >&, AimsData< T >&, int mUtil=-1 );
};

#endif
