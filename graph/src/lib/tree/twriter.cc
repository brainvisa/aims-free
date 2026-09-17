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
