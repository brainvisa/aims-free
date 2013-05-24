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

#include <aims/listview/qatreewidget.h>
#include <QTreeWidgetItem>
#include <QMouseEvent>

using namespace aims;
using namespace aims::gui;


struct QATreeWidget::Private
{
  Private();
  bool          dragpossible;
  QPoint        dragstartpos;
  Qt::MouseButtons buttons;
  Qt::KeyboardModifiers deadkeys;
  QTreeWidgetItem *itemUnderCursor;
};


QATreeWidget::Private::Private() : dragpossible( false ),
  itemUnderCursor( 0 )
{
}

// ---

QATreeWidget::QATreeWidget( QWidget* parent )
  : QTreeWidget( parent ), d( new Private )
{
}


QATreeWidget::~QATreeWidget()
{
  delete d;
}


bool QATreeWidget::edit( const QModelIndex & index, EditTrigger trigger, 
                          QEvent* event )
{
  if( index.column() != 2 )
    return false;
  return QTreeWidget::edit( index, trigger, event );
}


void QATreeWidget::mousePressEvent( QMouseEvent* event )
{
  if( event->button() == Qt::RightButton )
  {
    event->accept();
    QTreeWidgetItem* item = itemAt( event->pos() );
    if( item )
      emit itemRightPressed( item, event->pos() );
  }
  else
  {
    d->dragpossible = false;
    QTreeWidgetItem *item = itemAt( event->pos() );
    if( item && item->isSelected() )
    {
      // if the user clicked into the root decoration of the item, don't 
      // try to start a drag!
//       if( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
//           treeStepSize() 
//           * ( item->depth() + ( rootIsDecorated() ? 1 : 0) ) 
//           + itemMargin() ||
//           p.x() < header()->cellPos( header()->mapToActual( 0 ) ) )
//         {
          d->dragpossible = true;
          d->dragstartpos = event->pos();
//         }
    }
    d->buttons = event->buttons();
    d->deadkeys = event->modifiers();

    QTreeWidget::mousePressEvent( event );
  }
}


void QATreeWidget::mouseMoveEvent( QMouseEvent* event )
{
  QPoint p = event->pos();
  QTreeWidgetItem *item = itemAt( p );
  if( d->itemUnderCursor != item )
  {
    d->itemUnderCursor = item;
    emit cursorMoved( item, 0 ); // TODO: not always column 0...
  }

  if( d->dragpossible )
  {
    QPoint    pd = event->pos() - d->dragstartpos;
    if( pd.x() * pd.x() + pd.y() * pd.y() >= 10 )
    {
      QTreeWidgetItem *item = itemAt( p );
      if( item && item->isSelected() )
      {
        d->dragpossible = false;
        emit dragStart( item, event->buttons(), event->modifiers() );
        event->ignore();
        return;
      }
      d->dragpossible = false;
    }
  }
  QTreeWidget::mouseMoveEvent( event );
}


void QATreeWidget::mouseReleaseEvent( QMouseEvent* event )
{
  d->dragpossible = false;
  QTreeWidget::mouseReleaseEvent( event );
}


void QATreeWidget::leaveEvent( QEvent* ev )
{
  if( hasMouseTracking() )
    emit cursorMoved( 0, 0 );
}


Qt::MouseButtons QATreeWidget::buttonsAtLastEvent() const
{
  return d->buttons;
}


Qt::KeyboardModifiers QATreeWidget::deadKeysStateAtLastEvent() const
{
  return d->deadkeys;
}


