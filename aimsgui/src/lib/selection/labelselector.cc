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

#include <aims/selection/labelselector_p.h>
#include <aims/selection/selector.h>
#include <aims/io/selectionr.h>
#include <aims/io/selectionw.h>
#include <aims/roi/hie.h>
#include <aims/io/reader.h>
#include <graph/graph/graph.h>
#include <aims/listview/qatreewidget.h>
#include <qlayout.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qsplitter.h>
#include <QMenu>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QTreeWidgetItemIterator>
#include <QListWidget>
#include <QMimeData>
#include <QDrag>
#include <graph/tree/treader.h>
#include <cartobase/stream/sstream.h>
#include <iostream>
#include <vector>
using namespace Qt;

using namespace aims;
using namespace aims::gui;
using namespace std;


namespace
{

  string newSelectionName( const SelectionSet & sel, 
                           const SelectionSet* sel2 = 0 )
  {
    string				name( "unnamed_" ), name2;
    SelectionSet::const_iterator	is, es = sel.end();
    set<string>				old;

    for( is=sel.begin(); is!=es; ++is )
      {
        name2 = is->name();
        if( name2.length() > name.length() 
            && name2.substr( 0, name.length() ) == name )
          old.insert( name2.substr( name.length(), 
                                    name2.length() - name.length() ) );
      }
    if( sel2 )
      for( is=sel2->begin(), es=sel2->end(); is!=es; ++is )
        {
          name2 = is->name();
          if( name2.length() > name.length() 
              && name2.substr( 0, name.length() ) == name )
            old.insert( name2.substr( name.length(), 
                                      name2.length() - name.length() ) );
        }

    unsigned	i = 0;
    bool	found = true;

    do
      {
        ostringstream	ss;
        ss << i;
        ++i;
        if( old.find( ss.str() ) == old.end() )
          {
            found = false;
            name2 = ss.str();
          }
      }
    while( found );

    return name + name2;
  }

}


SelectBox::SelectBox( LabelSelector* ls, QWidget * parent, const char * name )
  : QATreeWidget( parent ), _labelsel( ls )
{
  setObjectName( name );

  setColumnCount( 1 );
  QTreeWidgetItem* hdr = new QTreeWidgetItem;
  setHeaderItem( hdr );
  hdr->setText( 0, tr( "Label" ) );

  setRootIsDecorated( true );
  setSelectionMode( QTreeWidget::ExtendedSelection );
  setItemsExpandable( true );
  setAllColumnsShowFocus( true );

  setAcceptDrops( true );
  connect( this, 
           SIGNAL( itemRightPressed( QTreeWidgetItem*, const QPoint & ) ),
            SLOT( rightButtonPressed( QTreeWidgetItem*, const QPoint & ) ));
}


SelectBox::~SelectBox()
{
}


void SelectBox::dragEnterEvent( QDragEnterEvent* event )
{
  if( event->mimeData()->hasFormat( "text/plain" ) )
    event->acceptProposedAction();
}


void SelectBox::dragMoveEvent( QDragMoveEvent* event )
{
  if( event->mimeData()->hasFormat( "text/plain" ) )
    event->acceptProposedAction();
}


void SelectBox::dropEvent( QDropEvent* event )
{
  // cout << "dropEvent\n";
  QString		txt = event->mimeData()->text();

  if( !txt.isEmpty() )
  {
    // cout << "dropped!\n";
    // cout << txt.toStdString() << endl;

    event->acceptProposedAction();
    QTreeWidgetItem	*item = itemAt( event->pos() );
    QString		gname;
    bool		rootsel = !item;

    if( item && item->parent() )
      item = item->parent();
    if( item )
      item->setExpanded( true );

    SelectionSet	ss;
    istringstream	istr( txt.toStdString() );
    SelectionReader	sr( istr );
    try
      {
        sr.read( ss );
        if( rootsel )
          mergeSelection( ss );
        else
          addSelection( ss, item->text(0).toStdString() );
      }
    catch( exception & e )
      {
        cerr << e.what() << endl;
      }
  }
  else
    event->ignore();
}


