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

#include <aims/listview/orderinglist.h>
#include <qpushbutton.h>
#include <QBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>

using namespace aims;
using namespace aims::gui;

struct QOrderingListWidget::Private
{
  QListWidget	*listwidget;
};


QOrderingListWidget::QOrderingListWidget( QWidget* parent, const char* name, 
                                          Qt::WindowFlags f )
  : QWidget( parent, f ), d( new Private )
{
  QHBoxLayout *lay = new QHBoxLayout( this );
  setLayout( lay );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->setSpacing( 10 );
  setObjectName( name );
  d->listwidget = new QListWidget( this );
  lay->addWidget( d->listwidget );
  d->listwidget->setSelectionMode( QListWidget::SingleSelection );
  QWidget *vb = new QWidget( this );
  lay->addWidget( vb );
  QVBoxLayout *vlay = new QVBoxLayout( vb );
  vb->setLayout( vlay );
  vlay->setContentsMargins( 0, 0, 0, 0 );
  QPushButton	*pbu = new QPushButton( tr( "Up" ), vb );
  vlay->addWidget( pbu );
  QPushButton	*pbd = new QPushButton( tr( "Down" ), vb );
  vlay->addWidget( pbd );

  connect( pbu, SIGNAL( clicked() ), SLOT( up() ) );
  connect( pbd, SIGNAL( clicked() ), SLOT( down() ) );
}


QOrderingListWidget::~QOrderingListWidget()
{
  delete d;
}


QListWidget *QOrderingListWidget::qListWidget()
{
  return d->listwidget;
}


const QListWidget *QOrderingListWidget::qListWidget() const
{
  return d->listwidget;
}


void QOrderingListWidget::up()
{
  int	i, n = d->listwidget->count(), ind = -1;
  QListWidgetItem	*sel = 0;
  for( i=0; i<n; ++i )
    if( d->listwidget->item(i)->isSelected() )
    {
      sel = d->listwidget->item( i );
      ind = i;
      break;
    }
  if( sel )
  {
    if( ind > 0 )
    {
      d->listwidget->takeItem( ind );
      d->listwidget->insertItem( ind - 1, sel );
      sel->setSelected( true );
    }
  }
}

void QOrderingListWidget::down()
{
  int	i, n = d->listwidget->count(), ind = -1;
  QListWidgetItem	*sel = 0;
  for( i=0; i<n; ++i )
    if( d->listwidget->item(i)->isSelected() )
    {
      sel = d->listwidget->item( i );
      ind = i;
      break;
    }
  if( sel )
  {
    if( ind < d->listwidget->count() )
    {
      d->listwidget->takeItem( ind );
      d->listwidget->insertItem( ind + 1, sel );
      sel->setSelected( true );
    }
  }
}


