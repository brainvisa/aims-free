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

#ifndef _AIMSMIREGISTER_OBJFUNC_H_
#define _AIMSMIREGISTER_OBJFUNC_H_

#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/resampling/sampler.h>
#include <aims/resampling/resampler.h>
#include <aims/information/pdf.h>
#include <aims/information/information.h>
#include <aims/io/writer.h>
#include <time.h>

// #define DEBUG 1

// ============================================================
// Declaration
// ============================================================
typedef short DefaultVoxelType;
typedef float DefaultParameterType;
static const unsigned int DefaultParametersSize = 6;

// ------------------------------------------------------------
// VolumeObjectiveFunc
// ------------------------------------------------------------
template <class T=DefaultVoxelType, 
          class P=DefaultParameterType, 
          int S=DefaultParametersSize>
class VolumeObjectiveFunc: public ObjectiveFunc<P, S>
{
public:
  static const unsigned int parameterscount = S;

  // ------------------------------------------------------------
  // Public type definitions
  // ------------------------------------------------------------
  typedef ObjectiveFunc<P, S> BaseType;

  typedef T VoxelType;
  typedef carto::VolumeRef<T> VolumeRefType;
  typedef aims::Resampler<T> ResamplerType;
  typedef Sampler<T> SamplerType;

  typedef P ParameterType;
  typedef typename BaseType::ParametersType ParametersType;
  
  // ------------------------------------------------------------
  // Public methods
  // ------------------------------------------------------------
  VolumeObjectiveFunc(int numLevel,
                      ResamplerType* interpolator,
                      SamplerType* comb,	     
                      int maskSize);
  virtual ~VolumeObjectiveFunc();

  virtual float eval(const ParametersType &) const { return 0.0; }
  void setRef(const VolumeRefType &);
  void setTest(const VolumeRefType &);
  void setGcRef(const float x, const float y, const float z);
  void setGcTest(const float x, const float y, const float z); 
  bool dataSettingOK() const { return _refOK && _testOK; };
  bool usePV() const { return !_interpolator; };

  virtual aims::AffineTransformation3d getTransformation(const ParametersType &) const;
  virtual Point3df getRotation(const ParametersType &) const = 0;
  virtual Point3df getTranslation(const ParametersType &) const = 0;

  // This method adds the attributed object useful parameters
  // of the objective function to be monitored
  virtual void fillMonitoredParameters(carto::AttributedObject &) const; 
  
protected:
  // ------------------------------------------------------------
  // Protected methods
  // ------------------------------------------------------------
  virtual void doPdf( const ParametersType& param ) const;
  
  // ------------------------------------------------------------
  // Protected attributes
  // ------------------------------------------------------------
  mutable VolumeRefType             _ref;    // Master Reference image managed as reference
  mutable VolumeRefType             _test;   // Master Test image managed as reference
  VolumeRefType*                    _reg;
  carto::VolumeRef<PVItem>        * _regc;
  
  int                               _numLevel;
  ResamplerType*                    _interpolator;
  SamplerType*                      _comb;
  carto::VolumeRef<float>         * _mask;
  carto::VolumeRef<float>         * _p1;
  carto::VolumeRef<float>         * _p2;
  carto::VolumeRef<float>         * _p12;
  
  AimsVector<float, 3>              _gcref;
  AimsVector<float, 3>              _gctest;
  
  bool                              _refOK, _testOK;
};

// ------------------------------------------------------------
// ObjFunc
// ------------------------------------------------------------
template <class T=DefaultVoxelType, 
          class P=DefaultParameterType, 
          int S=DefaultParametersSize>
class ObjFunc: public VolumeObjectiveFunc<T, P, S>
{
public:
  // ------------------------------------------------------------
  // Public type definitions
  // ------------------------------------------------------------
  typedef VolumeObjectiveFunc<T, P, S> BaseType;
  typedef typename BaseType::VoxelType VoxelType;
  typedef typename BaseType::ResamplerType ResamplerType;
  typedef typename BaseType::SamplerType SamplerType;

