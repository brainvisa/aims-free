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


#include <aims/listview/qalistview.h>
#if QT_VERSION >= 0x040000
#include <q3header.h>
#else
#include <qheader.h>
#endif
#include <iostream>

using namespace aims::gui;
using namespace std;


struct QAListView::QAListViewPrivate
{
  QAListViewPrivate();
  bool		dragpossible;
  QPoint	dragstartpos;
  Qt::MouseButtons buttons;
  Qt::KeyboardModifiers deadkeys;
  Q3ListViewItem *itemUnderCursor;
};


QAListView::QAListViewPrivate::QAListViewPrivate() : dragpossible( false ),
  itemUnderCursor( 0 )
{
}


// ----

QAListView::QAListView( QWidget *parent, const char* name )
  : Q3ListView( parent, name ), d( new QAListViewPrivate )
{
  // clearWFlags( WNorthWestGravity | WRepaintNoErase );
}


QAListView::~QAListView()
{
  delete d;
}


void QAListView::contentsMousePressEvent( QMouseEvent* mev )
{
  /*cout << "QAListView::contentsMousePressEvent: pos : " << mev->pos().x() 
    << ", " << mev->pos().y() << endl;*/
  d->dragpossible = false;
  /*if( mev->button() == LeftButton && mev->state() == NoButton )
    {*/
  QPoint p( contentsToViewport( mev->pos() ) );
  Q3ListViewItem	*item = itemAt( p );
  if( item && item->isSelected() )
    {
      // if the user clicked into the root decoration of the item, don't 
      // try to start a drag!
      if( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
	  treeStepSize() 
	  * ( item->depth() + ( rootIsDecorated() ? 1 : 0) ) 
	  + itemMargin() ||
	  p.x() < header()->cellPos( header()->mapToActual( 0 ) ) )
	{
	  d->dragpossible = true;
          d->dragstartpos = mev->pos();
	}
    }
  //}
  /*cout << "contentsMousePressEvent - dragpossible : " << d->dragpossible 
    << endl;*/
#if QT_VERSION > 0x040000
  d->buttons = mev->buttons();
  d->deadkeys = mev->modifiers();
#else
  d->buttons = mev->button();
  d->deadkeys = mev->state();
#endif
  Q3ListView::contentsMousePressEvent( mev );
}


void QAListView::contentsMouseMoveEvent( QMouseEvent* mev )
{
  /* cout << "contentsMouseMoveEvent - button : " << (int) mev->button()
       << ", state : " << (int) mev->state() << ", dragposs : " 
       << d->dragpossible << endl; */
  /*if( mev->button() == NoButton && mev->state() == LeftButton 
    && d->dragpossible )*/
  //if( mev->buttons() == Qt::NoButton )
  {
    QPoint p( contentsToViewport( mev->pos() ) );
    Q3ListViewItem        *item = itemAt( p );
    if( d->itemUnderCursor != item )
    {
      d->itemUnderCursor = item;
      emit cursorMoved( item, 0 ); // TODO: not always column 0...
    }
  }
  if( d->dragpossible )
    {
      QPoint	p = mev->pos() - d->dragstartpos;
      if( p.x() * p.x() + p.y() * p.y() >= 10 )
        {
          QPoint p( contentsToViewport( mev->pos() ) );
          Q3ListViewItem	*item = itemAt( p );
          if( item && item->isSelected() )
            {
              d->dragpossible = false;
              emit dragStart( item, mev->state() );
              mev->ignore();
              return;
            }
          d->dragpossible = false;
        }
    }
  Q3ListView::contentsMouseMoveEvent( mev );
  mev->ignore();
}


void QAListView::leaveEvent( QEvent* ev )
{
  if( hasMouseTracking() )
    emit cursorMoved( 0, 0 );
}


void QAListView::contentsMouseReleaseEvent( QMouseEvent* mev )
{
  d->dragpossible = false;
  // d->buttons = 0;
  // d->deadkeys = 0;
  Q3ListView::contentsMouseReleaseEvent( mev );
}


void QAListView::unselectInvisibleItems( Q3ListView* lv )
{
  Q3ListViewItemIterator	it( lv );
  Q3ListViewItem		*cur, *par;

  lv->blockSignals( true );
  while( it.current() )
    {
      cur = it.current();
      for( par=cur->parent(); par && par->isOpen(); par=par->parent() ) {}
      if( par && !par->isOpen() )
        cur->setSelected( false );
      ++it;
    }
  lv->blockSignals( false );
}


void QAListView::unselectInvisibleItems()
{
  unselectInvisibleItems( this );
}


Qt::MouseButtons QAListView::buttonsAtLastEvent() const
{
  return d->buttons;
}


Qt::KeyboardModifiers QAListView::deadKeysStateAtLastEvent() const
{
  return d->deadkeys;
}


void QAListView::_itemStartsRename( Q3ListViewItem* item, int col )
{
  emit itemStartsRename( item, col );
}


void QAListView::_itemCancelsRename( Q3ListViewItem* item, int col )
{
  emit itemCancelsRename( item, col );
}

