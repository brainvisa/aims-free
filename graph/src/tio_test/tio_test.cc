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
