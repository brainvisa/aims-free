
PyAims low-level classes and functions
======================================

You will look at this part only when needed, dont't look at it at the beginning. The useful part is accessed via the :doc:`pyaims_highlevel`.


IO system
+++++++++

.. autoclass:: soma.aims.Reader
  :members:

.. autoclass:: soma.aims.Writer
  :members:

  .. method:: writtenObjectType()

  .. method:: writtenObjectDataType()

  .. method:: writtenObjectFullType()

.. autoclass:: soma.aims.Finder
  :members:

  See the :aimsdox:`Finder C++ class <classaims_1_1Finder.html>`

  .. method:: check( filename )

  .. method:: header()

  .. method:: objectType()

  .. method:: dataType()

  .. method:: format()

.. autofunction:: soma.aims.typeCode


Neuroimaging data structures
++++++++++++++++++++++++++++

.. autoclass:: soma.aims.Volume_FLOAT
  :members:

  See the :cartoddox:`Volume C++ documentation <classcarto_1_1Volume.html>`

.. autoclass:: soma.aims.AimsData_FLOAT
  :members:

  See the :aimsdox:`AimsData C++ documentation <classAimsData.html>`

.. autoclass:: soma.aims.Graph
  :members:

  See the :graphdox:`Graph C++ documentation <classGraph.html>`

.. autoclass:: soma.aims.Tree
  :members:

  See the :graphdox:`Tree C++ documentation <classTree.html>`

.. autoclass:: soma.aims.BucketMap_VOID

  See the :aimsdox:`BucketMap C++ documentation <classaims_1_1BucketMap.html>`


General purpose C++ containers
++++++++++++++++++++++++++++++

.. autoclass:: soma.aims.carto
  :members: GenericObject

  This class represents the carto C++ namespace. It is rather used like a module, not as a class.

  .. py:class:: carto.GenericObject

    .. method:: getScalar

    .. method:: getString

    See the :cartobdox:`GenericObject C++ documentation <classcarto_1_1GenericObject.html>`

.. autoclass:: soma.aims.rc_ptr_GenericObject
  :members:

.. autoclass:: soma.aims.Point3df
  :members:

.. autoclass:: soma.aims.AimsVector_FLOAT_3
  :members:

  See the :aimsdox:`AimsVector C++ documentation <classAimsVector.html>`

.. autoclass:: soma.aims.Point2df
  :members:

.. autoclass:: soma.aims.Point3d
  :members:

.. autoclass:: soma.aims.AimsRGB
  :members:

.. autoclass:: soma.aims.AimsRGBA
  :members:


Conversion classes and functions
++++++++++++++++++++++++++++++++

.. autodata:: soma.aims.convertersObjectToPython

.. autoclass:: soma.aims.Converter_Volume_S16_Volume_FLOAT
  :members:

.. autoclass:: soma.aims.ShallowConverter_Volume_S16_Volume_FLOAT
  :members:

