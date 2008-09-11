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

#include <aims/qtcompat/qtoolbutton.h>
#include <qtoolbar.h>


#if QT_VERSION >= 0x040000
Q34ToolButton::Q34ToolButton( const QIcon & s, 
                              const QString & textLabel, 
                              const QString & grouptext, QObject * receiver, 
                              const char * slot, QToolBar * parent, 
                              const char * name ) 
  : QToolButton( s, textLabel, grouptext, receiver, slot, parent, name )
{
  // taken fom QToolBar code
  // there is apparently no simple way to do this in Qt 4...
  setAutoRaise( true );
  setIconSize( parent->iconSize() );
  setFocusPolicy( Qt::NoFocus );
  setToolButtonStyle( parent->toolButtonStyle() );
  connect( parent, SIGNAL( iconSizeChanged( QSize ) ),
           this, SLOT( setIconSize( QSize ) ) );
  connect( parent, SIGNAL( toolButtonStyleChanged( Qt::ToolButtonStyle ) ),
           this, SLOT( setToolButtonStyle( Qt::ToolButtonStyle ) ) );
  parent->addWidget( this );
}


Q34ToolButton::Q34ToolButton( QToolBar * parent )
  : QToolButton( parent )
{
  // taken fom QToolBar code
  // there is apparently no simple way to do this in Qt 4...
  setAutoRaise( true );
  setIconSize( parent->iconSize() );
  setFocusPolicy( Qt::NoFocus );
  setToolButtonStyle( parent->toolButtonStyle() );
  connect( parent, SIGNAL( iconSizeChanged( QSize ) ),
           this, SLOT( setIconSize( QSize ) ) );
  connect( parent, SIGNAL( toolButtonStyleChanged( Qt::ToolButtonStyle ) ),
           this, SLOT( setToolButtonStyle( Qt::ToolButtonStyle ) ) );
  parent->addWidget( this );
}

#else // Qt 3

Q34ToolButton::Q34ToolButton( const QIconSet & s, 
                              const QString & textLabel, 
                              const QString & grouptext, QObject * receiver, 
                              const char * slot, QToolBar * parent, 
                              const char * name ) 
  : QToolButton( s, textLabel, grouptext, receiver, slot, parent, name )
{
}


Q34ToolButton::Q34ToolButton( QToolBar * parent )
  : QToolButton( parent )
{
}

#endif


Q34ToolButton::~Q34ToolButton()
{
}

// -----

#if QT_VERSION >= 0x040000
QToolButtonInt::QToolButtonInt( int par, const QIcon & s, 
				const QString & textLabel, 
				const QString & grouptext, QObject * receiver, 
				const char * slot, QToolBar * parent, 
				const char * name ) 
  : Q34ToolButton( s, textLabel, grouptext, 0, 0, parent, name ), param( par )
{
  connect( this, SIGNAL( clicked() ), this, SLOT( propagateSignal() ) );
  if( receiver && slot )
    connect( this, SIGNAL( activated( int ) ), receiver, slot );
}


QToolButtonInt::QToolButtonInt( int par, QToolBar * parent )
  : Q34ToolButton( parent ), param( par )
{
  connect( this, SIGNAL( clicked() ), this, SLOT( propagateSignal() ) );
  // taken fom QToolBar code
  // there is apparently no simple way to do this in Qt 4...
  setAutoRaise( true );
  setIconSize( parent->iconSize() );
  setFocusPolicy( Qt::NoFocus );
  setToolButtonStyle( parent->toolButtonStyle() );
  connect( parent, SIGNAL( iconSizeChanged( QSize ) ),
           this, SLOT( setIconSize( QSize ) ) );
  connect( parent, SIGNAL( toolButtonStyleChanged( Qt::ToolButtonStyle ) ),
           this, SLOT( setToolButtonStyle( Qt::ToolButtonStyle ) ) );
}

#else // Qt 3

QToolButtonInt::QToolButtonInt( int par, const QIconSet & s, 
				const QString & textLabel, 
				const QString & grouptext, QObject * receiver, 
				const char * slot, QToolBar * parent, 
				const char * name ) 
  : Q34ToolButton( s, textLabel, grouptext, 0, 0, parent, name ), param( par )
{
  connect( this, SIGNAL( clicked() ), this, SLOT( propagateSignal() ) );
  if( receiver && slot )
    connect( this, SIGNAL( activated( int ) ), receiver, slot );
}


QToolButtonInt::QToolButtonInt( int par, QToolBar * parent )
  : Q34ToolButton( parent ), param( par )
{
  connect( this, SIGNAL( clicked() ), this, SLOT( propagateSignal() ) );
}

#endif


QToolButtonInt::~QToolButtonInt()
{
}


void QToolButtonInt::dragEnterEvent( QDragEnterEvent* ev )
{
  // cout << "QToolButtonInt::dragEnterEvent\n";
  emit( dragEntered( param, ev ) );
}


void QToolButtonInt::dragMoveEvent( QDragMoveEvent* ev )
{
  // cout << "QToolButtonInt::dragMoveEvent\n";
  emit( dragMoved( param, ev ) );
}


void QToolButtonInt::dropEvent( QDropEvent* ev )
{
  // cout << "QToolButtonInt::dropEvent\n";
  emit( dropped( param, ev ) );
}


