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


#include <aims/math/tensorMatrix.h>
#include <aims/math/mathelem.h>
#include <aims/data/fastAllocationData.h>
#include <aims/vector/vector.h>
#include <aims/math/trieder.h>

using namespace aims;

AimsData<float> 
AimsEigenToMatrix(const Trieder& trieder,
                  float lambda1,float lambda2,float lambda3)
{
  AimsFastAllocationData<float> tensor(3,3);

  tensor(0,0) = lambda1 * square(trieder.dirX().item(0)) +
                lambda2 * square(trieder.dirY().item(0)) +
                lambda3 * square(trieder.dirZ().item(0)) ;
  tensor(1,1) = lambda1 * square(trieder.dirX().item(1)) +
                lambda2 * square(trieder.dirY().item(1)) +
                lambda3 * square(trieder.dirZ().item(1)) ;
  tensor(2,2) = lambda1 * square(trieder.dirX().item(2)) +
                lambda2 * square(trieder.dirY().item(2)) +
                lambda3 * square(trieder.dirZ().item(2)) ;
  tensor(0,1) = lambda1 * trieder.dirX().item(0) * trieder.dirX().item(1) +
                lambda2 * trieder.dirY().item(0) * trieder.dirY().item(1) +
                lambda3 * trieder.dirZ().item(0) * trieder.dirZ().item(1) ;
  tensor(0,2) = lambda1 * trieder.dirX().item(0) * trieder.dirX().item(2) +
                lambda2 * trieder.dirY().item(0) * trieder.dirY().item(2) +
                lambda3 * trieder.dirZ().item(0) * trieder.dirZ().item(2) ;
  tensor(1,2) = lambda1 * trieder.dirX().item(1) * trieder.dirX().item(2) +
                lambda2 * trieder.dirY().item(1) * trieder.dirY().item(2) +
                lambda3 * trieder.dirZ().item(1) * trieder.dirZ().item(2) ;

  tensor(1,0) = tensor(0,1);
  tensor(2,0) = tensor(0,2);
  tensor(2,1) = tensor(1,2);

  return(tensor);
}
