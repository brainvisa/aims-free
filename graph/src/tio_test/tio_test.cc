/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#include <graph/tree/tree.h>
#include <graph/tree/treader.h>
#include <graph/tree/twriter.h>
#include <cartobase/object/sreader.h>
#include <cartobase/exception/parse.h>
#include <iostream>

using namespace std;
using namespace carto;

int main()
{
  SyntaxSet	as;
  SyntaxReader	ar("./hierarchy.stx");

  ar >> as;

  Tree          tree;
  TreeReader	tr("./sillons_allcolor.hie", as);

  try
    {
      tr >> tree ;
    }
  catch( parse_error & e )
    {
      cerr << e.what() << endl;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }

  cout << "Arbre : " << tree.getInterface<SyntaxedInterface>()->getSyntax() 
       << endl;
  cout << "Nb de noeuds 1er niveau : " << tree.childrenSize() << endl;

  TreeWriter		tw( "./example-bis.tree", as );

  cout << "\nWrite graph in current directory ? (y/n) : " << flush;
  char r;
  cin >> r;
  if( r == 'y' || r == 'Y' )
    {
      try
	{
	  tw << tree;
	}
      catch( parse_error & e )
	{
	  cerr << e.what() << endl;
	}
      catch( exception & e )
	{
	  cerr << e.what() << endl;
	}
    }

  set< Tree * > withLabel = tree.getElementsWith( string( "label" ) );
  cout << "Nb de noeuds avec l'attribut 'label': " << withLabel.size() << endl;

  withLabel.clear();

  withLabel = tree.getElementsWith( string( "label" ), string( "04" ) );
  cout << "Nb de noeuds donc l'attribut 'label' vaut '04': ";
  cout << withLabel.size() << endl;
  cout << "Les noeuds sont: " << endl;

  set< Tree * >::iterator it;
  string aname;
  for ( it=withLabel.begin(); it!=withLabel.end(); ++it )
    {
      (*it)->getProperty( "name", aname );
      cout << "\t" << aname << endl;
    }

  return( 0 );
}
