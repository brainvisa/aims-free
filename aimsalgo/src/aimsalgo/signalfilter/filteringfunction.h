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

  //==========================================================================
  // FILTERING FUNCTON INTERFACE
  //==========================================================================
  /// \brief Pure virtual class: interface for filtering functions
  ///        called by aims::FilteringImageAlgorithm and
  ///        aims::SubSamplingImageAlgorithm
  template <typename T>
  class FilteringFunctionInterface
  {
    public:
      virtual ~FilteringFunctionInterface() {};
      //----------------------------------------------------------------------
      // interface
      //----------------------------------------------------------------------
      /// Basic execute method that all derived class must implement (in
      /// the case where more parameters are needed, this methods runs
      /// with their default value)
      virtual T execute( const carto::VolumeRef<T> & in ) const = 0;
      /// Set the parameters of the filters
      /// If a parameter value is not set in the options object, a default
      /// value must be assigned.
      /// This method may do nothing if no options are available.
      virtual void setOptions( const carto::Object & options ) {}
      /// Update the parameters of the filters
      /// If a parameter value is not set in the options object, the current
      /// value must be kept.
      /// This method may do nothing if no options are available.
      virtual void updateOptions( const carto::Object & options ) {}
      /// clone method. It is better to implement it using a copy constructor.
      virtual FilteringFunctionInterface *clone() const = 0;

      //----------------------------------------------------------------------
      // backward compability
      //----------------------------------------------------------------------
      /// \deprecated
      T doit( const AimsData<T> & in ) __attribute__((__deprecated__))
      {
        return execute( in.volume() );
      }

    protected:
      //----------------------------------------------------------------------
      //   magic 3
      //----------------------------------------------------------------------
      FilteringFunctionInterface() {}
      FilteringFunctionInterface( const FilteringFunctionInterface<T> & );
      FilteringFunctionInterface<T> & operator= ( const FilteringFunctionInterface<T> & );
  };

} // namespace aims

#endif
