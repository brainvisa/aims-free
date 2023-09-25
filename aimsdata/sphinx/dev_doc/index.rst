
.. default-domain:: cpp

.. highlight:: cpp

============================
AIMS developer documentation
============================

For now this document merely contains one or two tips.

See also the :pyaims:`PyAims documentation <index.html>` and :pyaims:`PyAims tutorial <pyaims_tutorial.html>` for developing exmaples in python language.

The current document is dedicated to the C++ API. It may be used conjointly with the Doxygen :aimsdox:`C++ API reference <index.html>`.

.. contents:: index
   :local:

C++ API docs
------------

:aimsdata:`AIMS library C++ API <doxygen/index.html>`

AIMS is also composed of several sub-projects:

* :cartoddox:`Carto data <index.html>`: neuroimaging data structures and IO implementations
* :aimsdata:`AIMS data <user_doc/index.html>`: image processing basics (this doc)
* :aimsalgo:`AIMS algo <index.html>`: algorithms for AIMS
* :graphdox:`Graph <index.html>`: high-level graphs library
* :aimsgui:`AIMS GUI <index.html>`: `Qt-based <http://qt-project.org>`_ GUI classes for AIMS
* **Soma-IO**: C++ general IO libraries, containing these sub-projects:

  * :cartobdox:`Carto base <index.html>`: C++ general usage library
  * :somaio:`Soma-IO <index.html>`: IO libraries (:somaio:`C++ API <doxygen/index.html>`)


Using graph objects in AIMS
---------------------------

Graphs are generic data structures to represent a set of objects (nodes), which may (or may not) be linked by relations. In a general context, nodes and relations are objects which may have various shapes. This definition is general enough to make graphs able to represent almost any kind of objects collections. This is why we use them in AIMS and Anatomist to represent both structured sets of objects, such as sulci graphs, and less structured sets, such as regions of interests (ROI) sets. The practical interest is to have a generic data structure and IO for a wide range of objects sets, with a common representation.

We use them to represent many types of neuroimaging objects: ROIs, activation clusters, sulci, gyri, fibers, and more abstract object: sulci recognition and morphometry models, and potentially models of the whole world...

In AIMS graphs are represented as :graphdox:`Graph <classGraph.html>` objects. This :graphdox:`Graph <classGraph.html>` class in C++ may be inherited by more specialized structures in specific applications: in sulci-related algorithms for instance.
A graph contains a set of nodes, or vertices, represented in :graphdox:`Vertex <classVertex.html>` objects. Relations, or edges, are represented in :graphdox:`Edge <classEdge.html>`. All ingerit the generic object API, :cartobdox:`GenericObject <classcarto_1_1GenericObject.html>`, and more precisely the :cartobdox:`PropertySet <classcarto_1_1PropertySet.html>` dictionary, which offers both dynamic and builtin properties, and notification capabilities.


File formats
++++++++++++

Historically we use the *.arg* format, an ASCII format which stores the graph structure. Alternately AIMS also supports a python/json-like (*.minf*) format, and a XML variant. Lastly we experiment a database representation for single or multiple grpahs in *SQLite3* format (but this has not be generalized in our applications).

The graph structure file is generally completed by additional data, which are stored in a *.data* directory. Depending on the nature of the objects contained in the graph nodes and relations, the contents of this directory may vary. It typically stores :aimsdox:`meshes <classAimsTimeSurface.html>`, voxels lists (:aimsdox:`Buckets <classaims_1_1BucketMap.html>`), or :cartoddox:`volumes <classcarto_1_1Volume.html>`. But in specialized frameworks it may contain other objects (models and machine learning objects for sulci model graphs for instance).

The *.data* directory has generally two forms for Aims graphical objects: compact ("global") with one file for many objects, or split ("local") with one file per object, which may produce many little files.

The current *.arg* format has limitations in its genericity: properties allowed in it must be declared in external syntax files, which have to be loaded (see :cartobdox:`SyntaxReader <classcarto_1_1SyntaxReader.html>`).


Prerequisite: **cartobase** library basics
++++++++++++++++++++++++++++++++++++++++++

Reference counting
!!!!!!!!!!!!!!!!!!

Graph objects are based on reference counters. These smart pointers are used just like pointers.

::

    #include <cartobase/smart/rcptr.h>

    using namespace carto;

    int main()
    {
      rc_ptr<int>  rc1( new int );
      rc_ptr<int>  rc2 = rc1;
      int a = *rc1 + *rc2;
    }

In short, reference-counting pointers share the ownership of an object and destroy it when nobody needs it any longer: one must never **delete** a reference-counted object, it is done automatically.

Generic objects
!!!!!!!!!!!!!!!

:cartobdox:`GenericObject <classcarto_1_1GenericObject.html>` is the base class for all elements in a graph: graph, node, or relation.

