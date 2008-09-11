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

#include "attributedViewer.h"
#include <aims/listview/qpython.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/finder.h>
#include <aims/data/pheader.h>
#include <cartobase/object/pythonreader.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <aims/qtcompat/qlistview.h>
#include <aims/qtcompat/qpopupmenu.h>

using namespace aims;
using namespace aims::gui;
using namespace carto;
using namespace std;


void Bup::loadObject( const QString & filename )
{
  Object	obj;
////  GenericObject	*go = 0;
  try
    {
      // try direct python format first
      PythonReader	pr( filename.utf8().data() );
      obj = Object( pr.read() );
////      go = obj.get();
    }
  catch( exception & e )
    {
      // try Finder
      Finder	f;
      if( !f.check( filename.utf8().data() ) )
        {
          cerr << e.what() << endl;
          return;
        }
      else
        {
          // taken from AimsFileInfo
          string	type = f.dataType();
          string	format = f.format();
          bool          done = false;

          if( f.objectType() == "genericobject" && type == "any" )
            try
            {
              Reader<GenericObject>  r( filename.utf8().data() );
              obj.reset( r.read() );
              done = true;
            }
            catch( exception & e )
            {
              cerr << "error while loading generic object: " << e.what()
                  << endl;
            }
          if( !done )
          {
            const Header		*h = f.header();
            const PythonHeader	*ph = 0;
            if( !h )
              cerr << "format has no header (surprising...)\n";
            else
              {
                ph = dynamic_cast<const PythonHeader *>( h );
                if( !ph )
                  cerr << "header has no attributes\n";
              }

            if( !ph ) {
                obj = Object::value( PropertySet() );
            } else {
              obj = Object::value( *ph );
            }
            if( ! obj->hasProperty( "object_type" ) )
              obj->setProperty( "object_type", f.objectType() );
            if( !obj->hasProperty( "data_type" ) )
              obj->setProperty( "data_type", type );
            if( !obj->hasProperty( "file_type" ) )
              obj->setProperty( "file_type", format );
            vector<string>	types = f.possibleDataTypes();
            if( types.size() > 1
                && !obj->hasProperty( "possible_data_types" ) )
              obj->setProperty( "possible_data_types", types );
          }
        }
    }

  QPythonPrinter	pp( listview );
  //  Q3ListViewItem	*item = pp.write( *go, true );
  Q3ListViewItem	*item = pp.write( *obj.get(), true );
  Q3ListViewItem	*item2 = item->firstChild();
  if( item2 )
    {
      item->takeItem( item2 );
      delete item;
      listview->insertItem( item2 );
      item = item2;
    }
  item->setText( 0, filename );
  item->setOpen( true );
}


void Bup::loadObject()
{
  QString	s = QFileDialog::getOpenFileName();
  if( !s.isEmpty() )
    loadObject( s );
}


int main( int argc, char** argv )
{
  QApplication		app( argc, argv );
  AimsApplication	aapp( argc, (const char**) argv, 
			      "Displays attributed objects (.minf...) in a " 
                              "list view. AimsAttributedViewer can also " 
                              "show any file header supported by AIMS IO " 
                              "system, just like AimsFileInfo" );
  string		filename;

  aapp.addOption( filename, "-i", "file to display", true );
  try
    {
      aapp.initialize();

      QMainWindow	*mw = new QMainWindow;
      Bup		bup;
      bup.listview = new Q3ListView( mw );
      mw->setCentralWidget( bup.listview );
      app.setMainWidget( mw );

      QPopupMenu	*pop = new QPopupMenu;
      pop->insertItem( "Open...", &bup, SLOT( loadObject() ), 
                       Qt::CTRL + Qt::Key_O );
      pop->insertItem( "Quit", &app, SLOT( quit() ), Qt::CTRL + Qt::Key_Q );
      mw->menuBar()->insertItem( "File", pop );

      pop = new QPopupMenu;
      pop->insertItem( "Clear view", bup.listview, SLOT( clear() ), 
                       Qt::CTRL + Qt::Key_C );
      mw->menuBar()->insertItem( "View", pop );

      bup.listview->addColumn( "attribute/object" );
      bup.listview->addColumn( "type" );
      bup.listview->addColumn( "value" );
      bup.listview->setRootIsDecorated( true );

      if( !filename.empty() )
        bup.loadObject( filename.c_str() );

      mw->show();
      return( app.exec() );
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
    }
  return EXIT_FAILURE;
}


