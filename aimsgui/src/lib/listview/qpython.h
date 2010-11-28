/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
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

#ifndef AIMS_LISTVIEW_QPYTHON_H
#define AIMS_LISTVIEW_QPYTHON_H

#include <cartobase/object/object.h>
#include <cartobase/object/syntax.h>
#include <qglobal.h>
#if QT_VERSION >= 0x040000
class Q3ListView;
class Q3ListViewItem;
#else
class QListView;
class QListViewItem;
typedef QListView Q3ListView;
typedef QListViewItem Q3ListViewItem;
#endif
class QString;


namespace aims
{

namespace gui
{

  /**	Prints elements and attributes of a GenericObject (Python-like) object 
	into a QListView tree
  */
  class QPythonPrinter
  {
  public:
    typedef Q3ListViewItem* (*Helper)( const carto::GenericObject & obj, 
				       Q3ListViewItem* parent, 
				       QPythonPrinter & qp, 
				       const QString & attname, 
				       const QString & attype, 
				       bool viewinternal );
    typedef std::map<std::string, Helper>	HelperSet;

    QPythonPrinter( Q3ListViewItem* parent, 
		    const carto::SyntaxSet & syntax = carto::SyntaxSet(), 
		    const HelperSet & helpers = HelperSet() );
    QPythonPrinter( Q3ListView* parent, 
		    const carto::SyntaxSet & syntax = carto::SyntaxSet(), 
		    const HelperSet & helpers = HelperSet() );
    virtual ~QPythonPrinter();

    virtual Q3ListViewItem* write( const carto::GenericObject & object, 
				   bool writeInternals = false );
    virtual void write( const carto::GenericObject & object, 
			Q3ListViewItem* parent, const std::string & syntax, 
			const std::string & semantic, bool writeInternals );
    bool isInternal( const std::string & syntax, 
		     const std::string & semantic );
    int valueColumn() const { return( _valcol ); }
    int attributeColumn() const { return( _attcol ); }
    int typeColumn() const { return( _typcol ); }
    void setValueColumn( int c ) { _valcol = c; }
    void setPropertyColumn( int c ) { _attcol = c; }
    void setTypeColumn( int c ) { _typcol = c; }

  protected:
    virtual void initHelpers();

    carto::SyntaxSet	_rules;
    HelperSet		_helpers;
    Q3ListView		*_lview;
    Q3ListViewItem	*_lvitem;
    int			_valcol;
    int			_attcol;
    int			_typcol;
  };

}

}

#endif