A common, more specific generic object: *Dictionary*, which is a *typedef* from *std::map<std::string, carto::Object>*.

Another more specialized generic dictionary: :cartobdox:`PropertySet <classcarto_1_1PropertySet.html>` which has the same API but supports more interesting featrures.

Both support the :cartobdox:`DictionaryInterface <classcarto_1_1DictionaryInterface.html>` API.

Main generic methods:

* :cartobdox:`setProperty <classcarto_1_1DictionaryInterface.html#aebdd1521fd2076198c5b6a03f8edad59>`
* :cartobdox:`getProperty <classcarto_1_1DictionaryInterface.html#a3980b6186cdec7c0301af66bba5397f4>`
* iteration on properties, using either the *std::map* iterator, or the :cartobdox:`objectIterator <classcarto_1_1IterableInterface.html#a620bd24c973a6ffb719eec03a5d04882>` method and :cartobdox:`DictionaryIteratorInterface <classcarto_1_1DictionaryIteratorInterface.html>` API.

:cartobdox:`Object <classcarto_1_1Object.html>` is a reference counter to a :cartobdox:`GenericObject <classcarto_1_1GenericObject.html>`.


Main classes
++++++++++++

* :graphdox:`Graph <classGraph.html>`
* :graphdox:`Vertex <classVertex.html>`
* :graphdox:`Edge <classEdge.html>`
* :aimsdox:`Reader <classaims_1_1Reader.html>` <Graph>
* :aimsdox:`Writer <classaims_1_1Writer.html>` <Graph>
* :aimsdox:`RoiIterator <classaims_1_1RoiIterator.html>` and inherited classes
* :aimsdox:`MaskIterator <classaims_1_1MaskIterator.html>`


Standard handling
+++++++++++++++++

Reading
!!!!!!!

Avoid using the low-level classes in the :graphdox:`graph library <index.html>` (GraphReader / GraphParser / GraphWriter).

Instead, use the higher-level AIMS IO:

::

    #include <aims/io/reader.h>
    #include <graph/graph/graph.h>
    #include <iostream>

    using namespace aims;
    using namespace std;

    int main( int, char** )
    {
      Reader<Graph> reader( "filename.arg" );
      Graph         graph;
      try
      {
        reader.read( graph );
      }
      catch( exception & e )
      {
        cerr << e.what() << endl;
      }
    }

Iteration on nodes
!!!!!!!!!!!!!!!!!!

::

    Graph::iterator i, e = graph.end();
    Vertex *v;
    for( i=graph.begin(); i!=e; ++i )
    {
      v = *i;
      // do something with v
    }

Complete example 1 source:

.. literalinclude:: ../../code_examples/graphex1.cc
    :linenos:

.. seealso:: the :aimsex:`example 1 <graphex1.cc>`


Iteration on relations
!!!!!!!!!!!!!!!!!!!!!!

::

    Graph::iterator i, e = graph.end();
    Vertex::iterator ie, ee;
    Vertex *v;
    Edge   *edge;
    for( i=graph.begin(); i!=e; ++i )
    {
      v = *i;
      for( ie=v->begin(), ee=v->end(); ie!=ee; ++ie )
      {
        edge = *ie;
        // do something with edge
      }
    }

.. seealso:: illustrated in :aimsex:`example 1 <graphex1.cc>`


Iteration on ROIs
!!!!!!!!!!!!!!!!!

This is done using :aimsdox:`RoiIterator <classaims_1_1RoiIteratorOf_3_01Graph_01_4.html>` <Graph> and :aimsdox:`MaskIteratorOf <classaims_1_1MaskIterator.html>`, which are "simplified" iterators for ROI graphs which contain :aimsdox:`Buckets <classaims_1_1BucketMap.html>`.

Currently :aimsdox:`MaskIterator <classaims_1_1MaskIterator.html>` does not work on graphs shaped as label volumes.

:aimsdox:`RoiIteratorOf <classaims_1_1RoiIteratorOf_3_01AimsData_3_01T_01_4_01_4.html>` <:aimsdox:`AimsData <classAimsData.html>` <T> > however does work on volumes.

Complete example 2 source:

.. literalinclude:: ../../code_examples/graphex2.cc
    :linenos:

.. seealso:: :aimsex:`example 2 <graphex2.cc>` source file


Access to AIMS objects (meshes, buckets...) in graphs
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Using :aimsdox:`GraphManip <classaims_1_1GraphManip.html>` class and :aimsdox:`GraphElementCode <structaims_1_1GraphElementCode.html>` structures.

* When you know what you are looking for in a graph:

  .. literalinclude:: ../../code_examples/graphex3.cc
      :linenos:

  .. seealso:: :aimsex:`example 3 <graphex3.cc>` source file

