
#include <graph/tree/tfactory.h>

using namespace std;


TreeFactory::TreeFactory()
{
}


TreeFactory::~TreeFactory()
{
}


Tree* TreeFactory::makeTree( const string & syntax, 
			     bool allowChildren ) const
{
  return( new Tree( allowChildren, syntax ) );
}


TreeFactory* TreeFactory::clone() const
{
  return( new TreeFactory );
}




