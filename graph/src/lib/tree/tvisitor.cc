#include <graph/tree/tvisitor.h>
#include <graph/tree/tree.h>

using namespace carto;

//=============================================================================
//	M E T H O D S
//=============================================================================

TreeVisitor::~TreeVisitor()
{
}


void TreeVisitor::visitAttributedObject( AttributedObject* )
{
}


void TreeVisitor::visitTree( Tree* tr )
{
  this->visitAttributedObject( tr );
}


//=============================================================================
//	M E T H O D S
//=============================================================================


ConstTreeVisitor::~ConstTreeVisitor()
{
}


void ConstTreeVisitor::visitAttributedObject( const AttributedObject* )
{
}


void ConstTreeVisitor::visitTree( const Tree* tr )
{
  this->visitAttributedObject( tr );
}






