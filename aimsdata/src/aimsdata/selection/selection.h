/* Copyright (c) 1995-2005 CEA
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

#ifndef AIMS_SELECTION_SELECTION_H
#define AIMS_SELECTION_SELECTION_H


#include <string>
#include <list>


namespace aims
{

  class Selection
  {
  public:
    typedef std::list<std::string>::value_type value_type;
    typedef std::list<std::string>::pointer pointer;
    typedef std::list<std::string>::const_pointer const_pointer;
    typedef std::list<std::string>::reference reference;
    typedef std::list<std::string>::const_reference const_reference;

    typedef std::list<std::string>::difference_type difference_type;
    typedef std::list<std::string>::size_type size_type;

    typedef std::list<std::string>::iterator iterator;
    typedef std::list<std::string>::const_iterator const_iterator;
    typedef std::list<std::string>::reverse_iterator reverse_iterator;
    typedef std::list<std::string>::const_reverse_iterator 
    const_reverse_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    Selection();
    Selection( const std::string & selname );
    void select( const std::string& label );
    void selectQuick( const std::string& label );
    void unselect( const std::string& label );
    iterator find( const std::string & label );
    const_iterator find( const std::string & label ) const;
    bool empty() const;
    std::string name() const;

  private:
    std::list<std::string> my_labels;
    std::string my_name;
  };


  class SelectionSet
  {
  public:
    typedef std::list<Selection>::value_type value_type;
    typedef std::list<Selection>::pointer pointer;
    typedef std::list<Selection>::const_pointer const_pointer;
    typedef std::list<Selection>::reference reference;
    typedef std::list<Selection>::const_reference const_reference;

    typedef std::list<Selection>::difference_type difference_type;
    typedef std::list<Selection>::size_type size_type;

    typedef std::list<Selection>::iterator iterator;
    typedef std::list<Selection>::const_iterator const_iterator;
    typedef std::list<Selection>::reverse_iterator reverse_iterator;
    typedef std::list<Selection>::const_reverse_iterator 
    const_reverse_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    void addSelection( const Selection& selection, bool enabledups = false );
    void deleteSelection( Selection* );
    void deleteSelection( iterator i );
    void load( const std::string & filename );
    void append( const std::string & filename );
    void save( const std::string & filename ) const;
    const_iterator findSelection( const Selection & ) const;
    iterator findSelection( const Selection & );
    iterator findSelection( const std::string & );
    const_iterator findSelection( const std::string & ) const;
    bool empty() const;

  private:
    std::list<Selection> my_selections;
  };


} // namespace aims


inline
aims::Selection::iterator aims::Selection::begin()
{
	return my_labels.begin();
}


inline
aims::Selection::iterator aims::Selection::end()
{
	return my_labels.end();
}


inline
aims::Selection::const_iterator aims::Selection::begin() const
{
	return my_labels.begin();
}


inline
aims::Selection::const_iterator aims::Selection::end() const
{
	return my_labels.end();
}


inline
aims::Selection::reverse_iterator aims::Selection::rbegin()
{
	return my_labels.rbegin();
}


inline
aims::Selection::reverse_iterator aims::Selection::rend()
{
	return my_labels.rend();
}


inline
aims::Selection::const_reverse_iterator aims::Selection::rbegin() const
{
	return my_labels.rbegin();
}


inline
aims::Selection::const_reverse_iterator aims::Selection::rend() const
{
	return my_labels.rend();
}


inline
aims::SelectionSet::iterator aims::SelectionSet::begin()
{
	return my_selections.begin();
}


inline
aims::SelectionSet::iterator aims::SelectionSet::end()
{
	return my_selections.end();
}


inline
aims::SelectionSet::const_iterator aims::SelectionSet::begin() const
{
	return my_selections.begin();
}


inline
aims::SelectionSet::const_iterator aims::SelectionSet::end() const
{
	return my_selections.end();
}


inline
aims::SelectionSet::reverse_iterator aims::SelectionSet::rbegin()
{
	return my_selections.rbegin();
}


inline
aims::SelectionSet::reverse_iterator aims::SelectionSet::rend()
{
	return my_selections.rend();
}


inline
aims::SelectionSet::const_reverse_iterator aims::SelectionSet::rbegin() const
{
	return my_selections.rbegin();
}


inline
aims::SelectionSet::const_reverse_iterator aims::SelectionSet::rend() const
{
	return my_selections.rend();
}


#endif
