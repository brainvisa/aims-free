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