  typedef typename BaseType::ParameterType ParameterType;
  typedef typename BaseType::ParametersType ParametersType;
  
  // ------------------------------------------------------------
  // Public methods
  // ------------------------------------------------------------
  ObjFunc(int numLevel,
          ResamplerType * interpolator,
          SamplerType * comb,	     
          int maskSize);
  virtual ~ObjFunc(){}

  virtual Point3df getRotation(const ParametersType &) const;
  virtual Point3df getTranslation(const ParametersType &) const;
};

// ------------------------------------------------------------
// ObjFunc<T, 3> specialization
// ------------------------------------------------------------
template <class T, class P>
class ObjFunc<T, P, 3>: public VolumeObjectiveFunc<T, P, 3>
{
public:
  // ------------------------------------------------------------
  // Public type definitions
  // ------------------------------------------------------------
  typedef VolumeObjectiveFunc<T, P, 3> BaseType;

  typedef typename BaseType::VoxelType VoxelType;
  typedef typename BaseType::ResamplerType ResamplerType;
  typedef typename BaseType::SamplerType SamplerType;

  typedef typename BaseType::ParameterType ParameterType;
  typedef typename BaseType::ParametersType ParametersType;
  
  // ------------------------------------------------------------
  // Public methods
  // ------------------------------------------------------------
  ObjFunc(int numLevel,
          ResamplerType * interpolator,
          SamplerType * comb,	     
          int maskSize);
  virtual ~ObjFunc(){}

  virtual int getAxis() const;
  virtual void setAxis(const int &);
  virtual Point3df getRotation(const ParametersType &) const;
  virtual Point3df getTranslation(const ParametersType &) const;

protected:
  int _axis;
};

// ------------------------------------------------------------
// ObjFunc<T, 6> specialization
// ------------------------------------------------------------
template <class T, class P>
class ObjFunc<T, P, 6>: public VolumeObjectiveFunc<T, P, 6>
{
public:
  // ------------------------------------------------------------
  // Public type definitions
  // ------------------------------------------------------------
  typedef VolumeObjectiveFunc<T, P, 6> BaseType;

  typedef typename BaseType::VoxelType VoxelType;
  typedef typename BaseType::ResamplerType ResamplerType;
  typedef typename BaseType::SamplerType SamplerType;

  typedef typename BaseType::ParameterType ParameterType;
  typedef typename BaseType::ParametersType ParametersType;
  
  // ------------------------------------------------------------
  // Public methods
  // ------------------------------------------------------------
  ObjFunc(int numLevel,
          ResamplerType * interpolator,
          SamplerType * comb,	     
          int maskSize);
  virtual ~ObjFunc(){}

  virtual Point3df getRotation(const ParametersType &) const;
  virtual Point3df getTranslation(const ParametersType &) const;
};

// ============================================================
// Implementation
// ============================================================

// ------------------------------------------------------------
// VolumeObjectiveFunc
// ------------------------------------------------------------
template <class T, class P, int S>
inline
VolumeObjectiveFunc<T, P, S>::VolumeObjectiveFunc(int numLevel, 
                                                  ResamplerType * interpolator,
                                                  SamplerType * comb,
                                                  int maskSize)
  :BaseType(), 
    _ref(), _test(),
    _numLevel(numLevel),
    _interpolator(interpolator),
    _comb(comb) {

  _refOK = _testOK = false;
  _mask = new carto::VolumeRef<float>( maskSize );

  for ( int n = 0; n < maskSize; n++ )  
    (*_mask)( n ) = exp( - 0.5 * aims::sqr( n - maskSize / 2 ) );


  _p1  = new carto::VolumeRef<float>( _numLevel );
  _p2  = new carto::VolumeRef<float>( _numLevel );
  _p12 = new carto::VolumeRef<float>( _numLevel, _numLevel );
 
  _gctest = _gcref = AimsVector<float, 3>(0., 0., 0.) ;
  
}

