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

#ifndef CARTOBASE_OBJECT_HEADERED_H
#define CARTOBASE_OBJECT_HEADERED_H


#include <cartobase/pattern/observable.h>
#include <cartobase/pattern/observer.h>
#include <cartobase/object/property.h>
#include <cartobase/object/propertyfilter.h>
#include <cartobase/type/typetraits.h>
#include <map>
#include <set>


namespace carto
{


class Headered : virtual public RCObject, public Observable, public Observer
{

  public:

    Headered();
    Headered( const Headered& other );
    virtual ~Headered();

    Headered& operator = ( const Headered& other );

    const PropertySet& header() const;
    PropertySet& header();

    void addPropertyFilter( const rc_ptr< PropertyFilter >& propertyFilter );
    bool hasPropertyFilter( const std::string& propertyFilterName ) const;
    bool connect( const std::string& propertyFilterName,
                  const PropertyFilter::Slot& slot );
    bool disconnect( const std::string& propertyFilterName,
                     const PropertyFilter::Slot& slot );

    std::set< rc_ptr< PropertyFilter > >
      getPropertyFilters( const std::string& propertyName ) const;

    void delay();
    void flush();
    void blockSignals( bool );
    bool signalsBlocked() const;

    virtual void initialize();

    /// retreive or generate an UUID - this method intentionally is non-const
    std::string uuid();
    /** take the UUID from another header and set it to this.
        \return false if the source had no UUID
    */
    bool copyUuid( const PropertySet & );
    bool copyUuid( const Headered & );

  private:

    void slotPropertyChanged( const Object& sender, 
                              const std::string& propertyName,
                              const Object& oldValue );

    PropertySet _propertySet;
    bool _delayed;
    bool _blocked;
    std::set< rc_ptr< PropertyFilter > > _propertyFilters;
    std::set< rc_ptr< PropertyFilter > > _delayedEmissions;

};


  template <typename T> inline
  carto::Object getObjectHeader( Headered & h )
  {
    return Object::reference( h.header() );
  }

}


#endif
