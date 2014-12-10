
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

Volumes
:::::::



.. class:: soma.aims.Volume_U8
.. class:: soma.aims.Volume_S16
.. class:: soma.aims.Volume_U16
.. class:: soma.aims.Volume_S32
.. class:: soma.aims.Volume_U32
.. class:: soma.aims.Volume_RGB
.. class:: soma.aims.Volume_RGBA
.. class:: soma.aims.Volume_DOUBLE
.. class:: soma.aims.Volume_CDOUBLE
.. class:: soma.aims.Volume_CFLOAT
.. autoclass:: soma.aims.Volume_FLOAT
  :members:

.. class:: soma.aims.AimsData_U8
.. class:: soma.aims.AimsData_S16
.. class:: soma.aims.AimsData_U16
.. class:: soma.aims.AimsData_S32
.. class:: soma.aims.AimsData_U32
.. class:: soma.aims.AimsData_RGB
.. class:: soma.aims.AimsData_RGBA
.. class:: soma.aims.AimsData_DOUBLE
.. class:: soma.aims.AimsData_CDOUBLE
.. class:: soma.aims.AimsData_CFLOAT
.. autoclass:: soma.aims.AimsData_FLOAT
  :members:

  .. method:: clone()

  .. method:: dimT()
  
    equivalent to :py:meth:`soma.aims.Volume_FLOAT.getSizeT`

  .. method:: dimX()

    equivalent to :py:meth:`soma.aims.Volume_FLOAT.getSizeX`

  .. method:: dimY()

    equivalent to :py:meth:`soma.aims.Volume_FLOAT.getSizeY`

  .. method:: dimZ()
  
    equivalent to :py:meth:`soma.aims.Volume_FLOAT.getSizeZ`

  .. method:: fill( value )

  .. method:: fillBorder( value )

  .. method:: header()

  .. method:: maxIndex()

  .. method:: maximum()

  .. method:: minIndex()

  .. method:: minimum()

  .. method:: setSizeT( sizet )
  
    This is the **voxel size**, not the volume size as in :py:class::`soma.aims.Volume_FLOAT`.

  .. method:: setSizeX( sizex )

  .. method:: setSizeXYZT( size )

  .. method:: setSizeY( sizey )

  .. method:: setSizeZ( sizez )

  .. method:: setValue( value, posx, posy = 0, posz = 0, post = 0 )

  .. method:: sizeT()

    This is the **voxel size**, not the volume size as in :py:class::`soma.aims.Volume_FLOAT`.

  .. method:: sizeX()

  .. method:: sizeY()

  .. method:: sizeZ()

  .. method:: value( posx, posy = 0, posz = 0, post = 0 )

  .. method:: volume()
  
    Returns the underlying :py:class::`soma.aims.Volume_FLOAT` object.


Graphs and trees
::::::::::::::::

.. autoclass:: soma.aims.Graph
  :members:

  See the :graphdox:`Graph C++ documentation <classGraph.html>`

.. autoclass:: soma.aims.Tree
  :members:

  See the :graphdox:`Tree C++ documentation <classTree.html>`


Meshes and textures
:::::::::::::::::::

.. autoclass:: soma.aims.AimsTimeSurface_3
  :members:

  See the :aimsdox:`AimsTimeSurface C++ documentation <classAimsTimeSurface.html>`
  
  .. note::
  
    Compared to the C++ classes, the texture type template argument has beed dropped from the Python bindings, because textures are handled via separate objects (see :py:func:`soma.aims.TimeTexture`) and the builtin texture is actually never used.

.. autoclass:: soma.aims.TimeTexture_FLOAT
  :members:

  See the :aimsdox:`TimeTexture C++ documentation <classTimeTexture.html>`

.. autoclass:: soma.aims.Texture_FLOAT
  :members:

  See the :aimsdox:`Texture C++ documentation <classTexture.html>`


"Buckets"
:::::::::

.. autoclass:: soma.aims.BucketMap_VOID

  See the :aimsdox:`BucketMap C++ documentation <classaims_1_1BucketMap.html>`


General purpose C++ containers
++++++++++++++++++++++++++++++

Generic object
::::::::::::::

.. autoclass:: soma.aims.carto
  :members: GenericObject

  This class represents the carto C++ namespace. It is rather used like a module, not as a class.

  .. py:class:: carto.GenericObject

    .. method:: getScalar

    .. method:: getString

    See the :cartobdox:`GenericObject C++ documentation <classcarto_1_1GenericObject.html>`

.. autoclass:: soma.aims.rc_ptr_GenericObject
  :members:


Vectors and points
::::::::::::::::::

.. autoclass:: soma.aims.Point3df
  :members:

.. autoclass:: soma.aims.AimsVector_FLOAT_3
  :members:

  See the :aimsdox:`AimsVector C++ documentation <classAimsVector.html>`

.. autoclass:: soma.aims.Point2df
  :members:

.. autoclass:: soma.aims.Point3d
  :members:


RGB
:::

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

