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

#include <cstdlib>
#include <aims/selection/labelselector.h>
#include <aims/selection/selector.h>
#include <aims/getopt/getopt2.h>
#include <qapplication.h>


using namespace aims;
using namespace aims::gui;
using namespace carto;
using namespace std;


int main( int argc, char** argv )
{
  AimsApplication	aapp( argc, (const char**) argv, 
			      "Selects ROI labels from a nomenclature\n"
			      "(to be used as input for ROI and morphometry "
			      "commands)" );
  string		model, hier, presel;
  bool			batch = false;

  aapp.addOption( model, "-m", "Load a model", true );
  aapp.addOption( hier, "-n", "Load a nomenclature hierarchy", true );
  aapp.addOption( presel, "-p", "Load a pre-made selection", true );
  aapp.addOption( batch, "-b", "Batch mode: process the input selection/model "
                  "if possible and immediately outputs a selection without " 
                  "user interaction", true );
  aapp.alias( "--model", "-m" );
  aapp.alias( "--nomenclature", "-n" );
  aapp.alias( "--hierarchy", "-n" );
  aapp.alias( "--presel", "-p" );
  aapp.alias( "--batch", "-b" );

  try
    {
      aapp.initialize();

      if( batch )
      {
        Selector sel;
        if( !model.empty() )
          sel.loadModel( model );
        if( !hier.empty() )
          sel.loadNomenclature( hier );
        if( !presel.empty() )
          sel.loadPreSelection( presel );
        sel.printSelection();

        return EXIT_SUCCESS;
      }

      QApplication          app( argc, argv );
      LabelSelector
        *ls = new LabelSelector( 0, "LabelSelector", false );
      ls->resize( ls->sizeHint() );
      app.setMainWidget( ls );
      ls->show();
      if( !model.empty() )
        {
          ls->loadModel( model.c_str() );
          ls->showTab( "model" );
        }
      if( !hier.empty() )
        ls->loadHierarchy( hier.c_str() );
      if( !presel.empty() )
        {
          ls->loadPreselection( presel.c_str() );
          ls->activatePreselection();
        }
      if( model.empty() && presel.empty() && !hier.empty() )
        ls->showTab( "hierarchy" );
      if( ls->exec() )
        ls->printSelection();
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


