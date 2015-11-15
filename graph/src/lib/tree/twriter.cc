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

//--- header files ------------------------------------------------------------

#include <graph/tree/twriter.h>
#include <cartobase/object/lexicon.h>
#include <graph/tree/tree.h>
#include <cartobase/exception/file.h>

#include <assert.h>

using namespace carto;
using namespace std;

//--- methods -----------------------------------------------------------------

TreeWriter::TreeWriter( const string& filename,
			const SyntaxSet& syntax, 
			const HelperSet& helpers )
  : AttributedWriter( filename, syntax, helpers ), 
    ConstTreeVisitor()
{
}


TreeWriter::TreeWriter( const SyntaxSet& syntax, 
			const HelperSet& helpers )
  : AttributedWriter( syntax, helpers ), 
    ConstTreeVisitor()
{
}


TreeWriter::~TreeWriter()
{
}


void
TreeWriter::visitTree( const Tree* tree )
{
  writeTree( tree, Lexicon::tree() );
}


void TreeWriter::writeTree( const Tree* tree, const string & type )
{
  assert( tree != 0 );

  if( tree->getParent() == 0 )	// top-level tree ?
    {
      (*_stream) << "# tree 1.0\n" << endl;
    }
  // both top-level tree and sub-tree
  (*_stream) << Lexicon::begin() << ' ' << type << ' ' 
             << tree->getInterface<SyntaxedInterface>()->getSyntax()
	     << endl;

  writeAttributes( *tree );

  for ( Tree::const_iterator n = tree->begin(); n != tree->end(); ++n )
    {
      (*_stream) << '\n';
      ((Tree*) *n)->const_accept( *this );
    }

  (*_stream) << '\n'<< Lexicon::end() << endl;
}


TreeWriter& operator << ( TreeWriter & writer, const Tree & tree )
{
  tree.const_accept( writer );
  return writer;
}
