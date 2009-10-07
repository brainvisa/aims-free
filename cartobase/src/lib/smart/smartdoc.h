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

namespace carto
{

/*!
  \class rc_ptr
  \brief Reference-counting pointer

  The contained object is deleted when no rc_ptr point to it anymore
*/


/*!
  \class RCObject
  \brief Base class for reference counted objects (intrusive)

  RCObject implements the reference counted object idiom. It allows the 
  building of several \e Ref (see \c const_ref) with the same pointer.

  Full description of the reference counted idiom in:
  \verbatim
  	Scott Meyers
  	More Effective C++: 35 New Ways to Improve Your Programs and Designs
  	Addison Wesley 1996
  	Item 29\endverbatim

  Full description of the observer/subject pattern in:
  \verbatim
	Erich Gamma, Richard Helm, Ralph Johnson, John Vlissides
	Design Patterns: Elements of Reusable Object-Oriented Software
	Addison Wesley 1995
	pp. 293-303\endverbatim

  \sa rc_ptr weak_ptr

In some cases it can be necessary to build a \e Ref from a pointer to an already allocated object. This operation is allowed if and only if the object class derives from \c RCObject.

Here is an example of what \e must \e not be done. The following program build a \c Window class that has a static member named \c shownWindows which contains a list of all shown windows. The \c show() method must add a reference to \c this in the shown windows list. The program will compile but it will crash because the \c Window object will be destroyed several times. The problem is that the \c ref<Window> created in \c main() and the one created in \c Window::show() are in different scope. Therefore they have no way to know about eachother existence. Thus, each \c ref<Window> will use its own reference counter and both will destroy the \c Window upon destruction.
\code
#include <iostream>
#include <list>
#include "rcptr.h"

using namespace carto;

class Window;


class Window
{
public:

  Window();
  ~Window();

  void show();
  void hide();

private:

  static list< ref<Window> > shownWindows;
};

list< ref<Window> > Window::shownWindows;

Window::Window()
{
  cerr << "Construction of a Window" << endl;
}

Window::~Window()
{
  cerr << "Destruction of a Window" << endl;
}

void Window::show()
{
  shownWindows.push_back( this ); // Error, this window will be destroyed several times
}

void Window::hide()
{
  shownWindows.remove( this ); // Error, this window will be destroyed several times
}


int main()
{
  ref<Window> mainWindow = new Window;
  mainWindow->show();
  mainWindow->hide();
}
\endcode

To avoid this problem, it is necessary to include the reference counter in the referenced object class (\c Window in our example). This is done by public inheritance of the class \c RCObject. Therefore, the right version of the program is slightly different from the bad one :

\code
#include <iostream>
#include <list>
#include "rcptr.h"

using namespace carto;

class Window;


class Window : public virtual RCObject
{
public:

  Window();
  ~Window();

  void show();
  void hide();

private:

  static list< ref<Window> > shownWindows;
};

list< ref<Window> > Window::shownWindows;

Window::Window()
{
  cerr << "Construction of a Window" << endl;
}

Window::~Window()
{
  cerr << "Destruction of a Window" << endl;
}

void Window::show()
{
  shownWindows.push_back( this );
}

void Window::hide()
{
  shownWindows.remove( this );
}


int main()
{
  ref<Window> mainWindow = new Window;
  mainWindow->show();
  mainWindow->hide();
}
\endcode

\warning It is strongly recommended to use \c virtual inheritance of \c RCObject to have exactly one reference counter in all derived classes.

\warning When a class \c T is derived from \c RCObject, the constructors and destructors of \c ref<T> and \c const_ref<T> are redefined. This is done statically (at compile time). The \e Ref mecanism does not use any virtual function member.
 */

} // namespace carto


/*!
  \class carto::weak_ptr <cartobase/smart/weakptr.h>
  \brief Observer pointer, observing a shfj::WeakObject

  When the observed object is destroyed, the weak pointer points to null
*/


/*!
  \class carto::WeakObject weakobject.h "cartobase/smart/weakobject.h"
  \brief Base class for weakly referenced objects

  WeakObject implements a specialized observer/subject pattern where weak_ptr
  is the observer.

  When destroyed, observers are notified and point to null

  \sa rc_ptr weak_ptr
*/


/*!
  \class carto::scoped_ptr <cartobase/smart/scopedptr.h>
  \brief Holds a temporary object pointer in a scope.

  Even less capable than std::auto_ptr. For allocating on the heap
  something that must be destructed when exiting the current scope.
  Just like a variable allocated on the stack...
*/