* exhaustive iteration:

  .. literalinclude:: ../../code_examples/graphex4.cc
      :linenos:

  .. seealso:: :aimsex:`example 4 <graphex4.cc>` source file


Internal representation: local or global (see files in *.data* directory)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

* local: one file per object
* global: one file for all objects with the same category identifier, concatenated using the time dimension

.. seealso:: illusteated in :aimsex:`example 4 <graphex4.cc>`


Internal / external representation: buckets or volumes
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

* Buckets: one bucket per vertex (or relation), with global or local representation
* Volumes: one label volume stored in the graph, and an index in each vertex / relation

Reading / writing
!!!!!!!!!!!!!!!!!

The 3rd optional parameter of :aimsdox:`Reader::read <classaims_1_1Reader.html#abdbaf5545ecb287aed390337b6ec15a9>` is used to specify if reading of internal AIMS objects in graphs is requested, and which ones:

::

    Reader<Graph> reader( "filename.arg" );
    Graph graph;
    reader.read( graph, 0, -1 ); // read all (vertices + relations)
    reader.read( graph, 0, 0 ); // read nothing
    reader.read( graph, 0, 1 ); // read objects in vetices

    // or with more general reading options (more modern):
    Reader<Graph> reader( "filename.arg" );
    Graph graph;
    Object options( new Dictionary );
    options->setProperty( "subobjectsfilter", -1 ); // read all, 0: rien, 1: vertices
    reader.setOptions( options );
    reader.read( graph );

Writing: missing objects which were not read are read before the graph is saved back, so that nothing is missing in the written graph.

::

    #include <aims/io/writer.h>
    // ...
    Writer<Graph> writer( "filename.arg" );
    writer.write( graph );

.. note:: When a graph is re-written in its original directory, but under a different name, it may or may not reuse the same *.data* directory, depending on the *"filename_base"* property in the graph. If this property value is *"\*"*, the *.data* directory will have the same base name as the *.arg*, so will be duplicated. This is the default behaviour in recent AIMS releases.


Specialized graphs (sigraph)
++++++++++++++++++++++++++++

These implementations are in the :sidox:`sigraph <index.html>` library.

* For sulci: :sidox:`FGraph <classsi1_1FGraph.html>`
* Models: :sidox:`MGraph <classsi1_1MGraph.html>`, :sidox:`FRGraph <classsi1_1FRGraph.html>`, primal sketches...

To read a specialized graph, normally it is sufficient to use the Reader object in "factory" mode (assuming appropriate IO plugins are loaded in AIMS): the specialized graph instance is built and returned by the reader:

::

    Reader<Graph> reader( "filename.arg" );
    Graph *graph = reader.read();
    // graph can be a derived graph, like FGraph or FRGraph.

It is also possible to force the graph type and to use the other mode of the Reader:

::

    Reader<Graph> reader( "filename.arg" );
    FGraph graph;
    reader.read( graph );


Another specialized use of Graphs: fiber bundles
++++++++++++++++++++++++++++++++++++++++++++++++

Implementation in the :connectdox:`connectomist <index.html>` library.

The fiber bundles are stored on disk in their own format (*.bundles*, *.bundlesdata*). They have no dedicated data structure in memory: this format has been designed to be read and written on-the-fly, in stream mode (see :connectdox:`BundleProducer <classcomist_1_1BundleProducer.html>`, :connectdox:`BundleListener <classcomist_1_1BundleListener.html>` and derived classes).

However they can also be read as a graph: this is how they are used in Anatomist. In principle when the connectomist plugin is loaded, :aimsdox:`Reader <classaims_1_1Reader.html>` <Graph> can read *.bundles* files.

In bundles graphs nodes, curves or meshes can be found: meshes of segments or triangles.


In Python: pyaims
-----------------

.. default-domain:: py

.. highlight: py

.. currentmodule:: soma.aims

* The :pyaims:`PyAims documentation is here <index.html>`.

* An example: :pyaims:`in pyaims documentation <pyaims_examples.html#graph-manipulation-and-tests>`.

* :class:`~soma.aims.RoiIterator`, :class:`~soma.aims.MaskIterator` and :class:`~soma.aims.GraphManip` are available in Python.


Handling in Anatomist
---------------------

Load a graph

Nomenclatures

Viewing meshes / buckets

Labels

Models, sulci recognition annealing


Future evolutions, planed, dreamed, pending...
----------------------------------------------

* ROI / DOI: link between ROIs and data "textures"
* Simpler handling of AIMS in graphs, using higher-level layers like RoiIterator / MaskIterator
* Complete support of formats XML, minf and SQL
* Unification of concepts of graph and .minf
* Transparent handling of referentials and transformations
* ... ... ... ...



.. toctree::
    :maxdepth: 1

