
.. default-domain:: py

.. highlight: py

.. currentmodule:: soma.aims

PyAims high-level functions and classes
=======================================

This section is the "useful" part, what you need to know first.


IO system
+++++++++

.. autofunction:: read

.. autofunction:: write


Neuroimaging data structures
++++++++++++++++++++++++++++

.. autofunction:: soma.aims.Volume

  Volume classes bindings are instantiated on a number of types: U8, S16, U16, S32, U32, FLOAT, DOUBLE, RGB, RGBA. For an example of the volumes API, see the :py:class:`soma.aims.Volume_FLOAT` class.

.. autofunction:: soma.aims.AimsData

  AimsData classes bindings are instantiated on a number of types: U8, S16, U16, S32, U32, FLOAT, DOUBLE, RGB, RGBA. For an example of the AimsData API, see the :py:class:`soma.aims.AimsData_FLOAT` class.

.. autofunction:: soma.aims.AimsTimeSurface

  For an example of mesh surface classes, see the :py:class:`soma.aims.AimsTimeSurface_3` class.

.. autofunction:: soma.aims.TimeTexture

  TimeTexture classes bindings are instantiated on a number of types: S16, S32, U32, FLOAT, POINT2DF. For an example of the TimeTexture API, see the :py:class:`soma.aims.TimeTexture_FLOAT` class.

.. autofunction:: soma.aims.Texture

  Texture classes bindings are instantiated on a number of types: S16, S32, U32, FLOAT, POINT2DF. For an example of the Texture API, see the :py:class:`soma.aims.Texture_FLOAT` class.
  
  .. note::
    
    You normally seldom use the Texture classes directly: most of the time they are (and should be) used through the higher-level :py:func:`soma.aims.TimeTexture` function and classes.

.. autofunction:: soma.aims.BucketMap

  For an example of bucket classes, see the :py:class:`soma.aims.BucketMap_VOID` class, which is actually the only instantiation bound to Python, because textured buckets are not used directly.


Transformations and coordinates systems
+++++++++++++++++++++++++++++++++++++++

.. autoclass:: soma.aims.AffineTransformation3d
  :members:

  See the :aimsdox:`AffineTransformation3d C++ documentation <classaims_1_1AffineTransformation3d.html>`

.. autoclass:: soma.aims.Quaternion
  :members:

.. autoclass:: soma.aims.StandardReferentials
  :members:


General purpose C++ containers
++++++++++++++++++++++++++++++

.. autoclass:: soma.aims.Object
  :members:

  See the :cartobdox:`Object C++ documentation <classcarto_1_1Object.html>`

.. autofunction:: soma.aims.AimsVector

.. autofunction:: soma.aims.stdVector

.. autofunction:: soma.aims.stdSet

.. autofunction:: soma.aims.stdList

.. autofunction:: soma.aims.rc_ptr


Conversion classes and functions
++++++++++++++++++++++++++++++++

.. autofunction:: soma.aims.Converter

.. autofunction:: soma.aims.ShallowConverter


Graphs and ROIs
+++++++++++++++

.. autoclass:: GraphManip
  :members:

  Graph manipulation, which make things easier than the direct use of :class:`Graph`.
  See the :aimsdox:`GraphManip C++ documentation <classaims_1_1GraphManip.html>`

.. autoclass:: soma.aims.RoiIterator

.. autoclass:: soma.aims.MaskIterator


