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


#include <aims/listview/editablelistviewitem.h>
#include <aims/listview/qalistview.h>

using namespace aims::gui;


QEditableListViewItem::QEditableListViewItem( Q3ListView * parent )
  : Q3ListViewItem( parent )
{
}


QEditableListViewItem::QEditableListViewItem(
    Q3ListViewItem * parent )
  : Q3ListViewItem( parent )
{
}


QEditableListViewItem::QEditableListViewItem( Q3ListView * parent,
    Q3ListViewItem * after )
  : Q3ListViewItem( parent, after )
{
}


QEditableListViewItem::QEditableListViewItem(
    Q3ListViewItem * parent, Q3ListViewItem * after )
  : Q3ListViewItem( parent, after )
{
}


QEditableListViewItem::QEditableListViewItem(
    Q3ListView * parent, QString label1, QString label2, QString label3,
    QString label4, QString label5, QString label6, QString label7,
    QString label8 )
  : Q3ListViewItem( parent, label1, label2, label3, label4, label5, label6,
                    label7, label8 )
{
}


QEditableListViewItem::QEditableListViewItem(
    Q3ListViewItem * parent, QString label1, QString label2, QString label3,
    QString label4, QString label5, QString label6, QString label7,
    QString label8 )
  : Q3ListViewItem( parent, label1, label2, label3, label4, label5, label6,
                    label7, label8 )
{
}


QEditableListViewItem::QEditableListViewItem(
    Q3ListView * parent, Q3ListViewItem * after, QString label1,
    QString label2, QString label3, QString label4, QString label5,
    QString label6, QString label7, QString label8 )
  : Q3ListViewItem( parent, after, label1, label2, label3, label4, label5,
                    label6, label7, label8 )
{
}


QEditableListViewItem::QEditableListViewItem(
    Q3ListViewItem * parent, Q3ListViewItem * after, QString label1,
    QString label2, QString label3, QString label4, QString label5,
    QString label6, QString label7, QString label8 )
  : Q3ListViewItem( parent, after, label1, label2, label3, label4, label5,
                    label6, label7, label8 )
{
}


QEditableListViewItem::~QEditableListViewItem()
{
}


void QEditableListViewItem::startRename( int col )
{
  Q3ListViewItem::startRename( col );
  QAListView *parent = dynamic_cast<QAListView *>( listView() );
  if( parent )
    parent->_itemStartsRename( this, col );
}


void QEditableListViewItem::cancelRename( int col )
{
  Q3ListViewItem::cancelRename( col );
  QAListView *parent = dynamic_cast<QAListView *>( listView() );
  if( parent )
    parent->_itemCancelsRename( this, col );
}