template <class T, class P, int S>
inline
void VolumeObjectiveFunc<T, P, S>::setGcRef(const float x, const float y, const float z) {
  _gcref[0] = x;
  _gcref[1] = y;

  if (S > 2)
    _gcref[2] = z;
}

template <class T, class P, int S>
inline
void VolumeObjectiveFunc<T, P, S>::setGcTest(const float x, const float y, const float z) {
  _gctest[0] = x;
  _gctest[1] = y;
  if (S > 2)
    _gctest[2] = z;
}

template <class T, class P, int S>
inline
void VolumeObjectiveFunc<T, P, S>::setTest(const VolumeRefType & test) {

  _test = test;
  _testOK = true;

#if DEBUG
  std::cout << "VolumeObjectiveFunc<T, P, S>::setTest test image infos" << std::endl
            << "  - dimensions: [" 
            << carto::toString(_test.getSizeX()) << ", "
            << carto::toString(_test.getSizeY()) << ", "
            << carto::toString(_test.getSizeZ()) << "]" << std::endl
            << "  - voxel size: [" 
            << carto::toString(_test.getVoxelSize()[0]) << ", "
            << carto::toString(_test.getVoxelSize()[1]) << ", "
            << carto::toString(_test.getVoxelSize()[2]) << "]" << std::endl;
#endif

}

template <class T, class P, int S>
inline
void VolumeObjectiveFunc<T, P, S>::setRef(const VolumeRefType & ref) {
  _ref = ref;
  if (usePV()) {
    _regc = new carto::VolumeRef<PVItem>(ref.getSizeX(), ref.getSizeY(), ref.getSizeZ());
    _regc->setVoxelSize(ref.getVoxelSize()[0] , ref.getVoxelSize()[1], ref.getVoxelSize()[2], 1.0);
  }
  else {
    _reg = new VolumeRefType(ref.getSizeX(), ref.getSizeY(), ref.getSizeZ());
    _reg->setVoxelSize(ref.getVoxelSize()[0], ref.getVoxelSize()[1], ref.getVoxelSize()[2], 1.0);
  }

  _refOK = true;

#if DEBUG 
  std::cout << "VolumeObjectiveFunc<T, P, S>::setRef reference image infos:" << std::endl
            << "  - dimensions: [" 
            << carto::toString(_ref->getSizeX()) << ", "
            << carto::toString(_ref->getSizeY()) << ", "
            << carto::toString(_ref->getSizeZ()) << "]" << std::endl
            << "  - voxel size: [" 
            << carto::toString(_ref->getVoxelSize()[0]) << ", "
            << carto::toString(_ref->getVoxelSize()[1]) << ", "
            << carto::toString(_ref->getVoxelSize()[2]) << "]" << std::endl;
  std::cout << std::flush;
#endif  
}

template <class T, class P, int S>
inline
void VolumeObjectiveFunc<T, P, S>::doPdf(const ParametersType & param) const {
  aims::AffineTransformation3d motion = getTransformation(param);

  if (usePV()) {
    // Partial values pdf processing
    _comb->setRef(_test);
    _comb->doit(motion, *_regc);

    AimsJointPVPdf(_ref, _test, *_regc,  *_p12, *_p1, *_p2);    
  }
  else {
    // Direct pdf processing: an interpolator is required
    ASSERT( _interpolator );
    // Disable interpolation progress display
    std::ofstream ofs;
    ofs.setstate(std::ios_base::badbit);
    _interpolator->setVerboseStream(ofs);
    _interpolator->setRef(_test);
    _interpolator->doit(motion, **_reg);
    AimsJointMaskPdf(_ref, *_reg, *_p12, *_p1, *_p2);
  }
}

