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


#ifndef LMA_UTILS_H
#define LMA_UTILS_H

#include <cartodata/volume/volume.h>

namespace aims {
  class LMAUtils {
  public:
    static inline bool in( const Point3d& dims, const Point3d& p ) ;

    template <class T>
    void data2OptimizedFormat(
      const carto::rc_ptr<carto::Volume<T> >& data,
      const carto::rc_ptr<carto::Volume<short> >& mask,
      const Point3d& radiusAround,
      carto::rc_ptr<carto::Volume<float> >& totalIndivMatrix,
      carto::rc_ptr<carto::Volume<int> >& localisationMatrix,
      carto::rc_ptr<carto::Volume<short> >& dilatedMask ) ;

    template<class T>
    inline void extractIndividualFromMatrix(
      const carto::rc_ptr<carto::Volume<float> >& totalIndivMatrix, int ind,
      carto::rc_ptr<carto::Volume<T> >& indiv ) ;

    inline bool extractIndividualsFromMatrix(
      const carto::rc_ptr<carto::Volume<float> >& totalIndivMatrix,
      const carto::rc_ptr<carto::Volume<int> >& localisationMatrix,
      const list<Point3d>& selectedPoints,
      carto::rc_ptr<carto::Volume<float> >& mean,
      carto::rc_ptr<carto::Volume<float> >& individuals ) ;

    inline bool extractIndividualsFromMatrix(
      const carto::rc_ptr<carto::Volume<float> >& totalIndivMatrix,
      const carto::rc_ptr<carto::Volume<int> >& localisationMatrix,
      const Point3d& center,
      const Point3d& radius,
      carto::rc_ptr<carto::Volume<float> >& mean,
      carto::rc_ptr<carto::Volume<float> >& individuals ) ;

    inline void weightTimeVectors(
      carto::rc_ptr<carto::Volume<float> >& indivMatrix,
      const carto::rc_ptr<carto::Volume<float> >& weights ) ;
    
    inline void projectIndividualOnLinearModel(
      const carto::rc_ptr<carto::Volume<float> >& indivMatrix, int ind,
      const carto::rc_ptr<carto::Volume<float> >& modelMean,
      const carto::rc_ptr<carto::Volume<float> >& modelDirVect,
      carto::rc_ptr<carto::Volume<float> >& projection,
      carto::rc_ptr<carto::Volume<float> >& residual ) ;

    template <class T>
    carto::VolumeRef<T> getImageFromMapped(
      const carto::rc_ptr<carto::Volume<int> >& locMap,
      const carto::rc_ptr<carto::Volume<T> >& mapped,
      const carto::rc_ptr<carto::Volume<short> >& mask ) ;
    
    template <class T>
    carto::VolumeRef<T> getImageFromVector(
      const carto::rc_ptr<carto::Volume<int> >& locMap,
      const vector<T>& mapped,
      const carto::rc_ptr<carto::Volume<short> >& mask ) ;
		    
    carto::VolumeRef<double> kineticSampleDistance( const vector<DiscriminantAnalysisElement*>& classes ) ;
  private:
  } ;
}

#endif
