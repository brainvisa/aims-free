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

#ifndef AIMS_SIGNALFILTER_FILTERFUNCTION_H
#define AIMS_SIGNALFILTER_FILTERFUNCTION_H

//--- aims -------------------------------------------------------------------
#include <aims/connectivity/structuring_element.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                          // carto::Object
//--- std --------------------------------------------------------------------
#include <vector>
//--- forward declarations ---------------------------------------------------
namespace carto {
  template <typename T> class VolumeRef;
}
template <typename T> class AimsData;
//----------------------------------------------------------------------------

namespace aims {

  /// \brief Pure virtual class: interface for filtering functions
  ///        called by aims::FilteringImageAlgorithm and
  ///        aims::SubSamplingImageAlgorithm
  template <typename T>
  class FilteringFunction
  {
    public:
      virtual ~FilteringFunction() {};
      virtual void setOptions( carto::Object options ) = 0;
      /// Processing function used by aims::SubSamplingImageAlgorithm.
      /// The filtering function is applied to the full window
      virtual T execute( const carto::VolumeRef<T> & volume ) const = 0;
      /// Processing function used by aims::FilteringImageAlgorithm.
      /// The window should be of size one voxel. The structuring element
      /// should contain offsets to the voxels included in the filtering
      /// function. Border must be large enough so that using the
      /// structuring element is safe.
      virtual T execute( const carto::VolumeRef<T> & volume,
                         const StructuringElementRef & se ) const = 0;
      /// Differentiate linear and non linear functions
      virtual bool isLinear() const = 0;
      /// In linear case, the structuring element size depends on the filter
      /// type and its parameters. This method returns this structuring
      /// element. In cases were such a StrEl does not exist (in the non
      /// linear case for example) strel::none() is returned.
      virtual StructuringElementRef getStructuringElement(
        const std::vector<double> & voxel_size = std::vector<double>(4,1.)
      ) const
      {
        return strel::none();
      }
      /// Clone
      virtual FilteringFunction * clone() const = 0;

      //----------------------------------------------------------------------
      //   Bindings
      //----------------------------------------------------------------------
      /// Backward compability
      T doit( const AimsData<T> & in )
      {
        return execute( in.volume() );
      }
  };

} // namespace aims

#endif
