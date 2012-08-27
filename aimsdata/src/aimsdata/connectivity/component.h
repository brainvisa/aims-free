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

#ifndef AIMS_CONNECTIVITY_COMPONENT_H
#define AIMS_CONNECTIVITY_COMPONENT_H


#include <aims/config/aimsdata_config.h>
#include <aims/connectivity/connectivity.h>

template <typename T> class AimsBucket;
class Void;
template <typename T> class AimsData;

namespace aims
{
  
  template <typename T> class BucketMap;
  
  template <typename T, typename O>
  class ConnectedComponentEngine {

    public :
      ConnectedComponentEngine();
  };
  
  // Specialization
  template <typename T, typename O>
  class ConnectedComponentEngine<AimsData<T>, AimsData<O> > {

    public :
      ConnectedComponentEngine();
    
      static void connected( AimsData<T>& data, 
                             AimsData<O>& out, 
                             aims::Connectivity::Type connectivity,
                             std::map<O, size_t>& valids, 
                             const T & backgrnd = 0, bool bin = true, 
                             size_t minSize = 0, size_t numMax = 0, 
                             bool verbose = true );
                             
    
      static void connected( AimsData<T>& data, 
                             AimsData<O>& out, 
                             aims::Connectivity::Type connectivity,
                             const T & backgrnd = 0, bool bin = true, 
                             size_t minSize = 0, size_t numMax = 0, 
                             bool verbose = true );
                                  
  };
         
  template<typename T, typename O>
  inline
  void ConnectedComponentEngine<AimsData<T>, AimsData<O> >::connected( AimsData<T>& data, 
                                                                       AimsData<O>& out, 
                                                                       aims::Connectivity::Type connectivity,
                                                                       const T & backgrnd, bool bin, 
                                                                       size_t minSize, size_t numMax, 
                                                                       bool verbose )
  {
    std::map<O, size_t> valids;
    ConnectedComponentEngine<AimsData<T>, AimsData<O> >::connected(data, 
                                                                   out,
                                                                   connectivity, 
                                                                   valids, 
                                                                   backgrnd, bin, minSize, 
                                                                   numMax, verbose);
  }

  template<typename T>
  inline
  void AimsConnectedComponent( AimsData<T>& data, 
                               aims::Connectivity::Type connectivity,
                               std::map<T, size_t>& valids, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minSize = 0, size_t numMax = 0, 
                               bool verbose = true )
  {
    ConnectedComponentEngine<AimsData<T>, AimsData<T> >::connected(data, 
                                              data,
                                              connectivity, 
                                              valids, 
                                              backgrnd, bin, minSize, 
                                              numMax, verbose);
  }

  template<typename T>
  inline
  void AimsConnectedComponent( AimsData<T>& data,
                               aims::Connectivity::Type connectivity,
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minSize = 0, size_t numMax = 0, 
                               bool verbose = true )
  {
      std::map<T, size_t> valids;
    
      ConnectedComponentEngine<AimsData<T>, AimsData<T> >::connected(data, 
                                                data,
                                                connectivity, 
                                                valids, 
                                                backgrnd, bin, minSize, 
                                                numMax, verbose);
  }

  template<typename T>
  void AimsConnectedComponent( BucketMap<T>& data,
                               aims::Connectivity::Type connectivity,
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minSize = 0, size_t numMax = 0, 
                               bool verbose = true );

  template <typename T>
  void AimsConnectedComponent( AimsBucket<Void>& component,
                               const AimsData<T>& data,
                               aims::Connectivity::Type connectivity, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minsize = 0, size_t maxcomp = 0, 
                               bool verbose = true );
    
  template <typename T>
  void AimsConnectedComponent( AimsBucket<Void>& component,
                               const BucketMap<T>& data,
                               Connectivity::Type connectivity, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minsize = 0, size_t maxcomp = 0, 
                               bool verbose = true );
                               
  template <typename T>
  AimsData<int16_t> AimsLabeledConnectedComponent( AimsBucket<Void>& component,
                                                   const AimsData<T>& data,
                                                   aims::Connectivity::Type connectivity, 
                                                   const T & backgrnd = 0, bool bin = true, 
                                                   size_t minsize = 0, size_t maxcomp = 0, 
                                                   bool verbose = true );
                                                   
}

#endif
