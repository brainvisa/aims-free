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

#include <aims/selection/labelselector_p.h>
#include <aims/selection/selector.h>
#include <aims/io/selectionr.h>
#include <aims/io/selectionw.h>
#include <aims/listview/qalistview.h>
#include <aims/roi/hie.h>
#include <aims/io/reader.h>
#include <graph/graph/graph.h>
#include <aims/qtcompat/qvbox.h>
#include <aims/qtcompat/qhbox.h>
#include <aims/qtcompat/qdragobject.h>
#include <aims/qtcompat/qpopupmenu.h>
#include <qlayout.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qsplitter.h>
#include <graph/tree/treader.h>
#include <cartobase/stream/sstream.h>
#include <iostream>
#include <vector>
#if QT_VERSION >= 0x040000
using namespace Qt;
#endif

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


SelectBox::SelectBox( LabelSelector* ls, QWidget * parent, const char * name, 
                      Qt::WFlags f )
  : Q3ListView( parent, name, f ), _labelsel( ls )
{
  addColumn( "Label" );
  setRootIsDecorated( true );
  setSelectionMode( Q3ListView::Extended );
  setAcceptDrops( true );
#if QT_VERSION >= 0x040000
  connect( this, 
           SIGNAL( rightButtonPressed( Q3ListViewItem*, const QPoint &, int ) ),
	   SLOT( rightButtonPressed( Q3ListViewItem*, const QPoint &, int ) ));
#else
  connect( this, 
           SIGNAL( rightButtonPressed( QListViewItem*, const QPoint &, int ) ),
	   SLOT( rightButtonPressed( QListViewItem*, const QPoint &, int ) ));
#endif
}

SelectBox::~SelectBox()
{
}

void SelectBox::dragEnterEvent( QDragEnterEvent* event )
{
  //cout << "dragEnterEvent\n";
  event->accept( QTextDrag::canDecode( event ) );
}

void SelectBox::dropEvent( QDropEvent* event )
{
  //cout << "dropEvent\n";
  QString		txt;

  if( QTextDrag::decode( event, txt ) )
    {
      //cout << "dropped!\n";
      //cout << txt << endl;

      QPoint p( viewport()->mapFromParent( event->pos() ) );
      Q3ListViewItem	*item = itemAt( p );
      QString		gname;
      bool		rootsel = !item;

      if( item && item->parent() )
	item = item->parent();
      if( item )
	item->setOpen( true );

      SelectionSet	ss;
      istringstream	istr( txt.utf8().data() );
      SelectionReader	sr( istr );
      try
        {
          sr.read( ss );
          if( rootsel )
            mergeSelection( ss );
          else
            addSelection( ss, item->text(0).utf8().data() );
        }
      catch( exception & e )
        {
          cerr << e.what() << endl;
        }
    }
  else
    event->accept( false );
}


void SelectBox::insertUniqueItem( Q3ListViewItem* parent, const QString & txt )
{
  Q3ListViewItem	*li;
  if( parent )
    li = parent->firstChild();
  else
    li = firstChild();

  while( li )
    {
      if( li->text(0) == txt )
	return;
      li = li->nextSibling();
    }

  if( parent )
    new Q3ListViewItem( parent, txt );
  else
    new Q3ListViewItem( this, txt );
}


SelectionSet SelectBox::selection() const
{
  Q3ListViewItem	*li = firstChild(), *sli;
  SelectionSet	ss;

  while( li )
    {
      sli = li->firstChild();
      Selection		s( li->text( 0 ).utf8().data() );
      while( sli )
	{
	  s.select( sli->text( 0 ).utf8().data() );
	  sli = sli->nextSibling();
	}
      ss.addSelection( s );
      li = li->nextSibling();
    }

  return( ss );
}


void SelectBox::rightButtonPressed( Q3ListViewItem*, const QPoint & pos, int )
{
  QPopupMenu	pop( 0, "right-click" );
  pop.insertItem( tr( "Delete selected items" ), this, 
                  SLOT( deleteSelection() ) );
  pop.insertItem( tr( "Clear selection" ), this, SLOT( clearAll() ) );
  pop.exec( pos, 0 );
}


void SelectBox::clearAll()
{
  clear();
}


