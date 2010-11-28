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

#include <aims/attributedview/attributedview.h>
#include <aims/listview/qpython.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/finder.h>
#include <qmainwindow.h>
#include <qapplication.h>
#include <aims/qtcompat/qlistview.h>

using namespace aims;
using namespace carto;
using namespace std;



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
      AttributedView *mw = new AttributedView();
      mw->load( filename );
      app.setMainWidget( mw );
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


