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


#include "mutualInfo.h"
#include <math.h>

float mutualInformation( const float* p1,
                         const float* p2,
                         float** p12,
                         int levels )
{

  float v1 = 0.0f;
  float v2 = 0.0f;
  float v12 = 0.0f;
  float mi=0.0;
  int l1, l2;
  for ( l2 = 0; l2 < levels; l2++ )
    for ( l1 = 0; l1 < levels; l1++ )
    {

      v1 = p1[ l1 ];
      v2 = p2[ l2 ];
      v12 = p12[ l1 ][ l2 ];
      if ( v2 && v12 && v1 )
        mi += v12 * log( v12 / ( v1 * v2 ) );

    }

  return mi;

}
