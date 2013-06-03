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

#ifndef AIMS_RGB_RGB2_H
#define AIMS_RGB_RGB2_H
//--- soma-io ------------------------------------------------------------------
#include <soma-io/image/voxelvalue_d.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/type/types.h>
//--- system -------------------------------------------------------------------
#define AIMS_DEBUG_RGB
#ifdef AIMS_DEBUG_RGB
  #include <iostream>
#endif
//------------------------------------------------------------------------------

//class AimsRGBA;

class AimsRGB : public soma::VoxelValue<uint8_t,3>
{
  public:
    AimsRGB( const soma::VoxelValue<uint8_t,3> &other ) : soma::VoxelValue<uint8_t,3>( other ) { }
    AimsRGB( const uint8_t &r = 0, const uint8_t &g = 0, const uint8_t &b = 0 )
    : soma::VoxelValue<uint8_t,3>()
    {
      #ifdef AIMS_DEBUG_RGB
        std::cout << "AIMSRGB:: constructor( " 
                  << (int) r << ", "
                  << (int) g << ", "
                  << (int) b << " )" << std::endl;
      #endif
      (*this)[0] = r;
      (*this)[1] = g;
      (*this)[2] = b;
    }
    AimsRGB( const AimsRGB &other ) : soma::VoxelValue<uint8_t,3>( other ) { }
    ~AimsRGB() {};
    
    AimsRGB & operator = ( const AimsRGB & other )
    {
      #ifdef AIMS_DEBUG_RGB
        std::cout << "AIMSRGB:: operator=( other )" << std::endl;
      #endif
      (*this)[0] = other[0];
      (*this)[1] = other[1];
      (*this)[2] = other[2];
      return *this;
    }
    
    AimsRGB & operator = ( const uint8_t & value )
    {
      #ifdef AIMS_DEBUG_RGB
        std::cout << "AIMSRGB:: operator=( " 
                  << (int) value << " )" << std::endl;
      #endif
      (*this)[0] = (uint8_t) value;
      (*this)[1] = (uint8_t) value;
      (*this)[2] = (uint8_t) value;
      return *this;
    }
    
    const uint8_t& red  () const { return (*this)[0]; }
    const uint8_t& green() const { return (*this)[1]; }
    const uint8_t& blue () const { return (*this)[2]; }
          uint8_t& red  ()       { return (*this)[0]; }
          uint8_t& green()       { return (*this)[1]; }
          uint8_t& blue ()       { return (*this)[2]; }
};

#endif