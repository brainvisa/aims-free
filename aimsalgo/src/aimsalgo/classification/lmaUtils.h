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


#ifndef LMA_UTILS_H
#define LMA_UTILS_H

#include <cartobase/smart/rcptr.h>
#include <aims/roi/roiIterator.h>
#include <aims/roi/maskIterator.h>
#include <aims/data/fastAllocationData.h>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/bucket/bucketMap.h>
#include <aims/math/math_g.h>
#include <aims/math/pca.h>
#include <aims/math/ppca.h>
#include <aims/math/discriminantanalysis.h>
#include <aims/classification/cah.h>
#include <aims/connectivity/connectivity.h>
#include <aims/connectivity/component_d.h>
#include <aims/morphology/morphology_g.h>
#include <aims/signalfilter/signalfilter_g.h>
#include <cmath>
#include <map>
#include <set>
#include <queue>
#include <utility>

namespace aims {
  class LMAUtils {
  public:
    static inline bool in( const Point3d& dims, const Point3d& p ) ;

    template <class T>
    void data2OptimizedFormat( const AimsData<T>& data, const AimsData<short>& mask, 
		      const Point3d& radiusAround, 
		      AimsData<float>& totalIndivMatrix, AimsData<int>& localisationMatrix, 
		      AimsData<short>& dilatedMask ) ;

    template<class T>
    inline void extractIndividualFromMatrix( const AimsData<float>& totalIndivMatrix, int ind, AimsData<T>& indiv ) ;

    inline bool extractIndividualsFromMatrix( const AimsData<float>& totalIndivMatrix, 
			      const AimsData<int>& localisationMatrix,
			      const list<Point3d>& selectedPoints,
			      AimsData<float>& mean,
			      AimsData<float>& individuals ) ;

    inline bool extractIndividualsFromMatrix( const AimsData<float>& totalIndivMatrix, 
			      const AimsData<int>& localisationMatrix,
			      const Point3d& center,
			      const Point3d& radius,
			      AimsData<float>& mean,
			      AimsData<float>& individuals ) ;

    inline void weightTimeVectors( AimsData<float>& indivMatrix, 
		   const AimsData<float>& weights ) ;
    
    inline void projectIndividualOnLinearModel( const AimsData<float>& indivMatrix, int ind, 
				const AimsData<float>& modelMean, const AimsData<float>& modelDirVect,
				AimsData<float>& projection, AimsData<float>& residual ) ;

    template <class T>
    AimsData<T> getImageFromMapped( const AimsData<int>& locMap,
		    const AimsData<T>& mapped, const AimsData<short>& mask ) ;
    
    template <class T>
    AimsData<T> getImageFromVector( const AimsData<int>& locMap,
		    const vector<T>& mapped, const AimsData<short>& mask ) ;
		    
    AimsData<double> kineticSampleDistance( const vector<DiscriminantAnalysisElement*>& classes ) ;
  private:
  } ;
}

#endif
