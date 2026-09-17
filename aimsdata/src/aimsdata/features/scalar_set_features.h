#ifndef AIMS_SCALAR_SET_FEATURES_H
#define AIMS_SCALAR_SET_FEATURES_H


#include <aims/features/features.h>


namespace aims {

  //---------------------//
 //  ScalarSetFeatures  //
//---------------------//

class ScalarSetFeatures : public ScalarFeaturesProvider
{
private:
  void _setScalarFeatureNames();

public:
  typedef ScalarFeaturesProvider::Scalar_t Scalar_t;

  ScalarSetFeatures();
  ScalarSetFeatures( const std::vector< Scalar_t > & );
  void setValues( const std::vector< Scalar_t > & );
  void setValues( const std::vector< Scalar_t > &,
                  const std::vector< Scalar_t > & );
  inline virtual ~ScalarSetFeatures();
  
  inline const std::vector< Scalar_t > &values() const;
  inline std::vector< Scalar_t > &values();

  virtual void scalarFeatureValues( std::vector< Scalar_t > & ) const;

private:

  // The following is mutable because it is necessary to sort
  // values for median computation.
  mutable std::vector< Scalar_t > _values;
  std::vector< Scalar_t > _weights;
};


  //---------------------//
 //  ScalarSetFeatures  //
//---------------------//

//-----------------------------------------------------------------------------
inline ScalarSetFeatures::~ScalarSetFeatures() {}


//-----------------------------------------------------------------------------
inline const std::vector< ScalarSetFeatures::Scalar_t > &
ScalarSetFeatures::values() const
{
  return _values;
}


//-----------------------------------------------------------------------------
inline std::vector< ScalarSetFeatures::Scalar_t > &
ScalarSetFeatures::values()
{
  return _values;
}



} // namesapce aims

#endif // ifndef AIMS_SCALAR_SET_FEATURES_H
