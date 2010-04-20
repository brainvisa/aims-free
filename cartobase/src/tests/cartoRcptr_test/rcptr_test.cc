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

#define private public
#include <cartobase/smart/rcptr.h>
#include <cartobase/smart/rcobject.h>
#include <cartobase/exception/assert.h>
#include <iostream>


class Bar
{
  public:
    Bar();
    virtual ~Bar();
};


Bar::Bar()
{
  std::cout << ">>> " << this << std::endl;
}


Bar::~Bar()
{
  std::cout << "<<< " << this << std::endl;
}


class Foo : public carto::RCObject
{
  public:
    Foo();
    virtual ~Foo();
};


Foo::Foo()
{
  std::cout << ">>> " << this << std::endl;
}


Foo::~Foo()
{
  std::cout << "<<< " << this << std::endl;
}


carto::rc_ptr<Foo> function1();

carto::rc_ptr<Foo> function1()
{
  return carto::rc_ptr<Foo>( new Foo );
}


carto::rc_ptr<Foo> function2( const carto::rc_ptr<Foo>& p );

carto::rc_ptr<Foo> function2( const carto::rc_ptr<Foo>& p )
{
  return p;
}


carto::rc_ptr<Foo> function3( carto::rc_ptr<Foo> p );

carto::rc_ptr<Foo> function3( carto::rc_ptr<Foo> p )
{
  return p;
}


int main() 
{
  std::cout << "testing rc_ptr constructors (RCObject)..." << std::endl;
  {
    carto::rc_ptr<Foo> x1, x2;
    ASSERT( !x1 );
    ASSERT( !x2 );
    ASSERT( x1 == 0 );
    ASSERT( x2 == 0 );
    ASSERT( x1 == x2 );
  }
  std::cout << "testing rc_ptr constructors..." << std::endl;
  {
    carto::rc_ptr<Bar> x1, x2;
    ASSERT( !x1 );
    ASSERT( !x2 );
    ASSERT( x1 == 0 );
    ASSERT( x2 == 0 );
    ASSERT( x1 == x2 );
  }

  std::cout << "testing rc_ptr constructors (RCObject)..." << std::endl;
  {
    carto::rc_ptr<Foo> x1 = carto::rc_ptr<Foo>( new Foo );
    carto::rc_ptr<Foo> x2 = carto::rc_ptr<Foo>( new Foo );
    ASSERT( x1 );
    ASSERT( x2 );
    ASSERT( !(x1 == 0) );
    ASSERT( !(x2 == 0) );
    ASSERT( x1 != x2 );
  }
  std::cout << "testing rc_ptr constructors..." << std::endl;
  {
    carto::rc_ptr<Bar> x1 = carto::rc_ptr<Bar>( new Bar );
    carto::rc_ptr<Bar> x2 = carto::rc_ptr<Bar>( new Bar );
    ASSERT( x1 );
    ASSERT( x2 );
    ASSERT( !(x1 == 0) );
    ASSERT( !(x2 == 0) );
    ASSERT( x1 != x2 );
  }

  std::cout << "testing rc_ptr constructors (RCObject)..." << std::endl;
  {
    carto::rc_ptr<Foo> x1;
    ASSERT( x1 == 0 );
    {
      carto::rc_ptr<Foo> x2 = carto::rc_ptr<Foo>( new Foo );
      x1 = x2;
      ASSERT( x1 == x2 );
    }
    ASSERT( x1 != 0 );
  }
  std::cout << "testing rc_ptr constructors..." << std::endl;
  {
    carto::rc_ptr<Bar> x1;
    ASSERT( x1 == 0 );
    {
      carto::rc_ptr<Bar> x2 = carto::rc_ptr<Bar>( new Bar );
      x1 = x2;
      ASSERT( x1 == x2 );
    }
    ASSERT( x1 != 0 );
  }

  std::cout << "testing rc_ptr::reset() (RCObject)..." << std::endl;
  {
    carto::rc_ptr<Foo> x1;
    ASSERT( x1 == 0 );
    x1.reset( new Foo );
    ASSERT( x1 != 0 );
    carto::rc_ptr<Foo> x2( x1 );
    ASSERT( x2 != 0 );
    ASSERT( x1 == x2 );
    x1.reset();
    ASSERT( x1 == 0 );
    ASSERT( x2 != 0 );
    x2.reset();
    ASSERT( x2 == 0 );
  }
  std::cout << "testing rc_ptr::reset()..." << std::endl;
  {
    carto::rc_ptr<Bar> x1;
    ASSERT( x1 == 0 );
    x1.reset( new Bar );
    ASSERT( x1 != 0 );
    carto::rc_ptr<Bar> x2( x1 );
    ASSERT( x2 != 0 );
    ASSERT( x1 == x2 );
    carto::rc_ptr<Bar> x3( x1 );
    ASSERT( x3 != 0 );
    ASSERT( x1 == x3 );
    x1.reset();
    ASSERT( x1 == 0 );
    ASSERT( x2 != 0 );
    x2.reset();
    ASSERT( x2 == 0 );
  }

  std::cout << "testing rc_ptr as a return value (RCObject)..." << std::endl;
  {
    carto::rc_ptr<Foo> x1;
    ASSERT( x1 == 0 );
    x1 = function1();
    ASSERT( x1 != 0 );
  }

  std::cout << "testing rc_ptr as an argument (RCObject)..." << std::endl;
  {
    carto::rc_ptr< Foo > x1( new Foo );
    ASSERT( x1 != 0 );
    carto::rc_ptr< Foo > x2( function2( x1 ) );
    ASSERT( x1 == x2 );
    carto::rc_ptr< Foo > x3( function3( x1 ) );
    ASSERT( x1 == x3 );
    carto::rc_ptr< Foo > x4( function2( function2( x1 ) ) );
    ASSERT( x1 == x4 );
  }

}
