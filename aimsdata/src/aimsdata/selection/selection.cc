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

#include <aims/selection/selection.h>
#include <aims/io/selectionr.h>
#include <aims/io/selectionw.h>

using aims::Selection;
using aims::SelectionSet;
using namespace std;


Selection::Selection()
{
}


Selection::Selection( const string & selname )
  : my_name( selname )
{
}


string Selection::name() const
{
  return my_name;
}


void Selection::select(const std::string& label)
{
  iterator	i, e = end();
  for( i=begin(); i!=e && *i!=label; ++i ) {}
  if( i == e )
    my_labels.push_back(label);
}


void Selection::selectQuick(const std::string& label)
{
  my_labels.push_back(label);
}


void Selection::unselect(const std::string& label)
{
  iterator	i, e = end();
  for( i=begin(); i!=e && *i!=label; ++i ) {}
  if( i != e )
    my_labels.erase( i );
}


Selection::const_iterator Selection::find( const std::string& label ) const
{
  const_iterator	i, e = end();
  for( i=begin(); i!=e && *i!=label; ++i ) {}
  return e;
}


Selection::iterator Selection::find( const std::string& label )
{
  iterator	i, e = end();
  for( i=begin(); i!=e && *i!=label; ++i ) {}
  return e;
}


bool Selection::empty() const
{
  return my_labels.empty();
}


SelectionSet::const_iterator 
SelectionSet::findSelection( const Selection & sel ) const
{
  const_iterator	i, e = end();
  for( i=begin(); i!=e && &*i!=&sel; ++i ) {}
  return i;
}


SelectionSet::iterator 
SelectionSet::findSelection( const Selection & sel )
{
  iterator	i, e = end();
  for( i=begin(); i!=e && &*i!=&sel; ++i ) {}
  return i;
}


SelectionSet::const_iterator 
SelectionSet::findSelection( const string & name ) const
{
  const_iterator	i, e = end();
  for( i=begin(); i!=e && i->name()!=name; ++i ) {}
  return i;
}


SelectionSet::iterator 
SelectionSet::findSelection( const string & name )
{
  iterator	i, e = end();
  for( i=begin(); i!=e && i->name()!=name; ++i ) {}
  return i;
}


bool SelectionSet::empty() const
{
  return my_selections.empty();
}


void SelectionSet::addSelection( const Selection& sel, bool enabledup )
{
  if( enabledup )
    my_selections.push_back( sel );
  else
    {
      iterator	i = findSelection( sel.name() );
      if( i == end() )
        my_selections.push_back( sel );
      else
        {
          Selection::const_iterator	is, es = sel.end();
          for( is=sel.begin(); is!=es; ++is )
            i->select( *is );
        }
    }
}


void SelectionSet::deleteSelection( Selection* sel )
{
  iterator	i, e = end();
  string	name = sel->name();
  for( i=begin(); i!=e && i->name()!=name; ++i ) {}
  if( i != e )
    my_selections.erase(i);
}


void SelectionSet::deleteSelection( iterator i )
{
	my_selections.erase(i);
}


void SelectionSet::load( const string & filename )
{
  my_selections.clear();
  append( filename );
}


void SelectionSet::append( const string & filename )
{
  SelectionReader	sr( filename );
  sr.read( *this );
}


void SelectionSet::save( const string & filename ) const
{
  SelectionWriter	sw( filename );
  sw.write( *this );
}


