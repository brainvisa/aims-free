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


#ifndef AIMSGUI_LISTVIEW_EDITABLELISTVIEWITEM_H
#define AIMSGUI_LISTVIEW_EDITABLELISTVIEWITEM_H


#include <aims/qtcompat/qlistview.h>

namespace aims
{

namespace gui
{

/* A QListViewItem that emits its parent QAListView signals when edited, and
   when edition is either validated or cancelled, so that the owner can setup
   and cleanup things upon edition.
*/
class QEditableListViewItem : public Q3ListViewItem
{
public:
  QEditableListViewItem( Q3ListView * parent );
  QEditableListViewItem( Q3ListViewItem * parent );
  QEditableListViewItem( Q3ListView * parent, Q3ListViewItem * after );
  QEditableListViewItem( Q3ListViewItem * parent, Q3ListViewItem * after );
  QEditableListViewItem( Q3ListView * parent, QString label1,
                         QString label2 = QString::null,
                         QString label3 = QString::null,
                         QString label4 = QString::null,
                         QString label5 = QString::null,
                         QString label6 = QString::null,
                         QString label7 = QString::null,
                         QString label8 = QString::null );
  QEditableListViewItem( Q3ListViewItem * parent, QString label1,
                         QString label2 = QString::null,
                         QString label3 = QString::null,
                         QString label4 = QString::null,
                         QString label5 = QString::null,
                         QString label6 = QString::null,
                         QString label7 = QString::null,
                         QString label8 = QString::null );
  QEditableListViewItem( Q3ListView * parent, Q3ListViewItem * after,
                         QString label1, QString label2 = QString::null,
                         QString label3 = QString::null,
                         QString label4 = QString::null,
                         QString label5 = QString::null,
                         QString label6 = QString::null,
                         QString label7 = QString::null,
                         QString label8 = QString::null );
  QEditableListViewItem( Q3ListViewItem * parent, Q3ListViewItem * after,
                         QString label1, QString label2 = QString::null,
                         QString label3 = QString::null,
                         QString label4 = QString::null,
                         QString label5 = QString::null,
                         QString label6 = QString::null,
                         QString label7 = QString::null,
                         QString label8 = QString::null );
  virtual ~QEditableListViewItem();

  virtual void startRename( int col );
  /** this method is promoted to public API since it sometimes needs to be
      called from outside */
  virtual void cancelRename( int col );

protected:
};

}

}

#endif

