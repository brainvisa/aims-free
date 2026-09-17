#ifndef GRAPH_TREE_TVISITOR_H
#define GRAPH_TREE_TVISITOR_H

#include <cartobase/object/attributed.h>

//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

#include <graph/config/graph_config.h>

class Tree;


//--- class declarations ------------------------------------------------------


/**	Abstract base class for visiting \link Tree Trees\endlink. 
	The Visitor pattern can be found in:
	- Erich Gamma, Richard Helm, Ralph Johnson, John M. Vlissides,
	  <a href="http://www.awl.com/cseng/titles/0-201-63361-2/">Design 
          patterns</a>, pp. 331-344. Addison Wesley, 1995.
*/	
class GRAPH_API TreeVisitor
{
public:
  virtual ~TreeVisitor();

  /**@name	Visitor pattern */
  //@{
  virtual void visitAttributedObject( carto::AttributedObject* ao );
  virtual void visitTree( Tree* tree );
  //@}

protected:
  TreeVisitor();

private:
};


//	inline functions


inline TreeVisitor::TreeVisitor()
{
}


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	Abstract base class for visiting constant \link Tree Trees\endlink. 
	The Vistor pattern can be found in:
	- Erich Gamma, Richard Helm, Ralph Johnson, John M. Vlissides,
	  <a href="http://www.awl.com/cseng/titles/0-201-63361-2/">Design 
          patterns</a>, pp. 331-344. Addison Wesley, 1995.
*/
class GRAPH_API ConstTreeVisitor : protected TreeVisitor
{
public:
  /**	@name Constructors, destructor*/
  //@{
  virtual ~ConstTreeVisitor();

  //@}

  /**	@name Visitor pattern*/
  //@{
  virtual void visitAttributedObject( const carto::AttributedObject* ao );
  virtual void visitTree( const Tree* tr );
  //@}

  /** Entry point of the visitor -- DO NOT OVERRIDE

      This method forwards to visitAttributedObject(const
      carto::AttributedObject*) (note the \c const argument). All children of
      ConstTreeVisitor shall override that \c const function, not this one. */
#if __cplusplus >= 201103L
  // ensure that no child class can override this method
  void visitAttributedObject( carto::AttributedObject* ao ) override final
#else
  void visitAttributedObject( carto::AttributedObject* ao )
#endif
  {
    visitAttributedObject(static_cast<const carto::AttributedObject*>(ao));
  };
  /** Entry point of the visitor -- DO NOT OVERRIDE

      This method forwards to visitTree(const carto::Tree*) (note the \c const
      argument). All children of ConstTreeVisitor shall override that \c const
      function, not this one. */
#if __cplusplus >= 201103L
  // ensure that no child class can override this method
  void visitTree( Tree* tr ) override final
#else
  void visitTree( Tree* tr )
#endif
  {
    visitTree(static_cast<const Tree*>(tr));
  };

protected:
  /**	@name Constructors */
  //@{
  /**	The programmer should not call the constructor of an
	abstract base class.
  */
  ConstTreeVisitor();

  //@}

};


//	inline functions


inline ConstTreeVisitor::ConstTreeVisitor()
{
}



#endif


