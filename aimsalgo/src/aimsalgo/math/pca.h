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


#ifndef AIMS_MATH_PCA_H
#define AIMS_MATH_PCA_H

#include <aims/def/general.h>
#include <cartodata/volume/volume.h>
#include <vector>


class AimsPCA 
{
public:
  AimsPCA( int significantNumberOfVp, bool normalize = false, bool center = true ) ;
  ~AimsPCA() {}
  
  template <class T>
    void doIt( const carto::rc_ptr<carto::Volume<T> > & individuals ) ;
  
  template <class T>
    void doIt( const std::list< Point3d>& selectedPoints, const carto::rc_ptr<carto::Volume<T> > & data ) ;
  
  
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
  carto::VolumeRef<float> projection(
    const carto::rc_ptr<carto::Volume<float> > & individual ) ;
  float reconstructionError2(
    const carto::rc_ptr<carto::Volume<float> > & individual ) ;
  
  const carto::VolumeRef<float>& projectionMatrix() ;
  const carto::VolumeRef<float>& reconstructionErrorMatrix() ;
  
  const std::vector<float>& eigenValues() const ;
  const carto::VolumeRef<float>& eigenVectors() const ;
  carto::VolumeRef<float> selectedEigenVectors() const ;
  
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
  carto::VolumeRef<float> _projectionMatrix ;
  carto::VolumeRef<float> _errorMatrix ;
  std::vector<float> _eigenValues ;
  carto::VolumeRef<float> _eigenVectors ;
  carto::VolumeRef<float> _selectedEigenVectors ;
  carto::VolumeRef<float> _selectedEigenVectorsTr ;
  
  int _significantNumberOfVp ;
  float _minimalInertia ;
};

#endif
