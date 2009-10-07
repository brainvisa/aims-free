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

// This file is only here for Doxygen (doc generation tool)
// Nothing to do with the code, in fact...

namespace carto
{

  /** \class Object

  <p>Basically, an Object is used whenever it is necessary to use data which type is not known at compile time. Therefore, an Object instance can contain any type of data and it can change at run time. However, C++ is a strongly typed language, it is therefore impossible to handle untyped data. To be able to define a typed generic API for untyped objects, we choosed to base our API on a limited (but extensible) set of <em>atoms</em> and <em>aggregates</em>. Atoms are objects that cannot be split into smaller objects. Aggregates are structures composed of objects (which are either atoms or aggregates).</p>
<p>The basic <code>Object</code> API uses the following standard atoms:
  <ul>
    <li><code>Scalar</code>: represents any numeric value</li>
    <li><code>String</code>: a characters string (like the C++ standard library class <code>std::string</code>)</li>
  </ul>
  </p>
  <p>Atoms can be grouped and/or organised with the following standard aggregates:
  <ul>
    <li><code>Array</code>: an ordered series of objects. The number of objects in the array as well as the type of each element can vary at run time.</li>
    <li><code>Dictionary</code>: a kind of untyped C/C++ <code>structure</code>. A <code>Dictionary</code> is composed of a series of (<em>key</em>, <em>value</em>) pairs where <em>key</em> is a <code>String</code> and <em>value</em> is an object.</li>
  </ul>
  </p>
  <p>All these <em>atoms</em> and <em>aggregates</em> (<em>i.e.</em> all objects) can be accessed through a set of C++ classes inheriting from <code>Interface</code> and described below.</p>
  
  \section main-use Why using Object ?
  
  There are two main reasons that led us to build generic object API. The first one is the need to have untyped dynamic array and dictionaries. This structures are required to store data from the generic <em>minf</em> format. This format contains a <code>Dictionary</code> which content is read from a file at run time. The main idea is to allow the storage of any kind of value in a <em>minf</em> file. The second reason is related to the need to exchange complex data and algorithm between C++ programs and other language (mainly <a href="http://www.python.org">Python</a>. It can be thought as a generalization of the <em>minf</em> requirement. The <em>minf</em> format can be considered as an external (<em>i.e.</em> non C++) language. Therefore, if the tools defined to wrap C++ objects to <em>minf</em> format are generic enough, they can be used to wrap these objects to other languages. Now, the <code>Interface</code> API is generic and language independant, therefore it is being used to create wrappers between C++ and various languages (at least <em>minf-python</em>, <em>minf-XML</em> and <a href="http://www.python.org">Python</a>).
  
  \section interfaces Object's interfaces
  
  <p>The main class that is going to be declared by generic objects users is Object. This class behaves like a pointer to a virtual class called GenericObject. Most people will not have to bother about GenericObject. The only point to remember is just that since Object is pointer alike, almost all of its methods are accessed through the "arrow" syntax (<em>e.g.</em> <code>object->method()</code>) instead of the "dot" syntax (<em>e.g.</em> <code>object.method()</code>).</p>
  
  <p>Object's methods are grouped by interface. Each interface correspond to a set of methods implementing access to a specific atom or aggregate. Interfaces are pure virtual classes derived from Interface. There is a set of standard interfaces which define the API for standard atoms and standard aggregate. It is also possible to define new custom interfaces as explained below.</p>
  
  <p>The standard interface methods differ from custom interface methods. Indeed, standard interfaces are defined before generic objects but custom interfaces are not known when generic objects are defined. Therefore, only standard interfaces methods can be accessed directly from Object's instance (via inheritance). Access to custom interface is described below.</p>


  \subsection standard-interfaces Standard interfaces
  
  <p>Here is a summary of the standard interfaces. Report to each interface documentation for more information:
  <ul>
  <li>ScalarInterface</li>
  <li>StringInterface</li>
  <li>ArrayInterface</li>
  <li>DynArrayInterface</li>
  <li>DictionaryInterface</li>
  <li>SizeInterface</li>
  <li>IterableInterface</li>
  <li>IteratorInterface</li>
  <li>DictionaryIteratorInterface</li>
  </ul>
  </p>
  
  
  \section reference-value How to create an Object ?

  <p>Object behave much like a generic pointer to a real C++ value. Unlike pointers, Object instances can manage memory usage and do all necessary memory allocation/deallocation. In this case the C++ value "belongs" to the Object. Such objects are called <em>value objects</em>. On the other hand, it is sometimes necessary to reference an existing C++ value as a generic object without memory management. Such objects are called <em>reference objects</em>. </p>
  
  <p>There are two functions to create either a value object or a reference. Object::value() returns an Object containing a copy of the value given in parameter whereas Object::reference() returns an object pointing to the object given in parameter. The real type of the object is the type of the parameter (these functions are template).<p>
  
  <p>Example:</p>
  \code
  // This will build a ref-counted copy of the object
  Object	rc1 = Object::value( (int) 12 );
  \endcode

  But if you want to reference an existing object:

  \code
  int existing = 12; // this one should not be destroyed before rc2
  Object rc2 = Object::reference( existing );
  \endcode

  \section usage How to use Objects ?
  
  <p>There are two ways to read or modify the value of an object. If you know the real type of the object, you can access to the stored via the value() and setValue() functions. These functions only work if you try to access the correct data type (\e exact type match: no inheritance is taken into account), otherwise the functions throw an exception. On the other hand, if you do not know the real object type, you must use either the standard interface or the custom interface API.

  \subsection standard-interface-usage How to use Object's standard interfaces

  <b><em>To do...</em></b>

  Object API uses an Interface system: subclasses of Interface 
  implement some "standard" APIs that can be accessed via the 
  GenericObject::getInterface function. All GenericObjects inherit the 
  most common interfaces: ScalarInterface, StringInterface, 
  DictionaryInterface, DynArrayInterface, DictionaryIteratorInterface, and 
  others implied by those listed here.

  A specialized GenericObject which stores (or references) an object that 
  implements a specific Interface will allow to get and use this interface: 
  getInterface() will return a pointer to the stored object matching interface.
  If the stored object itself does not implement the interface, getInterface() 
  will return a null pointer.

  Some Interface may be implemented by the specialized GenericObject, not by 
  the stored object. For instance DictionaryInterface is implemented on 
  map<string, Object> GenericObject specializations even if map doesn't 
  implement it directly.

  \subsection custom-interface-usage How to use Object's custom interfaces
  
  <b><em>To do...</em></b>

  \subsection usage-example Example:

  \code
  #include <cartobase/object/object.h>

  using namespace std;
  using namespace carto;

  // store an int and a string into a STL vector
  Object	obj = Object::value( vector<Object>() );
  obj->insertArrayItem( -1, Object::value( int( 10 ) ) );
  obj->insertArrayItem( -1, Object::value( string( "yup" ) ) );

  // access elements;
  cout << "int   : " << obj->getArrayItem(0)->value<int>() << endl;	// int
  cout << "string: " << obj->getArrayItem(1)->value<string>() << endl;	// string

  // The above code is equivalent to this one, not using Object pointers:
  vector<Object>	vec;
  vec.push_back( Object::value( int( 10 ) ) );
  vec.push_back( Object::value( string( "yup" ) ) );

  // access elements;
  cout << "int   : " << vec[0]->value<int>() << endl;	// int
  cout << "string: " << vec[1]->value<string>() << endl;	// string

  // to change an already stored value, you can either copy the 
  // whole ref counter (detaches it from other references to the older 
  // values)
  vec[0] = Object::value( int( 24 ) );
  // in this case you can also change type
  vec[0] = Object::value( string( "I'm a string now" ) );

  // but if you want to share references and keep the same data type, 
  // it's more efficient to only change the referenced value:
  vec[0]->value<int>() = 32;
  \endcode












  \section input-output Input / Output

  <p>
  Generic objects can be read from and written to disk in two formats:
  <ul>
  <li><b>minf-XML</b>: a XML format</li>
  <li><b>minf-python</b>: a python-like format</li>
  </ul>
  see PythonReader, PythonWriter, \link carto::readMinfXML readMinfXML\endlink.
  </p>
  


  \sa rc_ptr
  \sa Object;
  \sa PythonReader
  \sa PythonWriter
  */


  /** \class GenericObject

  base abstract generic object class.

  GenericObject holds the generic objects API, but functionalities are all
  implemented in subclasses (ValueObject or ReferenceObject).

  GenericObject is normally used through Object, a reference counting pointer
  to a generic object. See Object for the documentation.
  */


  /** \class TypedObject

  storage wrapper, derived but still abstract template class

  TypedObject represents any kind of Generic object, but is still an abstract 
  template class. See ValueObject and ReferenceObject for instanciable classes.
  */


  /** \class ValueObject

  storage wrapper, derived and instanciable template class

  ValueObject is a GenericObject that stores the template object in it, so it 
  owns its value.
  */


  /** \class ReferenceObject

  storage wrapper, derived and instanciable template class

  ReferenceObject is a GenericObject that references an external value object. 
  The external object stays at its original location and doesn't belong to 
  the ReferenceObject. So it must not be deleted while any ReferenceObject 
  still uses it.
  */

}

