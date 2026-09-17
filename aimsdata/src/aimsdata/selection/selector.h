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

