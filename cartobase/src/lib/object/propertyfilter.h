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

#ifndef CARTOBASE_OBJECT_PROPERTYFILTER_H
#define CARTOBASE_OBJECT_PROPERTYFILTER_H


#include <cartobase/smart/rcobject.h>
#include <cartobase/object/object.h>
#include <cartobase/sigc++/sigc++.h>
#include <string>
#include <set>
#include <map>


namespace carto
{


class PropertyFilter : public RCObject
{

  public:

    typedef ::sigc::signal1< void, const PropertyFilter& > Signal;
    typedef ::sigc::slot1< void, const PropertyFilter& > Slot;

    PropertyFilter( const std::string& name,
                    const std::set< std::string >& propertyNames );
    PropertyFilter( const std::string& name,
                    const std::string& propertyName );
    virtual ~PropertyFilter();


    const std::string& getName() const;
    Signal& getSignal();

    bool hasProperty( const std::string& propertyName ) const;
    bool hasOldValue( const std::string& propertyName ) const;

    void setOldValue( const std::string& propertyName,
                      const Object& oldValue );
    Object getOldValue( const std::string& propertyName ) const;

    void emitsignal();


  protected:

    std::string _name;
    std::set< std::string > _propertyNames;
    std::map< std::string, Object > _oldValues;
    Signal _signal;

};


}


#endif
