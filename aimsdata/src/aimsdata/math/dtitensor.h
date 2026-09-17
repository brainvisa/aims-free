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
