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

#ifndef AIMS_TRANSFORM_TALAIRACHTRANSFORM_H
#define AIMS_TRANSFORM_TALAIRACHTRANSFORM_H

#include <aims/vector/vector.h>
#include <aims/data/data.h>
#include <vector>

class Motion ;

namespace aims {
  
  class TalairachTransformation {
  public:
    enum TranslationComputation {
      FROMMASK,
      FROMAPC
    } ;
    
    TalairachTransformation() ;
    TalairachTransformation( std::vector<Point3df> first, 
			     AimsData<short> firstMask = AimsData<short>(),
			     std::vector<Point3df> second =  std::vector<Point3df>(), 
			     AimsData<short> secondMask = AimsData<short>(),
			     TranslationComputation translationComputation = FROMAPC ) ;
    
    ~TalairachTransformation() ;
    
    void setFirstTalairachPoints( std::vector<Point3df> first ) ;
    void setFirstMask( AimsData<short> firstMask ) ;
    
    void setSecondTalairachPoints( std::vector<Point3df> second ) ;
    void setSecondMask( AimsData<short> secondMask ) ;
    
    const Motion& transformation( ) ;
    
  private:
    std::vector<Point3df> _firstPoints ;
    std::vector<Point3df> _secondPoints ;
    AimsData<short> _firstMask ;
    AimsData<short> _secondMask ;
    
    TranslationComputation _translationComputation ;
    Motion * _motion ;
    bool _motionComputed ;
    bool _computable ;
    bool _withMask ;
    bool _toNormalizedTemplate ;
    const Motion& withoutMaskComputation() ;
    const Motion& withMaskComputation() ;
  } ;
}

#endif
