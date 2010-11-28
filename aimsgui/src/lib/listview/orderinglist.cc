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
#include <aims/qtcompat/qlistbox.h>
#include <aims/qtcompat/qvbox.h>

using namespace aims;
using namespace aims::gui;

struct QOrderingListBox::Private
{
  QListBox	*listbox;
};


QOrderingListBox::QOrderingListBox( QWidget* parent, const char* name, 
                                    Qt::WFlags f )
  : QHBox( parent, name, f ), d( new Private )
{
  setSpacing( 10 );
  d->listbox = new QListBox( this );
  d->listbox->setSelectionMode( QListBox::Single );
  QVBox	*vb = new QVBox( this );
  QPushButton	*pbu = new QPushButton( tr( "Up" ), vb );
  QPushButton	*pbd = new QPushButton( tr( "Down" ), vb );

  connect( pbu, SIGNAL( clicked() ), SLOT( up() ) );
  connect( pbd, SIGNAL( clicked() ), SLOT( down() ) );
}


QOrderingListBox::~QOrderingListBox()
{
  delete d;
}


QListBox *QOrderingListBox::qListBox()
{
  return d->listbox;
}


const QListBox *QOrderingListBox::qListBox() const
{
  return d->listbox;
}


void QOrderingListBox::up()
{
  unsigned	i, n = d->listbox->count();
  QListBoxItem	*sel = 0;
  for( i=0; i<n; ++i )
    if( d->listbox->isSelected( i ) )
      {
        sel = d->listbox->item( i );
        break;
      }
  if( sel )
    {
      int	ind = d->listbox->index( sel );
      if( ind > 0 )
        {
          d->listbox->setSelected( sel, false );
          d->listbox->takeItem( sel );
          d->listbox->insertItem( sel, ind - 1 );
          d->listbox->setSelected( sel, true );
        }
    }
}

void QOrderingListBox::down()
{
  unsigned	i, n = d->listbox->count();
  QListBoxItem	*sel = 0;
  for( i=0; i<n; ++i )
    if( d->listbox->isSelected( i ) )
      {
        sel = d->listbox->item( i );
        break;
      }
  if( sel )
    {
      int	ind = d->listbox->index( sel );
      if( (unsigned) ind < d->listbox->count() )
        {
          d->listbox->setSelected( sel, false );
          d->listbox->takeItem( sel );
          d->listbox->insertItem( sel, ind + 1 );
          d->listbox->setSelected( sel, true );
        }
    }
}