void SelectBox::insertUniqueItem( QTreeWidgetItem* parent, 
                                  const QString & txt )
{
  QTreeWidgetItem	*li;
  unsigned i, n;
  if( parent )
  {
    for( i=0, n=parent->childCount(); i<n; ++i )
    {
      li = parent->child( i );
      if( li->text(0) == txt )
        return;
    }
  }
  else
  {
    for( i=0, n=topLevelItemCount(); i<n; ++i )
    {
      li = topLevelItem( i );
      if( li->text(0) == txt )
        return;
    }
  }

  QTreeWidgetItem * newitem = new QTreeWidgetItem;
  newitem->setText( 0, txt );
  if( parent )
    parent->addChild( newitem );
  else
    addTopLevelItem( newitem );
}


SelectionSet SelectBox::selection() const
{
  QTreeWidgetItem	*li, *sli;
  unsigned i, n = topLevelItemCount(), j, m;
  SelectionSet	ss;

  for( i=0; i<n; ++i )
  {
    li = topLevelItem( i );
    m = li->childCount();
    Selection s( li->text( 0 ).toStdString() );
    for( j=0; j<m; ++j )
    {
      sli = li->child( j );
      s.select( sli->text( 0 ).toStdString() );
    }
    ss.addSelection( s );
  }

  return ss;
}


void SelectBox::rightButtonPressed( QTreeWidgetItem*, const QPoint & pos )
{
  QMenu	pop( "right-click" );
  pop.addAction( tr( "Delete selected items" ), this, 
                  SLOT( deleteSelection() ) );
  pop.addAction( tr( "Clear selection" ), this, SLOT( clearAll() ) );
  pop.exec( mapToGlobal( pos ) );
}


void SelectBox::clearAll()
{
  clear();
}


void SelectBox::deleteSelection()
{
  QTreeWidgetItem	*item, *item2;
  set<QTreeWidgetItem *>	todel;
  unsigned i, n = topLevelItemCount(), j, m;

  for( i=0; i<n; ++i )
  {
    item = topLevelItem( i );
    if( item->isSelected() )
      todel.insert( item );
    else
    {
      m = item->childCount();
      for( j=0; j<m; ++j )
      {
        item2 = item->child( j );
        if( item2->isSelected() )
          todel.insert( item2 );
      }
    }
  }

  set<QTreeWidgetItem *>::iterator	is, es = todel.end();
  for( is=todel.begin(); is!=es; ++is )
    delete *is;
}


void SelectBox::addSelection( const SelectionSet & sel )
{
  SelectionSet::const_iterator	is, es = sel.end();
  Selection::const_iterator	iss, ess;
  QTreeWidgetItem		*item, *item2;
  string			name;

  for( is=sel.begin(); is!=es; ++is )
  {
    name = is->name();
    if( name.empty() )
      name = newSelectionName( sel );
    item = new QTreeWidgetItem;
    item->setText( 0, name.c_str() );
    addTopLevelItem( item );
    for( iss=is->begin(), ess=is->end(); iss!=ess; ++iss )
    {
      item2 = new QTreeWidgetItem;
      item2->setText( 0, iss->c_str() );
      item->addChild( item2 );
    }
  }
}


