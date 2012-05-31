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

#ifndef AIMS_LISTVIEW_QALISTVIEW_H
#define AIMS_LISTVIEW_QALISTVIEW_H

#include <qglobal.h>
#include <aims/qtcompat/qlistview.h>
#include <aims/qtcompat/qmouseevent.h>

namespace aims
{

namespace gui
{

  /**	Improved QListView with better selection handling.
	Handles drag & drop
  */
  class QAListView : public Q3ListView
  {
    Q_OBJECT

  public:
    QAListView( QWidget* parent = 0, const char* name = 0 );
    virtual ~QAListView();

    /// This function can be called from a slot
    static void unselectInvisibleItems( Q3ListView* lv );
    /// mouse buttons pressed at last event. Qt doesn't provide it
    Qt::MouseButtons buttonsAtLastEvent() const;
    /// dead keys pressed at last event. Qt doesn't provide it
    Qt::KeyboardModifiers deadKeysStateAtLastEvent() const;

  public slots:
    void unselectInvisibleItems();

  signals:
    void dragStart( Q3ListViewItem*, Qt::ButtonState state );
    /** notifies the beginning of a child renaming.
        Warning: this signal is only emitted for QEditableListViewItem
        children
    */
    void itemStartsRename( Q3ListViewItem*, int col );
    /** notifies the abort of a child renaming.
    Warning: this signal is only emitted for QEditableListViewItem
    children
     */
    void itemCancelsRename( Q3ListViewItem*, int col );
    /** emitted when the mouse cursor moves over a different element.
    Needs so set mouse tracking on the QListView before it works.
     */
    void cursorMoved( Q3ListViewItem*, int col );

  protected:
    virtual void contentsMousePressEvent( QMouseEvent* mev );
    virtual void contentsMouseMoveEvent( QMouseEvent* mev );
    virtual void contentsMouseReleaseEvent( QMouseEvent* mev );
    virtual void leaveEvent( QEvent* ev );

  private:
    friend class QEditableListViewItem;

    struct QAListViewPrivate;

    void _itemStartsRename( Q3ListViewItem*, int col );
    void _itemCancelsRename( Q3ListViewItem*, int col );

    QAListViewPrivate	*d;
  };

}

}

#endif