template <class T, class P, int S>
inline
aims::AffineTransformation3d 
VolumeObjectiveFunc<T, P, S>::getTransformation(const ParametersType & param) const
{
  // std::cout << "VolumeObjectiveFunc<T, P, S>::getTransformation" << std::endl << std::flush;

  aims::AffineTransformation3d transformation;

  // Rotation around gravity center of test image
  Point3df rotation = getRotation(param);
  transformation.setRotationAffine(rotation[0], 
                                   rotation[1], 
                                   rotation[2], 
                                   this->_gctest);

  // Translation of gravity centers
  Point3df translation = this->_gcref
                         - this->_gctest 
                         + getTranslation(param);
  transformation.setTranslation(translation);

  return transformation;
}

template <class T, class P, int S>
inline
void VolumeObjectiveFunc<T, P, S>::fillMonitoredParameters(carto::AttributedObject & o) const {
  o.setProperty( "joint_histogram", *_p12 ) ;
  o.setProperty( "mask", *_mask ) ;
  o.setProperty( "gc_ref", _gcref ) ;
  o.setProperty( "gc_test", _gctest ) ;  
  
  return ;
} 

template <class T, class P, int S>
inline
VolumeObjectiveFunc<T, P, S>::~VolumeObjectiveFunc() {
  std::cout << "VolumeObjectiveFunc<T, S>::~VolumeObjectiveFunc" << std::endl << std::flush;
  if (_p12) delete _p12;
  if (_p2) delete _p2;
  if (_p1) delete _p1;
  if (_mask) delete _mask;
  if (_reg) delete _reg;
}

// ------------------------------------------------------------
// ObjFunc
// ------------------------------------------------------------

// ------------------------------------------------------------
// ObjFunc<T, P, 3> specialization
// ------------------------------------------------------------
// Partial specialization for 2D registration with 3 parameters
// ------------------------------------------------------------
template <class T, class P>
inline
ObjFunc<T, P, 3>::ObjFunc(int numLevel, 
                          ResamplerType * interpolator,
                          SamplerType * comb,
                          int maskSize)
  :BaseType(
      numLevel, 
      interpolator,
      comb,
      maskSize), 
  _axis(2) {
}

template <class T, class P>
inline
Point3df ObjFunc<T, P, 3>::getRotation(const ParametersType & param) const {
  Point3df rotation(0, 0, 0);
  rotation[_axis] = param[2];
  return rotation;
} 

template <class T, class P>
inline
Point3df ObjFunc<T, P, 3>::getTranslation(const ParametersType & param) const {
  // For rotation axis around X, axis is 0 and translation is (0.0, param[0], param[1])
  // For rotation axis around Y, axis is 1 and translation is (param[1], 0.0, param[0])
  // For rotation axis around Z, axis is 2 and translation is (param[0], param[1], 0.0)
  Point3df translation(0, 0, 0);
  translation[(_axis + 1) % 3] = param[0];
  translation[(_axis + 2) % 3] = param[1];
  return translation;
}

template <class T, class P>
inline
int ObjFunc<T, P, 3>::getAxis() const {
  return _axis;
} 

template <class T, class P>
inline
void ObjFunc<T, P, 3>::setAxis(const int & axis) {
  _axis = axis;
}

// ------------------------------------------------------------
// ObjFunc<T, 6> specialization
// ------------------------------------------------------------
// Partial specialization for 3D registration with 6 parameters
// ------------------------------------------------------------
template <class T, class P>
inline
ObjFunc<T, P, 6>::ObjFunc(int numLevel, 
                          ResamplerType * interpolator,
                          SamplerType * comb,
                          int maskSize)
  :BaseType(
      numLevel, 
      interpolator,
      comb,
      maskSize)
{}

template <class T, class P>
inline
Point3df ObjFunc<T, P, 6>::getRotation(const ParametersType & param) const {
  return Point3df(param[4], param[5], param[2]);
}

template <class T, class P>
inline
Point3df ObjFunc<T, P, 6>::getTranslation(const ParametersType & param) const {
  return Point3df(param[0], param[1], param[3]);
}

#endif