void SelectBox::addSelection( const SelectionSet & sel, const string & parent )
{
  if( _labelsel )
    {
      SelectionSet			nsel;
      SelectionSet::const_iterator	cis, ces = sel.end();
      Selection::const_iterator		iss, ess;

      Selection	s( parent );
      for( cis=sel.begin(); cis!=ces; ++cis )
        for( iss=cis->begin(), ess=cis->end(); iss!=ess; ++iss )
          s.select( *iss );
      nsel.addSelection( s );

      mergeSelection( nsel );

      // find parent
      QTreeWidgetItem			*pitem = 0;
      unsigned i, n = topLevelItemCount();

      for( i=0; i<n; ++i )
      {
        pitem = topLevelItem( i );
        if( pitem->text( 0 ) == parent.c_str() )
          break;
      }
      if( pitem )
        pitem->setExpanded( true );
      return;
    }

  // just in case our parent is not a LabelSelector

  // find parent
  QTreeWidgetItem *pitem = 0;
  unsigned i, n = topLevelItemCount();

  for( i=0; i<n; ++i )
  {
    pitem = topLevelItem( i );
    if( pitem->text(0) == parent.c_str() )
      break;
  }

  if( !pitem )
  {
    addSelection( sel );
    return;
  }

  // find top parent
  while( pitem->parent() )
    pitem = pitem->parent();

  SelectionSet::const_iterator	is, es = sel.end();
  Selection::const_iterator	iss, ess;

  for( is=sel.begin(); is!=es; ++is )
    {
      for( iss=is->begin(), ess=is->end(); iss!=ess; ++iss )
        insertUniqueItem( pitem, iss->c_str() );
    }
}


void SelectBox::mergeSelection( const SelectionSet & sel )
{
  //cerr << "SelectBox::mergeSelection\n";
  SelectionSet		old = selection();
  LabelSelector		*ls = _labelsel;
  if( ls )
    {
      const SelectionExpander	& se = ls->expander();
      se.merge( old, sel, ls->model(), ls->currentHierarchy() );
      clearAll();
      addSelection( old );
      return;
    }

  // just in case our parent is not a LabelSelector
  SelectionSet::const_iterator	is, es = sel.end();
  Selection::const_iterator	iss, ess;
  QTreeWidgetItem		*item;
  string			name;
  map<string, QTreeWidgetItem *>	items;
  map<string, QTreeWidgetItem *>::iterator	ii, ei = items.end();
  unsigned i, n = topLevelItemCount();

  for( is=sel.begin(); is!=es; ++is )
    {
      name = is->name();
      if( name.empty() )
        name = newSelectionName( sel, &old );
      ii = items.find( name );
      if( ii == ei )
        {
          item = 0;
          for( i=0; i<n; ++i )
          {
            item = topLevelItem( i );
            if( item->text(0) == name.c_str() )
              break;
          }
          if( !item )
          {
            item = new QTreeWidgetItem;
            item->setText( 0, name.c_str() );
            addTopLevelItem( item );
          }
          items[ name ] = item;
        }
      else
        item = ii->second;

      for( iss=is->begin(), ess=is->end(); iss!=ess; ++iss )
        insertUniqueItem( item, iss->c_str() );
    }
}


struct LabelSelector::LabelSelector_private
{
  LabelSelector_private() : currentHierarchy( -1 ), model( 0 ) {}

  QComboBox		*presels;
  QComboBox		*hierarchies;
  QTreeWidget		*hieview;
  QLabel		*modellabel;
  SelectBox		*selection;
  vector<Hierarchy *>	hie;
  int			currentHierarchy;
  Graph			*model;
  QPushButton		*modelsel;
  QPushButton		*modelselbysel;
  QListWidget		*modelbox;
  QTabWidget		*tabs;
  SelectionExpander	expander;
};


