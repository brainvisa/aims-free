/* Copyright (c) 1995-2005 CEA
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

/*
 *  2nd order tensor class for DTI
 */
#ifndef AIMS_MATH_DTITENSOR_H
#define AIMS_MATH_DTITENSOR_H

#include <aims/math/tensor.h>
#include <aims/math/mathelem.h>
#include <cartobase/type/types.h>


class DtiTensor;

float volumeRatio( const DtiTensor& thing );
float fractionalAniso( const DtiTensor& thing );
std::ostream& operator << (std::ostream& os, const DtiTensor& thing);


class DtiTensor
{
  public:

    enum
    {
      CORRECTED_N26 = 0, // corrected with estimation of mean diffusivity on N26
      CORRECTED_TC,      // corrected with estimation of mean diffusivity on
                         // tensor coefficients
      NO_PROBLEM,
      PROBLEM,
      NOT_POSITIVE_MATRIX,
      BAD_EIGENSYSTEM
    };


    DtiTensor()  { }
    DtiTensor( const Trieder& trieder, const Point3df& eigenvalue )
         : _base( trieder, eigenvalue )
    { }
    DtiTensor( const AimsVector<float,6>& coef )
         : _base( coef )
    { }
    DtiTensor( const Tensor& other ) : _base( other )
    { }
    DtiTensor(const DtiTensor& other) : _base( other._base ),
                                        _dir( other._dir ),
                                        _location( other._location ),
                                        _anisotropyVR( other._anisotropyVR ),
                                        _anisotropyFA( other._anisotropyFA ),
                                        _category( other._category )
    { }
    virtual ~DtiTensor() { }

    const Tensor& base() const { return _base; }
    Tensor& base() { return _base; }

    const Point3df& dir() const { return _dir; }
    Point3df& dir() { return _dir; }

    const Point3df& location() const { return _location; }
    Point3df& location() { return _location; }

    const float& anisotropyVR() const { return _anisotropyVR; }
    float& anisotropyVR() { return _anisotropyVR; }

    const float& anisotropyFA() const { return _anisotropyFA; }
    float& anisotropyFA() { return _anisotropyFA; }

    const int& category() const { return _category; }
    int& category() { return _category; }

    const short& index() const { return _index; }
    short& index() { return _index; }

    friend
    std::ostream& operator << (std::ostream& os, const DtiTensor& thing);

  protected:
    Tensor _base;
    Point3df _dir;
    Point3df _location;
    float _anisotropyVR;
    float _anisotropyFA; 
    int _category;
    short _index;
};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<> inline std::string DataTypeCode<DtiTensor>::dataType()
  {
    return "DTITENSOR";
  }

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


inline
float volumeRatio( const DtiTensor& thing )
{

  float averageDc = thing.base().meanDiffusivity();
  if ( averageDc <= 0.0 )
    return 0.0;

  float thirdInvariant = thing.base().coef().item( 0 ) *
                         ( thing.base().coef().item( 3 ) *
                           thing.base().coef().item( 5 ) -
                           thing.base().coef().item( 4 ) *
                           thing.base().coef().item( 4 ) ) -
                         thing.base().coef().item( 1 ) *
                         ( thing.base().coef().item( 1 ) *
                           thing.base().coef().item( 5 ) -
                           thing.base().coef().item( 2 ) *
                           thing.base().coef().item( 4 ) ) +
                         thing.base().coef().item( 2 ) *
                         ( thing.base().coef().item( 1 ) *
                           thing.base().coef().item( 4 ) -
                           thing.base().coef().item( 2 ) *
                           thing.base().coef().item( 3 ) );
                   
  if ( thirdInvariant < 0.0 )
    return 1.0;

  float val = 1.0 - thirdInvariant / cube( averageDc );

  if ( val < 0.0 )
    return 0.0;

  return val;

}


inline
float fractionalAniso( const DtiTensor& thing )
{

  float averageDc = thing.base().meanDiffusivity();

  float fourthInvariant = thing.base().coef().item( 0 ) *
                          thing.base().coef().item( 0 ) +
                          thing.base().coef().item( 3 ) *
                          thing.base().coef().item( 3 ) +
                          thing.base().coef().item( 5 ) *
                          thing.base().coef().item( 5 ) +
                          ( thing.base().coef().item( 1 ) * 
                            thing.base().coef().item( 1 ) +
                            thing.base().coef().item( 2 ) *
                            thing.base().coef().item( 2 ) +
                            thing.base().coef().item( 4 ) *
                            thing.base().coef().item( 4 ) ) * 2.0;

  float magnitudeDc = ( fourthInvariant < 0.0 ? 0.0 : 
                                          sqrt( fourthInvariant / 3.0 ) );

  float val = ( float )sqrt( 1.5 * ( 1.0 - ( averageDc * averageDc ) / 
                                           ( magnitudeDc * magnitudeDc ) ) );
  if ( val > 1.0 )
    val = 1.0;

  return val;

}


inline
std::ostream& operator << (std::ostream& os, const DtiTensor& thing)
{
  os << "{base="       << thing.base()
     << ",dir="        << thing.dir()
     << ",location="   << thing.location()
     << ",VR="         << thing.anisotropyVR()
     << ",FA="         << thing.anisotropyFA()
     << ",category=";
  switch( thing.category() )
  {
    case DtiTensor::NO_PROBLEM          : os << "NO_PROBLEM";break;
    case DtiTensor::NOT_POSITIVE_MATRIX : os << "NOT_POSITIVE_MATRIX";break;
    case DtiTensor::CORRECTED_N26       : os << "CORRECTED_N26";break;
    case DtiTensor::CORRECTED_TC        : os << "CORRECTED_TC";break;
    default                             : os << "UNKNOWN";break;
  }
  os << ",index=" << thing.index();
  os << "}";
  return os;
}


#endif
