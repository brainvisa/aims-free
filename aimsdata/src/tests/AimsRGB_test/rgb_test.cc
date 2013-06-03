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

#include <aims/rgb/rgb2.h>
#include <soma-io/image/voxelvalue_d.h>
#include <cartobase/exception/assert.h>
#include <iostream>

using namespace soma;
using namespace std;

int main()
{
  cout << "=====================================================================" << endl;
  cout << "                   A I M S R G B   T E S T                           " << endl;
  cout << "=====================================================================" << endl;
  
  cout << "---|  TESTING sizes  |-----------------------------------------------" << endl;
  cout << "sizeof( AimsRGB ) = " << sizeof( AimsRGB ) << endl;
  ASSERT( sizeof( AimsRGB ) == 3 );
  cout << "sizeof( VoxelValue<uint8_t,3> ) = " << sizeof( VoxelValue< uint8_t,3 > ) << endl;
  ASSERT( sizeof( VoxelValue< uint8_t,3 > ) == 3 );
  
  cout << "---|  TESTING creator()  |-------------------------------------------" << endl;
  cout << ">> AimsRGB voxel;" << endl;
  AimsRGB voxel;
  cout << "voxel = ( " 
       << (int) voxel.red() << ", "
       << (int) voxel.green() << ", "
       << (int) voxel.blue() << " )"
       << endl;
  ASSERT( voxel.red() == 0 && voxel.green() == 0 && voxel.blue() == 0 );
  
  cout << "---|  TESTING creator( r,g,b )  |------------------------------------" << endl;
  cout << ">> AimsRGB voxelrgb( 5, 10, 15 );" << endl;
  AimsRGB voxelrgb( 5, 10, 15 );
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING creator( other )  |------------------------------------" << endl;
  cout << ">> AimsRGB voxelcopy( voxelrgb );" << endl;
  AimsRGB voxelcopy( voxelrgb );
  cout << "voxelcopy = ( " 
       << (int) voxelcopy.red() << ", "
       << (int) voxelcopy.green() << ", "
       << (int) voxelcopy.blue() << " )"
       << endl;
  ASSERT( voxelcopy.red() == 5 && voxelcopy.green() == 10 && voxelcopy.blue() == 15 );
  
  cout << "---|  TESTING operator =  |------------------------------------------" << endl;
  cout << ">> voxelcopy = voxelrgb;" << endl;
  voxelcopy = voxel;
  cout << "voxelcopy = ( " 
       << (int) voxelcopy.red() << ", "
       << (int) voxelcopy.green() << ", "
       << (int) voxelcopy.blue() << " )"
       << endl;
  ASSERT( voxelcopy.red() == 0 && voxelcopy.green() == 0 && voxelcopy.blue() == 0 );
  
  cout << "---|  TESTING operator +=  |-----------------------------------------" << endl;
  cout << ">> voxelrgb += AimsRGB( 1, 2, 3 );" << endl;
  voxelrgb += AimsRGB( 1, 2, 3 );
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 6 && voxelrgb.green() == 12 && voxelrgb.blue() == 18 );
  
  cout << "---|  TESTING operator -=  |-----------------------------------------" << endl;
  cout << ">> voxelrgb -= AimsRGB( 1, 2, 3 );" << endl;
  voxelrgb -= AimsRGB( 1, 2, 3 );
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator = const  |------------------------------------" << endl;
  cout << ">> voxelcopy = 3;" << endl;
  voxelcopy = 3;
  cout << "voxelcopy = ( " 
       << (int) voxelcopy.red() << ", "
       << (int) voxelcopy.green() << ", "
       << (int) voxelcopy.blue() << " )"
       << endl;
  ASSERT( voxelcopy.red() == 3 && voxelcopy.green() == 3 && voxelcopy.blue() == 3 );
  
  cout << "---|  TESTING operator += const  |-----------------------------------" << endl;
  cout << ">> voxelrgb += 1;" << endl;
  voxelrgb += 1;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 6 && voxelrgb.green() == 11 && voxelrgb.blue() == 16 );
  
  cout << "---|  TESTING operator -= const  |-----------------------------------" << endl;
  cout << ">> voxelrgb -= 1;" << endl;
  voxelrgb -= 1;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator *= uint8_t  |---------------------------------" << endl;
  cout << ">> voxelrgb *= (uint8_t)2;" << endl;
  voxelrgb *= (uint8_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 10 && voxelrgb.green() == 20 && voxelrgb.blue() == 30 );
  
  cout << "---|  TESTING operator /= uint8_t  |---------------------------------" << endl;
  cout << ">> voxelrgb /= (uint8_t)2;" << endl;
  voxelrgb /= (uint8_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator *= uint16_t  |--------------------------------" << endl;
  cout << ">> voxelrgb *= (uint16_t)2;" << endl;
  voxelrgb *= (uint16_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 10 && voxelrgb.green() == 20 && voxelrgb.blue() == 30 );
  
  cout << "---|  TESTING operator /= uint16_t  |--------------------------------" << endl;
  cout << ">> voxelrgb /= (uint16_t)2;" << endl;
  voxelrgb /= (uint16_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator *= uint32_t  |--------------------------------" << endl;
  cout << ">> voxelrgb *= (uint32_t)2;" << endl;
  voxelrgb *= (uint32_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 10 && voxelrgb.green() == 20 && voxelrgb.blue() == 30 );
  
  cout << "---|  TESTING operator /= uint32_t  |--------------------------------" << endl;
  cout << ">> voxelrgb /= (uint32_t)2;" << endl;
  voxelrgb /= (uint32_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator *= uint64_t  |--------------------------------" << endl;
  cout << ">> voxelrgb *= (uint64_t)2;" << endl;
  voxelrgb *= (uint64_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 10 && voxelrgb.green() == 20 && voxelrgb.blue() == 30 );
  
  cout << "---|  TESTING operator /= uint64_t  |--------------------------------" << endl;
  cout << ">> voxelrgb /= (uint64_t)2;" << endl;
  voxelrgb /= (uint64_t)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator *= float  |-----------------------------------" << endl;
  cout << ">> voxelrgb *= (float)2;" << endl;
  voxelrgb *= (float)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 10 && voxelrgb.green() == 20 && voxelrgb.blue() == 30 );
  
  cout << "---|  TESTING operator /= float  |-----------------------------------" << endl;
  cout << ">> voxelrgb /= (float)2;" << endl;
  voxelrgb /= (float)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator *= double  |----------------------------------" << endl;
  cout << ">> voxelrgb *= (double)2;" << endl;
  voxelrgb *= (double)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 10 && voxelrgb.green() == 20 && voxelrgb.blue() == 30 );
  
  cout << "---|  TESTING operator /= double  |----------------------------------" << endl;
  cout << ">> voxelrgb /= (double)2;" << endl;
  voxelrgb /= (double)2;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 5 && voxelrgb.green() == 10 && voxelrgb.blue() == 15 );
  
  cout << "---|  TESTING operator (AimsRGB + AimsRGB) |-------------------------" << endl;
  cout << ">> voxelrgb = AimsRGB(1,2,3)+AimsRGB(3,2,1);" << endl;
  voxelrgb = AimsRGB(1,2,3)+AimsRGB(3,2,1);
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 4 && voxelrgb.green() == 4 && voxelrgb.blue() == 4 );
  
  cout << "---|  TESTING operator (AimsRGB - AimsRGB) |-------------------------" << endl;
  cout << ">> voxelrgb = AimsRGB(1,2,3)-AimsRGB(1,2,3);" << endl;
  voxelrgb = AimsRGB(1,2,3)-AimsRGB(1,2,3);
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 0 && voxelrgb.green() == 0 && voxelrgb.blue() == 0 );
  /*
  cout << "---|  TESTING operator (AimsRGB + const) |-------------------------" << endl;
  cout << ">> voxelrgb = AimsRGB(1,2,3)+1;" << endl;
  voxelrgb = AimsRGB(1,2,3)+1;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 2 && voxelrgb.green() == 3 && voxelrgb.blue() == 4 );
  /*
  cout << "---|  TESTING operator (AimsRGB - const) |-------------------------" << endl;
  cout << ">> voxelrgb = AimsRGB(1,2,3)-1;" << endl;
  voxelrgb = AimsRGB(1,2,3)-1;
  cout << "voxelrgb = ( " 
       << (int) voxelrgb.red() << ", "
       << (int) voxelrgb.green() << ", "
       << (int) voxelrgb.blue() << " )"
       << endl;
  ASSERT( voxelrgb.red() == 0 && voxelrgb.green() == 1 && voxelrgb.blue() == 2 );
  */
  cout << "---------------------------------------------------------------------" << endl;
}