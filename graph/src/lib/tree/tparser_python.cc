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