void SelectBox::deleteSelection()
{
  Q3ListViewItem	*item = firstChild(), *parent = 0;
  set<Q3ListViewItem *>	todel;

  while( item )
    {
      //cout << "item: " << item->text(0).utf8().data() << endl;
      if( item->isSelected() )
        {
          todel.insert( item );
          item = item->nextSibling();
        }
      else
        {
          parent = item;
          item = item->firstChild();
        }
      while( !item && parent )
        {
          item = parent;
          parent = parent->parent();
          item = item->nextSibling();
        }
    }

  set<Q3ListViewItem *>::iterator	is, es = todel.end();
  for( is=todel.begin(); is!=es; ++is )
    delete *is;
}


void SelectBox::addSelection( const SelectionSet & sel )
{
  SelectionSet::const_iterator	is, es = sel.end();
  Selection::const_iterator	iss, ess;
  Q3ListViewItem			*item;
  string			name;

  for( is=sel.begin(); is!=es; ++is )
    {
      name = is->name();
      if( name.empty() )
        name = newSelectionName( sel );
      item = new Q3ListViewItem( this, name.c_str() );
      for( iss=is->begin(), ess=is->end(); iss!=ess; ++iss )
        new Q3ListViewItem( item, iss->c_str() );
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
      Q3ListViewItem			*pitem;

      for( pitem=firstChild(); pitem && pitem->text(0)!=parent.c_str(); 
           pitem=pitem->nextSibling() ) {}
      if( pitem )
        pitem->setOpen( true );
      return;
    }

  // just in case our parent is not a LabelSelector

  // find parent
  Q3ListViewItem			*pitem;

  for( pitem=firstChild(); pitem && pitem->text(0)!=parent.c_str(); 
       pitem=pitem->nextSibling() ) {}
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
  Q3ListViewItem			*item;
  string			name;
  map<string, Q3ListViewItem *>	items;
  map<string, Q3ListViewItem *>::iterator	ii, ei = items.end();

  for( is=sel.begin(); is!=es; ++is )
    {
      name = is->name();
      if( name.empty() )
        name = newSelectionName( sel, &old );
      ii = items.find( name );
      if( ii == ei )
        {
          for( item=firstChild(); item && item->text(0)!=name.c_str(); 
               item=item->nextSibling() ) {}
          if( !item )
            item = new Q3ListViewItem( this, name.c_str() );
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
  Q3ListView		*hieview;
  QLabel		*modellabel;
  SelectBox		*selection;
  vector<Hierarchy *>	hie;
  int			currentHierarchy;
  Graph			*model;
  QPushButton		*modelsel;
  QPushButton		*modelselbysel;
  QListBox		*modelbox;
  QTabWidget		*tabs;
  SelectionExpander	expander;
};


LabelSelector::LabelSelector( QWidget* parent, const char* name, bool modal, 
			      Qt::WFlags f )
  : QDialog( parent, name, modal, f ), d( new LabelSelector_private )
{
  setCaption( tr( "Labels selector" ) );
  QVBoxLayout	*lay = new QVBoxLayout( this );
  QSplitter *hsplit = new QSplitter( Qt::Vertical, this );
  lay->addWidget( hsplit );
  QTabWidget	*tabs = new QTabWidget( hsplit );
  d->tabs = tabs;
  tabs->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
                                    QSizePolicy::Expanding ) );

  // preselection panel

  QVBox	*presel = new QVBox( tabs );
  tabs->addTab( presel, tr( "Pre-selections" ) );
  presel->setSpacing( 10 );
  presel->setMargin( 10 );
  QHBox	*hb = new QHBox( presel );
  hb->setSpacing( 10 );
  d->presels = new QComboBox( hb );
  QPushButton	*pb = new QPushButton( "...", hb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( loadPreselection() ) );
  hb = new QHBox( presel );
  pb = new QPushButton( tr( "Select" ), hb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( activatePreselection() ) );
  pb = new QPushButton( tr( "+ Select" ), hb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( activateAddPreselection() ) );

  // hierarchy panel

  QVBox	*hier = new QVBox( tabs );
  tabs->addTab( hier, tr( "By hierarchy" ) );
  hier->setSpacing( 10 );
  hier->setMargin( 10 );
  QHBox	*hhb = new QHBox( hier );
  hhb->setSpacing( 10 );
  QLabel	*hl = new QLabel( tr( "Hierarchy :" ), hhb );
  hl->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  d->hierarchies = new QComboBox( hhb );
  QPushButton	*hlb = new QPushButton( "...", hhb );
  hlb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  d->hieview = new QAListView( hier );
  d->hieview->addColumn( "Label" );
  d->hieview->setRootIsDecorated( true );
  d->hieview->setSelectionMode( Q3ListView::Extended );
  d->hieview->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
					  QSizePolicy::Expanding ) );
  connect( hlb, SIGNAL( clicked() ), this, SLOT( loadHierarchy() ) );
  connect( d->hierarchies, SIGNAL( activated( int ) ), this, 
	   SLOT( updateHierarchyView() ) );
  connect( d->hieview, SIGNAL( dragStart( Q3ListViewItem*, Qt::ButtonState ) ), 
	   this, SLOT( startDrag( Q3ListViewItem*, Qt::ButtonState ) ) );

  // model panel

  QVBox	*model = new QVBox( tabs );
  tabs->addTab( model, tr( "Model" ) );
  model->setSpacing( 10 );
  model->setMargin( 10 );
  hb = new QHBox( model );
  hb->setSpacing( 10 );
  d->modellabel = new QLabel( hb );
  d->modellabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  pb = new QPushButton( "...", hb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( loadModel() ) );
  hb = new QHBox( model );
  hb->setSpacing( 10 );
  pb = new QPushButton( tr( "Select (model + hierarchy)" ), hb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( selectModel() ) );
  d->modelsel = pb;
  pb->setEnabled( false );
  pb = new QPushButton( tr( "Select (model + selection)" ), hb );
  pb->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  connect( pb, SIGNAL( clicked() ), this, SLOT( selectModelPlusSel() ) );
  d->modelselbysel = pb;
  pb->setEnabled( false );
  d->modelbox = new QListBox( model );
  d->modelbox->setSelectionMode( QListBox::Extended );
  connect( d->modelbox, SIGNAL( pressed( QListBoxItem* ) ), this, 
           SLOT( startDragModel( QListBoxItem* )) );

  // custom panel
  QLabel	*custom = new QLabel( tr( "Not currently available\n"
					  "(work in progress)" ), tabs );
  tabs->addTab( custom, tr( "Custom" ) );

  // selection panel
  QVBox	*hsel = new QVBox( hsplit );
  hsel->setSpacing( 10 );
  hsel->setMargin( 10 );
  new QLabel( tr( "Selection :" ), hsel );
  new QLabel( tr( "If you don't understand what all this is about, just " 
                  "click \"accept\", it will be OK in most cases" ), hsel );
  d->selection = new SelectBox( this, hsel );
  d->selection->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
					    QSizePolicy::Expanding ) );

  // common OK/cancel
  QHBox		*dial = new QHBox( this );
  dial->setMargin( 10 );
  dial->setSpacing( 10 );
  lay->addWidget( dial );
  QPushButton	*acc = new QPushButton( tr( "Accept" ), dial );
  QPushButton	*can = new QPushButton( tr( "Cancel" ), dial );
  QPushButton	*aut = new QPushButton( tr( "Auto" ), dial );
  QPushButton	*chk = new QPushButton( tr( "Check" ), dial );
  QPushButton	*clr = new QPushButton( tr( "Clear" ), dial );
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
    = QFileDialog::getOpenFileName( QString::null, "*.sel;;*", this, 
				    tr( "Open a pre-selection" ) );
  if( fname != QString::null )
    loadPreselection( fname );
}


