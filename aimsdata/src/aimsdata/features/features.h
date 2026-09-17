#ifndef AIMS_FEATURES_H
#define AIMS_FEATURES_H


#include <vector>
#include <string>


namespace aims {

  //--------------------------//
 //  ScalarFeaturesProvider  //
//--------------------------//

/**
 * This class should be inherited by all subclasses that provides scalar
 * features. A scalar feature is a named scalar value.
 */
class ScalarFeaturesProvider
{
public:
  typedef double Scalar_t;

  inline virtual ~ScalarFeaturesProvider();
  
  /** Return the name of all scalar features provided by this object
   */
  inline const std::vector< std::string > &scalarFeatureNames() const;
  /** Clear an fill its parameter with the values of the scalar features. The 
   *  returned vector has the same size as the one returned by 
   * scalarFeatureNames().
   */
  virtual void scalarFeatureValues( std::vector< Scalar_t > & ) const = 0;

protected:

  /** Constructors of derived classes must fill this vector with scalar
   * feature names.
   */
  std::vector< std::string > _scalarFeatureNames;
};


  //--------------------------//
 //  ScalarFeaturesProvider  //
//--------------------------//

//-----------------------------------------------------------------------------
inline ScalarFeaturesProvider::~ScalarFeaturesProvider() {}


//-----------------------------------------------------------------------------
inline const std::vector< std::string > &
ScalarFeaturesProvider::scalarFeatureNames() const
{
  return _scalarFeatureNames;
}


} // namesapce aims

#endif // ifndef AIMS_FEATURES_H
