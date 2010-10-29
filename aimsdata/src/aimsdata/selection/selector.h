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

#ifndef AIMS_SELECTION_SELECTOR_H
#define AIMS_SELECTION_SELECTOR_H

#include <string>
#include <iostream>

class Graph;

namespace aims
{
  class Selection;
  class SelectionSet;
  class Hierarchy;

  class Selector
  {
  public:
    Selector();
    virtual ~Selector();

    Selection query( const Selection &, const Hierarchy & ) const;

    const Hierarchy* nomenclature() const;
    void setNomenclature( Hierarchy* hie, bool setowner = true );
    void loadNomenclature( const std::string & filename );
    const Graph* model() const;
    void setModel( Graph* model, bool setowner = true );
    void loadModel( const std::string & filename );
    void loadPreSelection( const std::string & filename );
    void setPreSelection( SelectionSet* sel );
    void printSelection( std::ostream & ostr = std::cout ) const;
    SelectionSet selection() const;
    void clearSelection();

  private:
    struct Private;
    Private *d;
  };


  class SelectionExpander
  {
  public:
    SelectionExpander( bool enabledups = false );
    ~SelectionExpander();

    void setDuplicatesEnabled( bool );
    bool duplicatesEnabled() const;

    SelectionSet query( const SelectionSet &, const Hierarchy & ) const;
    /// selects labelled model nodes
    SelectionSet query( const Graph & model ) const;
    SelectionSet query( const Graph & model, const Hierarchy & ) const;
    SelectionSet query( const Graph & model, const SelectionSet & ) const;
    SelectionSet query( const Graph & model, const SelectionSet &, 
                        const Hierarchy & ) const;
    /** Most generic function (accepts null pointers).
        The difference between query() and check() functions is that query 
        tries to make the richest selection by populating them with the 
        hierarchy if provided. check() only eliminates selections not 
        conforming to the model (if given) */
    SelectionSet query( const Graph *model, const SelectionSet *, 
                        const Hierarchy * ) const;
    SelectionSet check( const Graph *model, const SelectionSet *, 
                        const Hierarchy * ) const;
    void removeDuplicates( SelectionSet &, const Hierarchy & ) const;
    void merge( SelectionSet & dst, const SelectionSet & toadd ) const;
    void merge( SelectionSet & dst, const SelectionSet & toadd, 
                const Graph *mode, const Hierarchy *nom ) const;

  private:
    bool	_enabledup;
  };

}


#endif