void LabelSelector::loadPreselection( const QString & filename )
{
  //cout << "loadPreselection " << filename << endl;
  int	i, n = d->presels->count();
  for( i=0; i<n && d->presels->text(i)!=filename; ++i ) {}
  if( i == n )
    d->presels->insertItem( filename );
#if QT_VERSION >= 300
  d->presels->setCurrentText( filename );
#else
  d->presels->setCurrentItem( n );
#endif
}


void LabelSelector::loadHierarchy()
{
  QString	fname 
    = QFileDialog::getOpenFileName( QString::null, "*.hie;;*", this, 
				    tr( "Select a hierarchy" ) );
  if( fname != QString::null )
    loadHierarchy( fname );
}


void LabelSelector::loadHierarchy( const QString & filename )
{
  //cout << "loadHierarchy " << filename << endl;
  Hierarchy	*h = 0;
  try
    {
      h = new Hierarchy;
      TreeReader	tr( filename.utf8().data(), h->syntax() );
      tr >> *h;
      d->hie.push_back( h );
      d->hierarchies->insertItem( filename );
      d->hierarchies->setCurrentItem( d->hierarchies->count() - 1 );
      updateHierarchyView();
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      delete h;
    }
}


static void appendHierarchy( Q3ListView* lv, Q3ListViewItem* li, const Tree* t )
{
  Q3ListViewItem	*item = 0;
  string	name;
  if( t->getProperty( "name", name ) )
    {
      if( li )
	item = new Q3ListViewItem( li, name.c_str() );
      else
	item = new Q3ListViewItem( lv, name.c_str() );
    }

  Tree::const_iterator	it, et = t->end();

  for( it=t->begin(); it!=et; ++it )
    appendHierarchy( lv, item, (const Tree *) *it );
}


