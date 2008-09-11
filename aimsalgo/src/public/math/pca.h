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


#ifndef AIMS_MATH_PCA_H
#define AIMS_MATH_PCA_H

#include <aims/def/general.h>
#include <aims/data/fastAllocationData.h>
#include <vector>


class AimsPCA 
{
public:
  AimsPCA( int significantNumberOfVp, bool normalize = false, bool center = true ) ;
  ~AimsPCA() {}
  
  template <class T>
    void doIt( const AimsData<T>& individuals ) ;
  
  template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, const AimsData<T>& data ) ;
  
  
  float noiseVariance( float& meanNorm ) ;
  double totalVariance( float& meanNorm ) const ;
    
  float relativeUnreconstructedVariance( float& meanNorm ) ;
  void setSignificantNumberOfVp( int significantNumberOfVp ) ;
  void setMinimalInertia( float inertiaLimit ) ;
  
  float minimalInertia() const ;
  float significantNumberOfVp() const ;

  float significantInertia() ;
  float noiseInertia() ;
  float unreconstructedVariance( ) ;
  float relativeUnreconstructedVariance( ) ;
  AimsData<float> projection( const AimsData<float>& individual ) ;
  float reconstructionError2( const AimsData<float>& individual ) ;
  
  const AimsData<float>& projectionMatrix() ;
  const AimsData<float>& reconstructionErrorMatrix() ;
  
  const std::vector<float>& eigenValues() const ;
  const AimsData<float>& eigenVectors() const ;
  AimsData<float> selectedEigenVectors() const ;
  
  const std::vector<float>& mean() const ;
  const std::vector<float>& var() const ;
  
  bool valid() const { return _validPca ; }
  
protected:
  void computeErrorAndProjMatrices() ;
  bool _validPca ;
  bool _computed ;
  bool _matricesComputed ;
  bool _center ;
  bool _normalize ;
  std::vector<float> _mean ;
  std::vector<float> _var ;
  
  std::vector<float> _projectionVector ;
  aims::AimsFastAllocationData<float> _projectionMatrix ;
  aims::AimsFastAllocationData<float> _errorMatrix ;
  std::vector<float> _eigenValues ;
  aims::AimsFastAllocationData<float> _eigenVectors ;
  aims::AimsFastAllocationData<float> _selectedEigenVectors ;
  aims::AimsFastAllocationData<float> _selectedEigenVectorsTr ;
  
  int _significantNumberOfVp ;
  float _minimalInertia ;
};

#endif
