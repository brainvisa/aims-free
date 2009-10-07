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

/*
 *  2nd order tensor class
 */
#ifndef AIMS_MATH_TENSOR_H
#define AIMS_MATH_TENSOR_H

#include <aims/config/aimsdata_config.h>
#include <aims/math/trieder.h>


class Tensor;

AIMSDATA_API int operator == (const Tensor& thing1, const Tensor& thing2);
AIMSDATA_API float norm(const Tensor& thing);
AIMSDATA_API std::ostream& operator << (std::ostream& os, const Tensor& thing);


class AIMSDATA_API Tensor
{
  public:
    Tensor()  { }
    Tensor( const Trieder& trieder, const Point3df& eigenvalue )
         : _trieder( trieder ), _eigenvalue( eigenvalue )
    { }
    Tensor( const AimsVector<float,6>& coef )
         : _coef( coef )
    { }
    Tensor(const Tensor& other) : _trieder( other._trieder ),
                                  _eigenvalue( other._eigenvalue ),
                                  _coef( other._coef ),
                                  _trace( other._trace )
    { }
    virtual ~Tensor() { }

    const Trieder& trieder() const { return _trieder; }
    Trieder& trieder() { return _trieder; }

    const Point3df& eigenvalue() const { return _eigenvalue; }
    Point3df& eigenvalue() { return _eigenvalue; }

    const AimsVector<float,6>& coef() const { return _coef; }
    AimsVector<float,6>& coef() { return _coef; }

    const float& trace() const { return _trace; }
    float& trace() { return _trace; }

    float diffusion( const Point3df& dir ) const;

    float meanDiffusivity() const;

    float dot( const Tensor& other ) const;

    friend
    int operator == (const Tensor& thing1, const Tensor& thing2);

    friend
    std::ostream& operator << (std::ostream& os, const Tensor& thing);

  protected:
    Trieder _trieder;
    Point3df _eigenvalue;
    AimsVector<float,6> _coef;
    float _trace;
};


inline
float Tensor::diffusion( const Point3df& dir ) const
{
  return _coef.item(0) * dir.item(0) * dir.item(0) +
         _coef.item(3) * dir.item(1) * dir.item(1) +
         _coef.item(5) * dir.item(2) * dir.item(2) +
         2.0 * _coef.item(1) * dir.item(0) * dir.item(1) +
         2.0 * _coef.item(2) * dir.item(0) * dir.item(2) +
         2.0 * _coef.item(4) * dir.item(1) * dir.item(2);
}


inline
float Tensor::dot( const Tensor& other ) const
{
  return coef().item(0) * other.coef().item(0) +
         coef().item(3) * other.coef().item(3) +
         coef().item(5) * other.coef().item(5) +
         2.0 * coef().item(1) * other.coef().item(1) +
         2.0 * coef().item(2) * other.coef().item(2) +
         2.0 * coef().item(4) * other.coef().item(4) ;
}


inline
float Tensor::meanDiffusivity() const
{
  return ( coef().item(0) + coef().item(3) + coef().item(5) ) / 3.0;
}


inline
int operator == (const Tensor& thing1, const Tensor& thing2)
{
  return thing1.trieder() == thing2.trieder() &&
         thing1.eigenvalue() == thing2.eigenvalue() &&
         thing1.coef() == thing2.coef();
}


inline
float norm(const Tensor& thing)
{
   return sqrt( thing.coef().item(0) * thing.coef().item(0) +
                thing.coef().item(3) * thing.coef().item(3) +
                thing.coef().item(5) * thing.coef().item(5) +
                2.0 * thing.coef().item(1) * thing.coef().item(1) +
                2.0 * thing.coef().item(2) * thing.coef().item(2) +
                2.0 * thing.coef().item(4) * thing.coef().item(4) ); 
}


inline
std::ostream& operator << (std::ostream& os, const Tensor& thing)
{
  os << "{trieder=" << thing.trieder()
     << ",eigenvalue=" << thing.eigenvalue()
     << ",coef=" << thing.coef()
     << ",trace="  << thing.trace();
  os << "}";
  return os;
}


#endif