LabelSelector::LabelSelector( QWidget* parent, const char* name, bool modal, 
			      Qt::WindowFlags f )
  : QDialog( parent, f ), d( new LabelSelector_private )
{
  setWindowTitle( tr( "Labels selector" ) );
  setObjectName( name );
  if( modal )
    setWindowModality( Qt::WindowModal );
  QVBoxLayout	*lay = new QVBoxLayout( this );
  QSplitter *hsplit = new QSplitter( Qt::Vertical, this );
  lay->addWidget( hsplit );
  QTabWidget	*tabs = new QTabWidget( hsplit );
  d->tabs = tabs;
  tabs->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
                                    QSizePolicy::Expanding ) );

  // preselection panel

  QWidget	*presel = new QWidget;
  QVBoxLayout *vlay = new QVBoxLayout( presel );
  presel->setLayout( vlay );
  tabs->addTab( presel, tr( "Pre-selections" ) );
  vlay->setSpacing( 10 );
  vlay->setMargin( 10 );
  QWidget *hb = new QWidget( presel );
  vlay->addWidget( hb );
  QHBoxLayout *hlay = new QHBoxLayout( hb );
  hb->setLayout( hlay );
  hlay->setSpacing( 10 );
  hlay->setMargin( 0 );
  d->presels = new QComboBox( hb );
  hlay->addWidget( d->presels );
  QPushButton	*pb = new QPushButton( "...", hb );
  hlay->addWidget( pb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( loadPreselection() ) );
  hb = new QWidget( presel );
  vlay->addWidget( hb );
  hlay = new QHBoxLayout( hb );
  hb->setLayout( hlay );
  pb = new QPushButton( tr( "Select" ), hb );
  hlay->addWidget( pb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( activatePreselection() ) );
  pb = new QPushButton( tr( "+ Select" ), hb );
  hlay->addWidget( pb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( activateAddPreselection() ) );

  // hierarchy panel

  QWidget *hier = new QWidget( tabs );
  tabs->addTab( hier, tr( "By hierarchy" ) );
  vlay = new QVBoxLayout( hier );
  hier->setLayout( vlay );
  vlay->setSpacing( 10 );
  vlay->setMargin( 10 );
  QWidget *hhb = new QWidget( hier );
  vlay->addWidget( hhb );
  hlay = new QHBoxLayout( hhb );
  hhb->setLayout( hlay );
  hlay->setSpacing( 10 );
  hlay->setMargin( 0 );
  QLabel	*hl = new QLabel( tr( "Hierarchy :" ), hhb );
  hlay->addWidget( hl );
  hl->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  d->hierarchies = new QComboBox( hhb );
  hlay->addWidget( d->hierarchies );
  QPushButton	*hlb = new QPushButton( "...", hhb );
  hlay->addWidget( hlb );
  hlb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );

  d->hieview = new QATreeWidget( hier );
  vlay->addWidget( d->hieview );
  d->hieview->setColumnCount( 1 );
  QTreeWidgetItem* hdr = new QTreeWidgetItem;
  d->hieview->setHeaderItem( hdr );
  hdr->setText( 0, tr( "Label" ) );
  d->hieview->setDragEnabled( true );
  // disable "natural" treewidget drag&drop: we overload it.
  d->hieview->setDragDropMode( QAbstractItemView::NoDragDrop );

  d->hieview->setRootIsDecorated( true );
  d->hieview->setSelectionMode( QTreeWidget::ExtendedSelection );
  d->hieview->setItemsExpandable( true );
  d->hieview->setAllColumnsShowFocus( true );

  d->hieview->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
                                          QSizePolicy::Expanding ) );
  connect( hlb, SIGNAL( clicked() ), this, SLOT( loadHierarchy() ) );
  connect( d->hierarchies, SIGNAL( activated( int ) ), this, 
           SLOT( updateHierarchyView() ) );
  connect( d->hieview, SIGNAL( dragStart( QTreeWidgetItem*, Qt::MouseButtons,
                                          Qt::KeyboardModifiers ) ), 
           this, SLOT( startDrag( QTreeWidgetItem*, Qt::MouseButtons,
                                  Qt::KeyboardModifiers ) ) );

  // model panel

  QWidget *model = new QWidget( tabs );
  tabs->addTab( model, tr( "Model" ) );
  vlay = new QVBoxLayout( model );
  model->setLayout( vlay );
  vlay->setSpacing( 10 );
  vlay->setMargin( 10 );
  hb = new QWidget( model );
  vlay->addWidget( hb );
  hlay = new QHBoxLayout( hb );
  hb->setLayout( hlay );
  hlay->setSpacing( 10 );
  hlay->setMargin( 0 );
  d->modellabel = new QLabel( hb );
  hlay->addWidget( d->modellabel );
  d->modellabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  pb = new QPushButton( "...", hb );
  hlay->addWidget( pb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( loadModel() ) );
  hb = new QWidget( model );
  vlay->addWidget( hb );
  hlay = new QHBoxLayout( hb );
  hb->setLayout( hlay );
  hlay->setSpacing( 10 );
  hlay->setMargin( 0 );
  pb = new QPushButton( tr( "Select (model + hierarchy)" ), hb );
  hlay->addWidget( pb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( selectModel() ) );
  d->modelsel = pb;
  pb->setEnabled( false );
  pb = new QPushButton( tr( "Select (model + selection)" ), hb );
  hlay->addWidget( pb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( selectModelPlusSel() ) );
  d->modelselbysel = pb;
  pb->setEnabled( false );
  d->modelbox = new ModelBox( model );
  vlay->addWidget( d->modelbox );
  d->modelbox->setSelectionMode( QListWidget::ExtendedSelection );
  connect( d->modelbox, SIGNAL( dragStart( QListWidgetItem*, Qt::MouseButtons, 
                                           Qt::KeyboardModifiers ) ), 
           this,
           SLOT( startDragModel( QListWidgetItem*, Qt::MouseButtons, 
                                 Qt::KeyboardModifiers )) );

  // custom panel
  QLabel	*custom = new QLabel( tr( "Not currently available\n"
					  "(work in progress)" ), tabs );
  tabs->addTab( custom, tr( "Custom" ) );

  // selection panel
  QWidget	*hsel = new QWidget( hsplit );
  vlay = new QVBoxLayout( hsel );
  hsel->setLayout( vlay );
  vlay->setSpacing( 10 );
  vlay->setMargin( 10 );
  vlay->addWidget( new QLabel( tr( "Selection :" ), hsel ) );
  vlay->addWidget(
    new QLabel( tr( "If you don't understand what all this is about, just " 
                  "click \"accept\", it will be OK in most cases" ), hsel ) );
  d->selection = new SelectBox( this, hsel );
  vlay->addWidget( d->selection );
  d->selection->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
					    QSizePolicy::Expanding ) );

  // common OK/cancel
  QWidget *dial = new QWidget( this );
  lay->addWidget( dial );
  hlay = new QHBoxLayout( dial );
  dial->setLayout( hlay );
  hlay->setMargin( 10 );
  hlay->setSpacing( 10 );
  QPushButton	*acc = new QPushButton( tr( "Accept" ), dial );
  hlay->addWidget( acc );
  QPushButton	*can = new QPushButton( tr( "Cancel" ), dial );
  hlay->addWidget( can );
  QPushButton	*aut = new QPushButton( tr( "Auto" ), dial );
  hlay->addWidget( aut );
  QPushButton	*chk = new QPushButton( tr( "Check" ), dial );
  hlay->addWidget( chk );
  QPushButton	*clr = new QPushButton( tr( "Clear" ), dial );
  hlay->addWidget( clr );
  acc->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  can->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  aut->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  chk->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  clr->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  acc->setDefault( true );
  connect( acc, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( can, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( aut, SIGNAL( clicked() ), this, SLOT( autoActivate() ) );
  connect( chk, SIGNAL( clicked() ), this, SLOT( check() ) );
  connect( clr, SIGNAL( clicked() ), d->selection, SLOT( clearAll() ) );

  resize( 500, 500 );	// give me a decent size
}


LabelSelector::~LabelSelector()
{
  unsigned	i, n = d->hie.size();
  for( i=0; i<n; ++i )
    delete d->hie[i];
  delete d->model;
  delete d;
}


void LabelSelector::loadPreselection()
{
  QString	fname 
    = QFileDialog::getOpenFileName( this, tr( "Open a pre-selection" ), 
                                    QString(), "Selections (*.sel)" );
  if( !fname.isEmpty() )
    loadPreselection( fname );
}


void LabelSelector::loadPreselection( const QString & filename )
{
  //cout << "loadPreselection " << filename << endl;
  int	i, n = d->presels->count();
  for( i=0; i<n && d->presels->itemText(i)!=filename; ++i ) {}
  if( i == n )
    d->presels->addItem( filename );
  d->presels->setItemText( d->presels->currentIndex(), filename );
}


void LabelSelector::loadHierarchy()
{
  QString	fname 
    = QFileDialog::getOpenFileName( this, tr( "Select a hierarchy" ), 
                                    QString(), "Hierarchies (*.hie)" );
  if( !fname.isEmpty() )
    loadHierarchy( fname );
}


void LabelSelector::loadHierarchy( const QString & filename )
{
  //cout << "loadHierarchy " << filename << endl;
  Hierarchy	*h = 0;
  try
    {
      h = new Hierarchy;
      TreeReader	tr( filename.toStdString(), h->syntax() );
      tr >> *h;
      d->hie.push_back( h );
      d->hierarchies->addItem( filename );
      d->hierarchies->setCurrentIndex( d->hierarchies->count() - 1 );
      updateHierarchyView();
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      delete h;
    }
}


static void appendHierarchy( QTreeWidget* lv, QTreeWidgetItem* li, const Tree* t )
{
  QTreeWidgetItem	*item = 0;
  string	name;
  if( t->getProperty( "name", name ) )
  {
    item = new QTreeWidgetItem;
    item->setText( 0, name.c_str() );
    if( li )
      li->addChild( item );
    else
      lv->addTopLevelItem( item );
  }

  Tree::const_iterator	it, et = t->end();

  for( it=t->begin(); it!=et; ++it )
    appendHierarchy( lv, item, (const Tree *) *it );
}


void LabelSelector::updateHierarchyView()
{
  //cout << "updateHierarchyView\n";
  int	item = d->hierarchies->currentIndex();
  if( item == d->currentHierarchy )
    return;
  d->currentHierarchy = item;
  d->hieview->clear();

  appendHierarchy( d->hieview, 0, d->hie[ item ] );
  updateModelCaps();
}


void LabelSelector::startDrag( QTreeWidgetItem* item, Qt::MouseButtons,
                               Qt::KeyboardModifiers state )
{
  //cout << "startDrag\n";
  if( item && item->isSelected() )
    {
      QTreeWidgetItem	*li;
      QTreeWidgetItemIterator il( d->hieview );
      SelectionSet	ss;
      Selection		s;
      bool		deep = !(state & Qt::ShiftModifier);

      while( *il )
      {
        li = *il;
        if( li->isSelected() )
        {
          if( deep )
          {
            Selection	s2( li->text( 0 ).toStdString() );
            s2.select( li->text( 0 ).toStdString() );
            ss.addSelection( s2 );
          }
          else
            s.select( li->text( 0 ).toStdString() );
          //cout << "select " << li->text( 0 ) << endl;
        }
        ++il;
      }

      if( !deep )
        ss.addSelection( s );

      ss = d->expander.query( ss, *d->hie[ d->hierarchies->currentIndex() ] );

      // encode as string
      ostringstream	ostr;
      SelectionWriter	sw( ostr );
      sw.write( ss );

      QDrag *drag = new QDrag( this );
      QMimeData *mimeData = new QMimeData;
      mimeData->setText( ostr.str().c_str() );
      drag->setMimeData( mimeData );
      Qt::DropAction dropAction = drag->exec( Qt::CopyAction );
      dropAction = dropAction; // compilation warning...
    }
}


void LabelSelector::startDragModel( QListWidgetItem*, Qt::MouseButtons, 
                                    Qt::KeyboardModifiers )
{
  //cout << "startDragModel\n";
  int		i, n = d->modelbox->count();
  SelectionSet	sel;
  string	label;
  QListWidgetItem *item;

  for( i=0; i<n; ++i )
  {
    item = d->modelbox->item( i );
    if( item->isSelected() )
    {
      label = item->text().toStdString();
      Selection	s( label );
      s.select( label );
      sel.addSelection( s );
    }
  }

  Hierarchy	*h = currentHierarchy();
  if( h )
    sel = d->expander.query( sel, *h );

  // encode as string
  ostringstream	ostr;
  SelectionWriter	sw( ostr );
  sw.write( sel );

  QDrag *drag = new QDrag( this );
  QMimeData *mimeData = new QMimeData;
  mimeData->setText( ostr.str().c_str() );
  drag->setMimeData( mimeData );
  Qt::DropAction dropAction = drag->exec( Qt::CopyAction );
  dropAction = dropAction; // compilation warning...
}


void LabelSelector::accept()
{
  check();
  QDialog::accept();
}


void LabelSelector::printSelection( ostream & ostr ) const
{
  SelectionSet	ss = selection();
  SelectionWriter	sw( ostr );
  sw.write( ss );
}


SelectionSet LabelSelector::selection() const
{
  return( d->selection->selection() );
}


void LabelSelector::activatePreselection()
{
  if( d->presels->currentText().isEmpty() )
    return;

  SelectionSet		ss;
  try
    {
      SelectionReader	sr( d->presels->currentText().toStdString() );
      bool		tmp = false;
      if( d->presels->currentText() == "-" )
        {
          sr.open( cin );
          tmp = true;
        }
      sr.read( ss );
      d->selection->clear();
      d->selection->addSelection( ss );
      if( tmp )
        d->presels->removeItem( d->presels->currentIndex() );
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
}


void LabelSelector::activateAddPreselection()
{
  if( d->presels->currentText().isEmpty() )
    return;

  SelectionSet	ss;
  try
    {
      SelectionReader	sr( d->presels->currentText().toStdString() );
      sr.read( ss );
      d->selection->mergeSelection( ss );
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
}


void LabelSelector::loadModel()
{
  QString	fname
    = QFileDialog::getOpenFileName( this, tr( "Select a model" ), 
                                    QString(), "Model graphs (*.arg)" );
  if( !fname.isEmpty() )
    loadModel( fname );
}


void LabelSelector::loadModel( const QString & filename )
{
  Graph		*g;
  try
  {
    Reader<Graph>	rg( filename.toStdString() );
    g = rg.read( 0 );
    if( g )
    {
      delete d->model;
      d->model = g;
      d->modellabel->setText( filename );
      updateModelCaps();
    }
    else
    {
      clog << "Graph " << filename.toStdString() << " cannot be read." << endl;
      return;
    }
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return;
  }

  // model box

  string		label;
  Graph::iterator	iv, ev = g->end();

  d->modelbox->clear();
  for( iv=g->begin(); iv!=ev; ++iv )
    if( ( (*iv)->getProperty( "label", label )
          || (*iv)->getProperty( "name", label ) )
        && label != "any" && label != "generic" && label != "*" )
      d->modelbox->addItem( label.c_str() );
}


void LabelSelector::selectModel()
{
  if( d->model && d->currentHierarchy >= 0 
      && (int) d->hie.size() > d->currentHierarchy )
  {
    Hierarchy	*h = currentHierarchy();
    SelectionSet	ss = d->expander.query( *d->model, *h );
    d->selection->addSelection( ss );
  }
}


void LabelSelector::selectModelPlusSel()
{
  if( d->model )
  {
    SelectionSet	ss = d->expander.query( *d->model, selection() );
    d->selection->clearAll();
    d->selection->addSelection( ss );
  }
}


void LabelSelector::updateModelCaps()
{
  bool	state = false;
  if( currentHierarchy() )
    state = true;
  d->modelsel->setEnabled( state );
  d->modelselbysel->setEnabled( true );
}


Hierarchy* LabelSelector::currentHierarchy()
{
  if( d->currentHierarchy >= 0 && (int) d->hie.size() > d->currentHierarchy )
    return d->hie[ d->currentHierarchy ];
  return 0;
}


void LabelSelector::autoActivate()
{
  SelectionSet		sel = d->selection->selection();
  SelectionSet		*s = &sel;
  Hierarchy		*h = currentHierarchy();

  if( sel.begin() == sel.end() )
    s = 0;
  sel = d->expander.check( d->model, s, h );

  d->selection->clearAll();
  d->selection->addSelection( sel );
}


void LabelSelector::check()
{
  SelectionSet		sel = d->selection->selection();
  SelectionSet		*s = &sel;
  sel = d->expander.check( d->model, s, currentHierarchy() );

  d->selection->clearAll();
  d->selection->addSelection( sel );
}


void LabelSelector::showTab( const string & t )
{
  static map<string, int>	tabs;
  if( tabs.empty() )
    {
      tabs[ "preselection" ] = 0;
      tabs[ "hierarchy" ] = 1;
      tabs[ "model" ] = 2;
      tabs[ "custom" ] = 3;
    }
  map<string, int>::iterator	it = tabs.find( t );
  if( it != tabs.end() )
    d->tabs->setCurrentIndex( it->second );
}


Graph* LabelSelector::model()
{
  return d->model;
}


const SelectionExpander & LabelSelector::expander() const
{
  return d->expander;
}


// ------

struct LabelSelector::ModelBox::Private
{
  Private();
  bool          dragpossible;
  QPoint        dragstartpos;
  Qt::MouseButtons buttons;
  Qt::KeyboardModifiers deadkeys;
};


LabelSelector::ModelBox::Private::Private() : dragpossible( false )
{
}


LabelSelector::ModelBox::ModelBox( QWidget* parent )
  : QListWidget( parent ), d( new Private )
{
}


LabelSelector::ModelBox::~ModelBox()
{
  delete d;
}


void LabelSelector::ModelBox::mousePressEvent( QMouseEvent* event )
{
  if( event->button() == Qt::RightButton )
  {
//     event->accept();
//     QTreeWidgetItem* item = itemAt( event->pos() );
//     if( item )
//       emit itemRightPressed( item, event->pos() );
  }
  else
  {
    d->dragpossible = false;
    QListWidgetItem *item = itemAt( event->pos() );
    if( item && item->isSelected() )
    {
      d->dragpossible = true;
      d->dragstartpos = event->pos();
    }
    d->buttons = event->buttons();
    d->deadkeys = event->modifiers();
  }

  if( !d->dragpossible )
    QListWidget::mousePressEvent( event );
}


void LabelSelector::ModelBox::mouseMoveEvent( QMouseEvent* event )
{
  QPoint p = event->pos();

  if( d->dragpossible )
  {
    QPoint    pd = event->pos() - d->dragstartpos;
    if( pd.x() * pd.x() + pd.y() * pd.y() >= 10 )
    {
      QListWidgetItem *item = itemAt( p );
      if( item && item->isSelected() )
      {
        d->dragpossible = false;
        emit dragStart( item, event->buttons(), event->modifiers() );
        event->accept();
        return;
      }
      d->dragpossible = false;
      QListWidget::mousePressEvent( event );
    }
  }
  if( !d->dragpossible )
    QListWidget::mouseMoveEvent( event );
}


void LabelSelector::ModelBox::mouseReleaseEvent( QMouseEvent* event )
{
  if( d->dragpossible )
    QListWidget::mousePressEvent( event );
  d->dragpossible = false;
  QListWidget::mouseReleaseEvent( event );
}


