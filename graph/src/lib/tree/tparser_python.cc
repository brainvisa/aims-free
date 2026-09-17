#include <graph/tree/tparser_python.h>
#include <graph/tree/tree.h>

using namespace carto;
using namespace std;

namespace carto
{
  struct TreeParser_Python::Private
  {
    Private();
    Private( const SyntaxSet & syntax );

    GraphParser_Python	parser;
  };


  TreeParser_Python::Private::Private()
    : parser( SyntaxSet() )
  {
  }


  TreeParser_Python::Private::Private( const SyntaxSet & syntax )
    : parser( syntax )
  {
  }

}


TreeParser_Python::TreeParser_Python( const SyntaxSet & syntax )
  : TreeParser(), d( new Private( syntax ) )
{
}


TreeParser_Python::~TreeParser_Python()
{
  delete d;
}


Tree* TreeParser_Python::read()
{
  //cout << "trying Python format...\n";

  Tree	*t = new Tree( true, "" );
  try
    {
      //cout << "tree created: " << t << endl;
      d->parser.read( *t );
    }
  catch( exception & )
    {
      delete t;
      t = 0;
    }
  //d->parser.detach();
  return t;
}


void TreeParser_Python::read( Tree & tree )
{
  //cout << "trying Python format...\n";
  /* cout << "tree syntax: " << tree.getSyntax() << endl; */

  try
    {
      d->parser.read( tree );
    }
  catch( exception & )
    {
      //d->parser.detach();
      throw;
    }
  //d->parser.detach();
}


void TreeParser_Python::close()
{
  d->parser.close();
}


void TreeParser_Python::attach( rc_ptr<DataSource> ds )
{
  d->parser.attach( ds );
}


void TreeParser_Python::detach()
{
  d->parser.detach();
}


rc_ptr<DataSource> TreeParser_Python::dataSource()
{
  return d->parser.dataSource();
}