void LabelSelector::updateHierarchyView()
{
  //cout << "updateHierarchyView\n";
  int	item = d->hierarchies->currentItem();
  if( item == d->currentHierarchy )
    return;
  d->currentHierarchy = item;
  d->hieview->clear();

  appendHierarchy( d->hieview, 0, d->hie[ item ] );
  updateModelCaps();
}


void LabelSelector::startDrag( Q3ListViewItem* item, ButtonState state )
{
  //cout << "startDrag\n";
  if( item && item->isSelected() )
    {
      Q3ListViewItem	*li = d->hieview->firstChild();
      SelectionSet	ss;
      Selection		s;
      bool		deep = !(state & ShiftButton);

      while( li )
	{
	  if( li->isSelected() )
	    {
	      if( deep )
		{
		  Selection	s2( li->text( 0 ).utf8().data() );
		  s2.select( li->text( 0 ).utf8().data() );
		  ss.addSelection( s2 );
		}
	      else
		s.select( li->text( 0 ).utf8().data() );
	      //cout << "select " << li->text( 0 ) << endl;
	    }
	  li = li->itemBelow();
	}

      if( !deep )
	ss.addSelection( s );

      ss = d->expander.query( ss, *d->hie[ d->hierarchies->currentItem() ] );

      // encode as string
      ostringstream	ostr;
      SelectionWriter	sw( ostr );
      sw.write( ss );

      QTextDrag	*dg = new QTextDrag( ostr.str().c_str(), this, 
                                     "selectiondrag" );
      dg->dragCopy();
    }
}


void LabelSelector::startDragModel( QListBoxItem* )
{
  //cout << "startDragModel\n";
  int		i, n = d->modelbox->count();
  SelectionSet	sel;
  string	label;

  for( i=0; i<n; ++i )
    if( d->modelbox->isSelected( i ) )
      {
        label = d->modelbox->text( i ).utf8().data();
        Selection	s( label );
        s.select( label );
        sel.addSelection( s );
      }

  Hierarchy	*h = currentHierarchy();
  if( h )
    {
      sel = d->expander.query( sel, *h );
    }

  // encode as string
  ostringstream	ostr;
  SelectionWriter	sw( ostr );
  sw.write( sel );

  QTextDrag	*dg = new QTextDrag( ostr.str().c_str(), this, 
                                     "selectiondrag" );
  dg->dragCopy();
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
      SelectionReader	sr( d->presels->currentText().utf8().data() );
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
        d->presels->removeItem( d->presels->currentItem() );
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
      SelectionReader	sr( d->presels->currentText().utf8().data() );
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
    = QFileDialog::getOpenFileName( QString::null, "*.arg;;*", this, 
				    tr( "Select a model" ) );
  if( fname != QString::null )
    loadModel( fname );
}


void LabelSelector::loadModel( const QString & filename )
{
  Graph		*g;
  try
    {
      Reader<Graph>	rg( filename.utf8().data() );
      g = rg.read( 0 );
      if( g )
        {
          delete d->model;
          d->model = g;
          d->modellabel->setText( filename );
          updateModelCaps();
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
    if( (*iv)->getProperty( "label", label ) && label != "any" 
        && label != "generic" && label != "*" )
      d->modelbox->insertItem( label.c_str() );
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
    d->tabs->setCurrentPage( it->second );
}


Graph* LabelSelector::model()
{
  return d->model;
}


const SelectionExpander & LabelSelector::expander() const
{
  return d->expander;
}


